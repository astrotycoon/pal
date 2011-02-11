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

#ifndef LIBPAL_PAL_FILE_APPLE_H_
#define LIBPAL_PAL_FILE_APPLE_H_

#if defined(PAL_PLATFORM_APPLE)

class palFile
{
public:
  palFile();
  ~palFile();

  bool OpenForReading(const char* filename);
  bool OpenForWritingTruncate(const char* filename);
  void Close();
  bool IsOpen();

  unsigned char* CopyContentsAsString(uint64_t* string_length);
  unsigned char* CopyContents(uint64_t* string_length);

  uint64_t Seek(palFileSeek p, int64_t offset);
  uint64_t Read(void* buffer, uint64_t num_bytes);
  uint64_t Write(const void* buffer, uint64_t num_bytes);
  uint64_t GetPosition();
  uint64_t GetSize();

protected:
  int handle_;
  uint64_t file_size_;
};

class palFileMapping
{
public:
  palFileMapping();
  ~palFileMapping();

  bool Init(int fileh);

  bool Map();
  bool UnMap();

  unsigned char* GetPointer() const;
  long GetSize() const;

protected:
  int mmaph;
  unsigned char* address;
  long size;
  int fileh;
};

#endif // defined(PAL_PLATFORM_APPLE)

#endif  // LIBPAL_PAL_FILE_APPLE_H_