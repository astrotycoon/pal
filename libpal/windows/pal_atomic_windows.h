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

#define _WINSOCKAPI_
#include <Windows.h>
#include <intrin.h>

template <> 
struct palAtomicIntegral<int32_t> {
private:
  volatile long value_;
  // disable copying
  palAtomicIntegral(const palAtomicIntegral&);
  palAtomicIntegral& operator=(const palAtomicIntegral&);
public:
  palAtomicIntegral() {
    _InterlockedExchange(&value_, 0);
  }
  palAtomicIntegral(int32_t initial_value) {
    _InterlockedExchange(&value_, initial_value);
  }

  /* Atomically stores new_value into *this */
  void Store(int32_t new_value) volatile {
    _InterlockedExchange(&value_, new_value);
  }

  /* Atomically fetches the value stored in *this and returns it */
  int32_t Load() const volatile {
    return _InterlockedExchangeAdd((volatile long*)&value_, 0);
  }

  /* Atomically store a new value and return old value */
  int32_t Exchange(int32_t new_value) volatile {
    return _InterlockedExchange(&value_, new_value);
  }

  /* Atomically compare the value with expected, and store new_value if they are equal */
  /* Returns true if value was expected, false otherwise */
  /* Updates expected with value read */
  bool CompareExchange(int32_t& expected, int32_t new_value) volatile {
    int32_t actual;
    actual = _InterlockedCompareExchange(&value_, new_value, expected);
    bool success = actual == expected;
    expected = actual;
    return success;
  }

  /* Atomically fetches the value stored in *this and returns it */
  operator int32_t() const volatile {
    return _InterlockedExchangeAdd((volatile long*)&value_, 0);
  }

  /* Atomically fetch the value, perform the operation and return the original value */
  int32_t FetchAdd(int32_t i) volatile {
    return _InterlockedExchangeAdd(&value_, i);
  }

  int32_t FetchSub(int32_t i) volatile {
    return _InterlockedExchangeAdd(&value_, -i);
  }

  int32_t FetchAnd(int32_t i) volatile {
    return _InterlockedAnd(&value_, i);
  }
  int32_t FetchOr(int32_t i) volatile {
    return _InterlockedOr(&value_, i);
  }
  int32_t FetchXor(int32_t i) volatile {
    return _InterlockedXor(&value_, i);
  }

  /* Atomically perform pre and post increment and decrement */
  int32_t operator++() volatile {
    return FetchAdd(1) + 1;
  }

  int32_t operator++(int) volatile {
    return FetchAdd(1);
  }

  int32_t operator--() volatile {
    return FetchSub(1) - 1;
  }

  int32_t operator--(int) volatile {
    return FetchSub(1);
  }

  /* Atomically perform the operations, returning the resulting value */
  int32_t operator+=(int32_t i) volatile {
    return FetchAdd(i) + i;
  }
  int32_t operator-=(int32_t i) volatile {
    return FetchSub(i) - i;
  }
  int32_t operator&=(int32_t i) volatile {
    return FetchAnd(i) & i;
  }
  int32_t operator|=(int32_t i) volatile {
    return FetchOr(i) | i;
  }
  int32_t operator^=(int32_t i) volatile {
    return FetchXor(i) ^ i;
  }
};

PAL_INLINE palAtomicFlag::palAtomicFlag() {
  _InterlockedExchange(&flag_, 0);
}

PAL_INLINE bool palAtomicFlag::TestAndSet() volatile {
  return _InterlockedCompareExchange(&flag_, 1, 0) == 1;
}

PAL_INLINE void palAtomicFlag::Clear() volatile {
  _InterlockedExchange(&flag_, 0);
}

PAL_INLINE palAtomicReferenceCount::palAtomicReferenceCount() {
  _InterlockedExchange(&count_, 0);
}

/* Atomically decrements reference count */
/* Returns true if reference count hit 0 */
PAL_INLINE bool palAtomicReferenceCount::Unref() volatile {
  return _InterlockedDecrement(&count_) == 0;
}

/* Atomically increases reference count */
PAL_INLINE void palAtomicReferenceCount::Ref() volatile {
  _InterlockedIncrement(&count_);
}

/* Atomically loads and returns the reference count */
PAL_INLINE uint32_t palAtomicReferenceCount::Load() const volatile {
  return _InterlockedExchangeAdd((volatile long*)&count_, 0);
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