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

#ifndef __PAL_MEMORY_H__
#define __PAL_MEMORY_H__

#include "libpal/pal_types.h"

#ifndef NULL
#define NULL 0
#endif

void* palMalloc(int size);
void  palFree(void* p);

/* Returns an address 'p' from the system heap, where p+offset is aligned to alignment */
/* Only guarantees that there will be size bytes available */
void* palMallocAligned(uint32_t size, uint32_t alignment, uint32_t offset);
/* Returns an address 'p' from the system heap, where p is aligned to alignment */
/* Only guarantees that there will be size bytes available */
void* palMallocAligned(uint32_t size, uint32_t alignment);
/* Frees the pointer returned from one of the above allocation routines */
void  palFreeAligned(void* ptr);

void* palMallocAligned4(uint32_t size);
void* palMallocAligned16(uint32_t size);
void* palMallocAligned128(uint32_t size);

void* pal_memmove(void* destination, const void* source, int bytes);
void* pal_memcopy(void* destination, const void* source, int bytes);
void* pal_memset(void* destination, unsigned char byte, int bytes);
void* pal_memzero(void* destination, int bytes);
int   pal_next_power_of_two(int x);

#endif
