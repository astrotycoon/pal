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

#include <string.h>
#include <stdlib.h>

#include "libpal/pal_debug.h"
#include "libpal/pal_align.h"
#include "libpal/pal_memory.h"


void* palMalloc(int size) {
  return malloc(size);
}

void  palFree(void* p) {
  free(p);
}

void* palMallocAligned(uint32_t size, uint32_t alignment, uint32_t offset) {
  if (alignment < 1) {
    // fix alignment
    alignment = 1;
  }

  const int pointerSize = sizeof(void*);
  const int requestedSize = size + alignment - 1 + pointerSize;

  void* original = malloc(requestedSize);

  
  if (!original)
    return NULL;

  uintptr_t address = (uintptr_t)original;
  address += pointerSize;
  uintptr_t aligned_address = palAlign(address + offset, alignment) - offset;
  void* aligned = (void*)aligned_address;
  *(void**)((char*)aligned-pointerSize) = original;

  return aligned;
}

void* palMallocAligned(uint32_t size, uint32_t alignment) {
  const int pointerSize = sizeof(void*);
  const int requestedSize = size + alignment - 1 + pointerSize;

  void* original = malloc(requestedSize);

  if (!original)
    return NULL;

  void* start = (char*)original + pointerSize;
  void* aligned = palAlign(start, alignment);
  *(void**)((char*)aligned-pointerSize) = original;

  return aligned;
}

void* palMallocAligned4 (uint32_t size) {
  void* ptr = palMallocAligned(size, 4);
  palAssert(palIsAligned4(ptr));
  return ptr;
}

void* palMallocAligned16(uint32_t size) {
  void* ptr = palMallocAligned(size, 16);
  palAssert(palIsAligned16(ptr));
  return ptr;
}

void* palMallocAligned128(uint32_t size) {
  void* ptr = palMallocAligned(size, 128);
  palAssert(palIsAligned128(ptr));
  return ptr;
}

void palFreeAligned(void* ptr) {
  void* original = *(void**)((char*)ptr-sizeof(void*));
  free(original);
}

void palMemoryCopyBytes(void* destination, const void* source, int bytes) {
  intptr_t d_start = reinterpret_cast<intptr_t>(destination);
  intptr_t d_end = d_start + bytes;
  intptr_t s_start = reinterpret_cast<intptr_t>(source);
  intptr_t s_end = s_start + bytes;
  if ((s_start >= d_start && s_start < d_end) ||
    (d_start >= s_start && d_start < s_end)) {
    // regions overlap
    memmove(destination, source, bytes);
  } else {
    memcpy(destination, source, bytes);
  }
}

void palMemoryZeroBytes(void* destination, int bytes) {
  memset(destination, 0, bytes);
}

void palMemorySetBytes(void* destination, unsigned char byte, int bytes) {
  memset(destination, byte, bytes);
}

int palRoundToPowerOfTwo(int x) {
	const int start = 1;
	int n = start;

	while (n < x) {
		n <<= 1;
	}

	return n;
}


// globally override new operator

void* operator new(size_t size) {
  // we default to 16-byte alignment.
  return palMallocAligned16(size);
}

void* operator new(size_t size, const palNewAlignment& alignment) {
  return palMallocAligned(size, alignment.alignment);
}

void operator delete(void* p) {
  palFreeAligned(p);
}

void* operator new[](size_t size) {
  return palMallocAligned16(size);
}

void* operator new[](size_t size, const palNewAlignment& alignment) {
  return palMallocAligned(size, alignment.alignment);
}

void operator delete[](void* p) {
  palFreeAligned(p);
}