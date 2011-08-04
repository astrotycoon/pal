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

#include "libpal/libpal.h"

class palAllocatorInterface {
public:
  palAllocatorInterface(const char* name) : _name(name), _memory_allocations(0), _memory_used(0) {
  }
  virtual ~palAllocatorInterface() {
    AssertOnLeak();
  }

  const char* GetName() const {
    return _name;
  }

  /* INTERFACE */
  virtual void* Allocate(uint32_t size, int alignment) = 0;
  virtual void Deallocate(void* ptr) = 0;
  virtual uint32_t GetSize(void* ptr) const = 0;
  /* END INTERFACE */

  int GetNumberOfAllocations() {
    return _memory_allocations;
  }
  int GetMemoryAllocated() {
    return _memory_used;
  }

protected:
  void ReportMemoryAllocation(uint32_t size) {
    _memory_used += size;
    _memory_allocations++;
  }
  void ReportMemoryDeallocation(uint32_t size) {
    _memory_used -= size;
    _memory_allocations--;
  }
  void AssertOnLeak() {
    palAssert(_memory_allocations == 0 && _memory_used == 0);
  }
private:
  uint32_t _memory_allocations;
  uint32_t _memory_used;
  PAL_DISALLOW_COPY_AND_ASSIGN(palAllocatorInterface);
  const char* _name;
};