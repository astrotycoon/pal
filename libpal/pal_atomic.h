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

#ifndef LIBPAL_PAL_ATOMIC_H_
#define LIBPAL_PAL_ATOMIC_H_

typedef struct { int value; } palAtomic;

PAL_INLINE int palAtomicInc(palAtomic* atomic);
PAL_INLINE int palAtomicDec(palAtomic* atomic);

PAL_INLINE int palAtomicAdd(int i, palAtomic* atomic);
PAL_INLINE int palAtomicSub(int i, palAtomic* atomic);

PAL_INLINE int palAtomicGet(palAtomic* atomic) { return atomic->value; }
PAL_INLINE void palAtomicSet(int i, palAtomic* atomic) { atomic->value = i; }

PAL_INLINE bool palAtomicCAS(int old_value, int new_value, palAtomic* atomic);
PAL_INLINE bool palAtomicDecAndTest(palAtomic* atomic);

PAL_INLINE void palAtomicMemoryBarrier();

#include "libpal/pal_atomic_inl.h"

#endif  // LIBPAL_PAL_ATOMIC_H_
