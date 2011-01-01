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

#include "libpal/pal_allocator.h"

#define kPalSystemHeapAlignedAllocatorMagic 0xf3f3dead

palSystemHeapAlignedAllocator::palSystemHeapAlignedAllocator(const char* name) {
  magic_ = kPalSystemHeapAlignedAllocatorMagic;
  name_ = name;
}

palSystemHeapAlignedAllocator::palSystemHeapAlignedAllocator(const palSystemHeapAlignedAllocator& x) {
  magic_ = x.magic_;
  name_ = x.name_;
}

palSystemHeapAlignedAllocator::palSystemHeapAlignedAllocator(const palSystemHeapAlignedAllocator& x, const char* name) {
  magic_ = x.magic_;
  name_ = name;
}

palSystemHeapAlignedAllocator& palSystemHeapAlignedAllocator::operator=(const palSystemHeapAlignedAllocator& x) {
  if (this == &x) return *this;
  name_ = x.name_;
  magic_ = x.magic_;

  return *this;
}

void* palSystemHeapAlignedAllocator::Allocate(size_t size, size_t alignment, size_t alignment_offset, int flags) {
  return palMallocAligned(size, alignment);
}

void  palSystemHeapAlignedAllocator::Deallocate(void* p, size_t size) {
  palFreeAligned(p);
}

const char* palSystemHeapAlignedAllocator::GetName() const {
  return name_;
}

void palSystemHeapAlignedAllocator::SetName(const char* name) {
  name_ = name;
}

bool operator==(const palSystemHeapAlignedAllocator& a, const palSystemHeapAlignedAllocator& b) {
  return a.magic_ == b.magic_;
}

bool operator!=(const palSystemHeapAlignedAllocator& a, const palSystemHeapAlignedAllocator& b) {
  return a.magic_ != b.magic_;
}