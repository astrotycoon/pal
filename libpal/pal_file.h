/*
	Copyright (c) 2009 John McCutchan <john@johnmccutchan.com>

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

#ifndef LIBPAL_PAL_FILE_H_
#define LIBPAL_PAL_FILE_H_

#include "libpal/pal_platform.h"
#include "libpal/pal_types.h"

// guarantees that there is a NULL character at the end
unsigned char* palCopyFileContentsAsString(const char* filename, uint64_t* string_length);

// does not append a NULL character to end of file stream
unsigned char* palCopyFileContents(const char* filename, uint64_t* length);

enum palFileSeek
{
  kFileSeekBegin = 0,
  kFileSeekCurrent = 1,
  kFileSeekEnd = 2,
  kFileSeekNUM = 3,
};

#if defined(PAL_PLATFORM_WINDOWS)
#include "libpal/windows/pal_file_windows.h"
#else
#warning no file implementation for your platform
#endif

#endif  // LIBPAL_PAL_FILE_H_
