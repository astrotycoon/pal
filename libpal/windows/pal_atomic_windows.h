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

#pragma once

#include "libpal/pal_platform.h"
#include "libpal/pal_debug.h"

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

template <> 
struct palAtomicIntegral<int64_t> {
private:
  volatile long long value_;
  // disable copying
  palAtomicIntegral(const palAtomicIntegral&);
  palAtomicIntegral& operator=(const palAtomicIntegral&);
public:
  palAtomicIntegral() {
    InterlockedExchange64(&value_, 0);
  }
  palAtomicIntegral(int64_t initial_value) {
    InterlockedExchange64(&value_, initial_value);
  }

  /* Atomically stores new_value into *this */
  void Store(int64_t new_value)  volatile {
    InterlockedExchange64(&value_, new_value);
  }

  /* Atomically fetches the value stored in *this and returns it */
  int64_t Load() const volatile {
    return InterlockedExchangeAdd64((volatile long long*)&value_, 0);
  }

  /* Atomically store a new value and return old value */
  int64_t Exchange(int64_t new_value) volatile {
    return InterlockedExchange64(&value_, new_value);
  }

  /* Atomically compare the value with expected, and store new_value if they are equal */
  /* Returns true if value was expected, false otherwise */
  /* Updates expected with value read */
  bool CompareExchange(int64_t& expected, int64_t new_value) volatile {
    int64_t actual;
    actual = InterlockedCompareExchange64(&value_, new_value, expected);
    bool success = actual == expected;
    expected = actual;
    return success;
  }

  /* Atomically fetches the value stored in *this and returns it */
  operator int64_t() const volatile {
    return InterlockedExchangeAdd64((volatile long long*)&value_, 0);
  }

  /* Atomically fetch the value, perform the operation and return the original value */
  int64_t FetchAdd(int64_t i) volatile {
    return InterlockedExchangeAdd64(&value_, i);
  }

  int64_t FetchSub(int64_t i) volatile {
    return InterlockedExchangeAdd64(&value_, -i);
  }

  int64_t FetchAnd(int64_t i) volatile {
    return InterlockedAnd64(&value_, i);
  }

  int64_t FetchOr(int64_t i) volatile {
    return InterlockedOr64(&value_, i);
  }

  int64_t FetchXor(int64_t i) volatile {
    return InterlockedXor64(&value_, i);
  }

  /* Atomically perform pre and post increment and decrement */
  int64_t operator++() volatile {
    return FetchAdd(1) + 1;
  }

  int64_t operator++(int) volatile {
    return FetchAdd(1);
  }

  int64_t operator--() volatile {
    return FetchSub(1) - 1;
  }

  int64_t operator--(int) volatile {
    return FetchSub(1);
  }

  /* Atomically perform the operations, returning the resulting value */
  int64_t operator+=(int64_t i) volatile {
    return FetchAdd(i) + i;
  }

  int64_t operator-=(int64_t i) volatile {
    return FetchSub(i) - i;
  }

  int64_t operator&=(int64_t i) volatile {
    return FetchAnd(i) & i;
  }
  int64_t operator|=(int64_t i) volatile {
    return FetchOr(i) | i;
  }
  int64_t operator^=(int64_t i) volatile {
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
PAL_INLINE int32_t palAtomicReferenceCount::Unref() volatile {
  int32_t new_count = _InterlockedDecrement(&count_);
  palAssert(new_count >= 0);
  return new_count;
}

/* Atomically increases reference count */
PAL_INLINE int32_t palAtomicReferenceCount::Ref() volatile {
  return _InterlockedIncrement(&count_);
}

/* Atomically loads and returns the reference count */
PAL_INLINE int32_t palAtomicReferenceCount::Load() const volatile {
  return _InterlockedExchangeAdd((volatile long*)&count_, 0);
}

PAL_INLINE palAtomicAddress::palAtomicAddress() {
  InterlockedExchangePointer(&value_, 0);
}

PAL_INLINE palAtomicAddress::palAtomicAddress(void* ptr) {
  InterlockedExchangePointer(&value_, ptr);
}

/* Atomically stores new_value into *this */
PAL_INLINE void palAtomicAddress::Store(void* new_value)  volatile {
  InterlockedExchangePointer(&value_, new_value);
}

/* Atomically fetches the value stored in *this and returns it */
PAL_INLINE void* palAtomicAddress::Load() const volatile {
#if defined(PAL_ARCH_32BIT)
  return reinterpret_cast<void*>(_InterlockedExchangeAdd((volatile long*)&value_, 0));
#else
  return reinterpret_cast<void*>(InterlockedExchangeAdd64((volatile long long*)&value_, 0));
#endif
}

/* Atomically store a new value and return old value */
PAL_INLINE void* palAtomicAddress::Exchange(void* new_value) volatile {
  return InterlockedExchangePointer(&value_, new_value);
}

/* Atomically compare the value with expected, and store new_value if they are equal */
/* Returns true if value was expected, false otherwise */
/* Updates expected with value read */
PAL_INLINE bool palAtomicAddress::CompareExchange(void*& expected, void* new_value) volatile {
  void* actual;
  actual = InterlockedCompareExchangePointer(&value_, new_value, expected);
  bool success = actual == expected;
  expected = actual;
  return success;
}

/* Atomically fetches the value stored in *this and returns it */
PAL_INLINE palAtomicAddress::operator void*() const volatile {
#if defined(PAL_ARCH_32BIT)
  return reinterpret_cast<void*>(_InterlockedExchangeAdd((volatile long*)&value_, 0));
#else
  return reinterpret_cast<void*>(InterlockedExchangeAdd64((volatile long long*)&value_, 0));
#endif
}

PAL_INLINE void* palAtomicAddress::FetchAdd(ptrdiff_t i) volatile {
#if defined(PAL_ARCH_32BIT)
  return reinterpret_cast<void*>(_InterlockedExchangeAdd((volatile long*)&value_, i));
#else
  return reinterpret_cast<void*>(InterlockedExchangeAdd64((volatile long long*)&value_, i));
#endif
}

PAL_INLINE void* palAtomicAddress::FetchSub(ptrdiff_t i) volatile {
#if defined(PAL_ARCH_32BIT)
  return reinterpret_cast<void*>(_InterlockedExchangeAdd((volatile long*)&value_, -i));
#else
  return reinterpret_cast<void*>(InterlockedExchangeAdd64((volatile long long*)&value_, -i));
#endif
}

PAL_INLINE void palAtomicMemoryBarrier() {
#if defined(PAL_ARCH_32BIT)
  long Barrier;
  __asm xchg Barrier, eax
#else // PAL_ARCH_64BIT
  __faststorefence();
#endif
}
