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

#include "libpal/pal_platform.h"
#include "libpal/pal_debug.h"
#include "libpal/pal_memory.h"
#include "libpal/pal_mem_blob.h"
#include "libpal/pal_file.h"

unsigned char *palCopyFileContentsAsString(const char *filename, uint64_t* size_out) {
  palFile pf;
  bool r;
  r = pf.OpenForReading(filename);
  if (!r) {
    return NULL;
  }
  unsigned char* b = pf.CopyContentsAsString(size_out);
  pf.Close();
  return b;
}

unsigned char* palCopyFileContents(const char *filename, uint64_t* size_out) {
  palFile pf;
  bool r;
  r = pf.OpenForReading(filename);
  if (!r) {
    return NULL;
  }
  unsigned char* b = pf.CopyContents(size_out);
  pf.Close();
  return b;
}

int palCopyFileContents(const char* filename, palMemBlob* blob) {
  palFile pf;
  bool r;
  blob->buffer = NULL;
  blob->buffer_size = 0;
  r = pf.OpenForReading(filename);
  if (!r) {
    return -1;
  }
  uint64_t len;
  blob->buffer = pf.CopyContents(&len);
  blob->buffer_size = len;
  pf.Close();
  return 0;
}
