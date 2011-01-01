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

#ifndef LIBPAL_PAL_ATOMIC_WINDOWS_H_
#define LIBPAL_PAL_ATOMIC_WINDOWS_H_

#include <Windows.h>

PAL_INLINE int palAtomicInc(palAtomic* atomic) {
  return InterlockedIncrement((volatile LONG*)atomic);
}

PAL_INLINE int palAtomicDec(palAtomic* atomic) {
  return InterlockedDecrement((volatile LONG*)atomic);
}

PAL_INLINE int palAtomicAdd(int i, palAtomic* atomic) {
  return InterlockedExchangeAdd((volatile LONG*)atomic, i) + i;
}

PAL_INLINE int palAtomicSub(int i, palAtomic* atomic) {
  return InterlockedExchangeAdd((volatile LONG*)atomic, -i) - i;
}

PAL_INLINE bool palAtomicCAS(int old_value, int new_value, palAtomic* atomic) {
  return InterlockedCompareExchange((volatile LONG*)atomic, new_value, old_value) == old_value;
}

PAL_INLINE bool palAtomicDecAndTest(palAtomic* atomic) {
  return InterlockedDecrement((volatile LONG*)atomic) == 0;
}

PAL_INLINE void palAtomicMemoryBarrier() {
#if defined(PAL_ARCH_32BIT)
  long Barrier;
  __asm xchg Barrier, eax
#else // PAL_ARCH_64BIT
  __faststorefence();
#endif
}


#endif  // LIBPAL_PAL_ATOMIC_WINDOWS_H_