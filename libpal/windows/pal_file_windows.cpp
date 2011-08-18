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
#include "libpal/pal_string.h"


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
