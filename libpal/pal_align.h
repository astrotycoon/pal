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

#ifndef LIBPAL_PAL_ALIGN_H_
#define LIBPAL_PAL_ALIGN_H_

#include "libpal/pal_types.h"

#if defined(PAL_COMPILER_MICROSOFT)
#define PAL_ALIGN(alignment, declaration) __declspec(align(alignment)) declaration
#define PAL_ALIGNOF(type) __alignof(type)
#elif defined(PAL_COMPILER_GNU)
#define PAL_ALIGN(alignment, declaration) declaration __attribute__ ((aligned (alignment)))
template<class T>
struct StructForAlignOf {
  unsigned char first_byte;
  T obj;
};
#define PAL_ALIGNOF(type) ( offsetof( StructForAlignOf< type >, obj ) )
#else
#error Cannot define PAL_ALIGN because compiler is unknown
#endif



void*     palAlign(void* ptr, uint32_t alignment);
uintptr_t palAlign(uintptr_t x, uintptr_t alignment);

bool palIsAligned(void* ptr, uint32_t alignment);
bool palIsAligned4(void* ptr);
bool palIsAligned16(void* ptr);
bool palIsAligned128(void* ptr);

void* palMallocAligned(uint32_t size, uint32_t alignment);
void* palMallocAligned4(uint32_t size);
void* palMallocAligned16(uint32_t size);
void* palMallocAligned128(uint32_t size);
void  palFreeAligned(void* ptr);

#endif  // LIBPAL_PAL_ALIGN_H_