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

#include "libpal/tlsf/tlsf.h"

#include "libpal/pal_types.h"
#include "libpal/pal_debug.h"
#include "libpal/pal_heap_allocator.h"

void palHeapAllocator::Create(void* mem, uint32_t size) {
  palSpinlockTake(&lock_);
  tlsf_create(mem, size);
  internal_ = mem;
  palSpinlockRelease(&lock_);
}

void* palHeapAllocator::Allocate(size_t size, int flags) {
  palSpinlockTake(&lock_);
  void* r = tlsf_malloc(internal_, size);
  palSpinlockRelease(&lock_);
  return r;
}

void* palHeapAllocator::Allocate(size_t size, size_t alignment, size_t alignment_offset, int flags) {
  palAssert(alignment_offset == 0); // don't support it
  palSpinlockTake(&lock_);
  void* r = tlsf_memalign(internal_, alignment, size);
  palSpinlockRelease(&lock_);
  return r; 
}

void  palHeapAllocator::Deallocate(void* p, size_t size) {
  palSpinlockTake(&lock_);
  tlsf_free(internal_, p);
  palSpinlockRelease(&lock_);
}

const char* palHeapAllocator::GetName() const {
  return name_;
}

void        palHeapAllocator::SetName(const char* name) {
  name_ = name;
}