/*
	Copyright (c) 2011 John McCutchan <john@johnmccutchan.com>

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

#include "libpal/pal_memory.h"
#include "libpal/pal_atomic.h"
#include "libpal/pal_align.h"

class palAllocatorInterface {
public:
  palAllocatorInterface(const char* name) : _name(name) {
    _memory_allocations.Store(0);
    _memory_used.Store(0);
  }
  virtual ~palAllocatorInterface() {
    AssertOnLeak();
  }

  /* INTERFACE */
  virtual void* Allocate(uint32_t size, int alignment = 8) = 0;
  virtual void Deallocate(void* ptr) = 0;
  virtual uint32_t GetSize(void* ptr) const = 0;

  template <class T>
  T* Construct() {
    void* allocation = Allocate(sizeof(T), PAL_ALIGNOF(T));
    return new (allocation) T();
  }

  template <class T, class P1>
  T* Construct(const P1& p1) {
    void* allocation = Allocate(sizeof(T), PAL_ALIGNOF(T));
    return new (allocation) T(p1);
  }

  template <typename T, class P1, class P2>
  T* Construct(const P1& p1, const P2& p2) {
    void* allocation = Allocate(sizeof(T), PAL_ALIGNOF(T));
    return new (allocation) T(p1, p2);
  }

  template <typename T, class P1, class P2>
  T* Construct(const P1& p1, P2& p2) {
    void* allocation = Allocate(sizeof(T), PAL_ALIGNOF(T));
    return new (allocation) T(p1, p2);
  }

  template <class T>
  void Destruct(T* p) {
    if (p) {
      p->~T();
      Deallocate(p);
    }
  }
  /* END INTERFACE */

  const char* GetName() const {
    return _name;
  }

  int GetNumberOfAllocations() {
    return _memory_allocations.Load();
  }
  int GetMemoryAllocated() {
    return _memory_used.Load();
  }

protected:
  void ReportMemoryAllocation(void* p, uint32_t size) {
    //palPrintf("[%s] %p %d +\n", _name, p, size);
    _memory_allocations.FetchAdd(1);
    _memory_used.FetchAdd(size);
  }
  void ReportMemoryDeallocation(void* p, uint32_t size) {
    //palPrintf("[%s] %p %d -\n", _name, p, size);
    _memory_allocations.FetchSub(1);
    _memory_used.FetchSub(size);
  }
  void AssertOnLeak() {
    if (_memory_allocations.Load() != 0 || _memory_used.Load() != 0) {
      palPrintf("Allocator %s has leaks [%d allocations - %d bytes]\n", _name, _memory_allocations.Load(), _memory_used.Load());
    }
  }
private:
  palAtomicInt32 _memory_allocations;
  palAtomicInt32 _memory_used;
  PAL_DISALLOW_COPY_AND_ASSIGN(palAllocatorInterface);
  const char* _name;
};