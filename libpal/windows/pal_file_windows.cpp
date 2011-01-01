/*
	Copyright (c) 2010 John McCutchan <john@johnmccutchan.com>

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.

	3. This notice may not be removed or altered from any source
	distribution.
*/

#include "libpal/pal_platform.h"

#if defined(PAL_PLATFORM_WINDOWS)
#define _CRT_SECURE_NO_WARNINGS
#include <io.h>
#include <share.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "libpal/pal_debug.h"
#include "libpal/pal_memory.h"
#include "libpal/pal_file.h"

palFile::palFile() {
  handle_ = INVALID_HANDLE_VALUE;
  file_size_ = 0;
}

palFile::~palFile() {
  Close();
}

bool palFile::OpenForReading(const char* filename) {
  HANDLE fileh = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  handle_ = fileh;
  if (fileh == INVALID_HANDLE_VALUE) {
    return false;
  }
  uint32_t low_size;
  uint32_t high_size;
  low_size = GetFileSize(fileh, (LPDWORD)&high_size);
  file_size_ = low_size | ((uint64_t)high_size << 32);
  return true;
}

bool palFile::OpenForWritingTruncate(const char* filename) {
  HANDLE fileh = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  handle_ = fileh;
  file_size_ = 0;
  return fileh != INVALID_HANDLE_VALUE;
}

void palFile::Close () {
  if (handle_ != INVALID_HANDLE_VALUE) {
    CloseHandle(handle_);
    handle_ = INVALID_HANDLE_VALUE;
  }
}

bool palFile::IsOpen() {
  return handle_ != INVALID_HANDLE_VALUE;
}

unsigned char* palFile::CopyContentsAsString(uint64_t* string_length)
{
  unsigned char *buf = NULL;
  buf = (unsigned char *)palMalloc((int)GetSize()+1);
  if (!buf)
    return NULL;
  uint64_t read_size = Read(buf, GetSize());
  palAssert(read_size == GetSize());
  buf[GetSize()] = '\0';
  if (string_length)
    *string_length = GetSize();
  return buf;
}

unsigned char* palFile::CopyContents(uint64_t* string_length) {
  unsigned char *buf = NULL;
  buf = (unsigned char *)palMalloc((int)GetSize());
  if (!buf)
    return NULL;
  uint64_t read_size = Read(buf, GetSize());
  palAssert(read_size == GetSize());
  if (string_length)
    *string_length = GetSize();
  return buf;
}

uint64_t palFile::Seek(palFileSeek p, int64_t offset ) {
  const uint32_t method_array[kFileSeekNUM] = { FILE_BEGIN, FILE_CURRENT, FILE_END };
  uint32_t method = method_array[p];

  BOOL r;
  uint64_t result;

  LARGE_INTEGER i, o;
  i.QuadPart = offset;
  r = SetFilePointerEx(handle_, i, &o, method);
  result = o.QuadPart;
  return result;
}

uint64_t palFile::Read(void* buffer, uint64_t num_bytes) {
  BOOL r;
  uint32_t bytes_read;

  r = ReadFile(handle_, buffer, (DWORD)num_bytes, (LPDWORD)&bytes_read, NULL);

  return bytes_read;
}

uint64_t palFile::Write(const void* buffer, uint64_t num_bytes) {
  BOOL r;
  uint32_t bytes_written;

  r = WriteFile(handle_, buffer, (DWORD)num_bytes, (LPDWORD)&bytes_written, NULL);

  return bytes_written;
}

uint64_t palFile::GetPosition() {
  return Seek(kFileSeekCurrent, 0);
}

uint64_t palFile::GetSize() {
  return file_size_;
}

palFileMapping::palFileMapping() {
	this->fileh = INVALID_HANDLE_VALUE;
	this->mmaph = INVALID_HANDLE_VALUE;
	this->address = NULL;
	this->fileh = fileh;
	this->size = 0;
}

PAL_NO_INLINE bool palFileMapping::Init(HANDLE fileh)
{
	this->mmaph = INVALID_HANDLE_VALUE;
	this->address = NULL;
	this->fileh = fileh;
	this->size = GetFileSize(fileh, NULL);
  return true;
}

PAL_NO_INLINE palFileMapping::~palFileMapping() {
	if (address != NULL) {
		UnmapViewOfFile(address);
	}
	if (mmaph != INVALID_HANDLE_VALUE) {
		CloseHandle(mmaph);
	}
}

PAL_NO_INLINE bool palFileMapping::Map() {
  if (fileh == INVALID_HANDLE_VALUE) {
		return false;
  }

  if (size == INVALID_FILE_SIZE) {
		return false;
  }

	mmaph = CreateFileMapping (fileh, NULL, PAGE_READONLY, 0, 0, NULL);
  if (mmaph == INVALID_HANDLE_VALUE) {
		return false;
  }

	void* r = MapViewOfFile (mmaph, FILE_MAP_READ, 0, 0, 0);
	if (!r) {
		return false;
	}
	address = (unsigned char*)r;
	return true;
}

PAL_NO_INLINE bool palFileMapping::UnMap() {
  if (address != NULL) {
		UnmapViewOfFile(address);
  }
  if (mmaph != INVALID_HANDLE_VALUE) {
		CloseHandle(mmaph);
  }
	address = NULL;
	mmaph = INVALID_HANDLE_VALUE;
	return true;
}

unsigned char* palFileMapping::GetPointer() const {
	return address;
}

long palFileMapping::GetSize() const {
	return size;
}
