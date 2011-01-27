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

#ifndef LIBPAL_PAL_ALLOCATOR_H__
#define LIBPAL_PAL_ALLOCATOR_H__

#include "libpal/pal_types.h"
#include "libpal/pal_memory.h"
#include "libpal/pal_align.h"

#define kPalAllocationFlagNone 0x0
#define kPalAllocationFlagGame 0x1
#define kPalAllocationFlagMultipleFrame 0x2
#define kPalAllocationFlagFrame 0x4

/* The palAllocator will always be used as a template argument
 * so you don't inherit from this class, but just implement this
 * interface. Avoids virtual function calls.
 */

class palAllocator {
protected:
  uint32_t magic_;
  const char* name_;
public:
  palAllocator(const char* name = "SystemHeapAlignedAllocator");
  palAllocator(const palAllocator& x);
  palAllocator(const palAllocator& x, const char* name);

  palAllocator& operator=(const palAllocator& x);

  void* Allocate(size_t size, int flags = kPalAllocationFlagNone);
  void* Allocate(size_t size, size_t alignment, size_t alignment_offset = 0, int flags = kPalAllocationFlagNone);
  void  Deallocate(void* p, size_t size);

  const char* GetName() const;
  void        SetName(const char* name);

  friend bool operator==(const palAllocator& a, const palAllocator& b);
  friend bool operator!=(const palAllocator& a, const palAllocator& b);
};

bool operator==(const palAllocator& a, const palAllocator& b);
bool operator!=(const palAllocator& a, const palAllocator& b);

#define PAL_CLASS_ALLOCATOR_OVERRIDE_SYSTEM_HEAP_ALIGNED_ALIGNOF(TName) \
  static void* operator new(size_t size) {                         \
    palAllocator a;                     \
    return a.Allocate(size, PAL_ALIGNOF(TName)); \
  }                                                                \
  static void operator delete(void* p) {                           \
    palAllocator a;                     \
    return a.Deallocate(p, 0);              \
  } \
  static void* operator new[](size_t size) { \
    palAllocator a; \
    return a.Allocate(size, PAL_ALIGNOF(TName)); \
  } \
  static void operator delete[](void* p) { \
    palAllocator a; \
    return a.Deallocate(p, 0); \
  }

#define PAL_CLASS_ALLOCATOR_OVERRIDE_SYSTEM_HEAP_ALIGNED_FIXED_ALIGN(Alignment) \
  static void* operator new(size_t size) {                         \
  palAllocator a;                     \
  return a.Allocate(size, Alignment); \
}                                                                \
  static void operator delete(void* p) {                           \
  palAllocator a;                     \
  return a.Deallocate(p, 0);              \
} \
  static void* operator new[](size_t size) { \
  palAllocator a; \
  return a.Allocate(size, Alignment); \
} \
  static void operator delete[](void* p) { \
  palAllocator a; \
  return a.Deallocate(p, 0); \
}


#endif  // LIBPAL_PAL_ALLOCATOR_H__
