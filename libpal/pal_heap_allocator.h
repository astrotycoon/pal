/*
	Copyright (c) 2010 John McCutchan <john@johnmccutchan.com>

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

#ifndef LIBPAL_PAL_HEAP_ALLOCATOR_H__
#define LIBPAL_PAL_HEAP_ALLOCATOR_H__

#include "libpal/pal_allocator.h"
#include "libpal/pal_spinlock.h"

class palHeapAllocator
{
  void* internal_;
  palSpinlock lock_;
  const char* name_;
public:
  
  palHeapAllocator() : internal_(NULL) {
    palSpinlockInit(&lock_);
    name_ = "palHeapAllocator";
  }
  
  palHeapAllocator(void* mem, uint32_t size) {
    palSpinlockInit(&lock_);
    name_ = "palHeapAllocator";
    Create(mem, size);
  }


  void Create(void* mem, uint32_t size);

  void* Allocate(size_t size, int flags = kPalAllocationFlagNone);
  void* Allocate(size_t size, size_t alignment, size_t alignment_offset = 0, int flags = kPalAllocationFlagNone);
  void  Deallocate(void* p, size_t size);

  const char* GetName() const;
  void        SetName(const char* name);
};

#endif  // LIBPAL_PAL_HEAP_ALLOCATOR_H__