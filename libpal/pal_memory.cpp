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

void* pal_memmove(void* destination, const void* source, int bytes) {
	return memmove(destination, source, bytes);
}

void* pal_memcopy(void* destination, const void* source, int bytes) {
	return memcpy(destination, source, bytes);
}

void* pal_memset(void* destination, unsigned char byte, int bytes) {
	return memset(destination, byte, bytes);
}

void* pal_memzero(void* destination, int bytes) {
	return pal_memset(destination, 0, bytes);
}

int pal_next_power_of_two(int x) {
	const int start = 1;
	int n = start;

	while (n < x) {
		n <<= 1;
	}

	return n;
}