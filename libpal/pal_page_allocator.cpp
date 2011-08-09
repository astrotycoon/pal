#include "libpal/pal_page_allocator.h"

#define PAGE_ALLOCATOR_MAGIC 0xdeadbeef

palPageAllocator::palPageAllocator() : palAllocatorInterface("Sytem Memory Page Allocator") {
#if defined(PAL_PLATFORM_WINDOWS)
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  _page_size = si.dwPageSize;
#else
#error no page allocator for your os
#endif
}

void* palPageAllocator::Allocate(uint32_t size, int alignment) {
  palAssert(alignment == _page_size);
  palAssert((size & (alignment-1)) == 0);
  unsigned char* p = NULL;
  VirtualAlloc(&p, size+_page_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  palAssert(p != NULL);
  {
    *((uint32_t*)p) = PAGE_ALLOCATOR_MAGIC;
    *((uint32_t*)(p + sizeof(uint32_t))) = size;
  }
  ReportMemoryAllocation(p+_page_size, size+_page_size);
  return p+_page_size;
}

void palPageAllocator::Deallocate(void* ptr) {
  unsigned char* p = reinterpret_cast<unsigned char*>(ptr);
  p -= _page_size;
  uint32_t magic = *((uint32_t*)p);
  p += 4;
  uint32_t size = *((uint32_t*)p);
  palAssert(magic == PAGE_ALLOCATOR_MAGIC);
  p -= 4;
  VirtualFree(p, 0, MEM_RELEASE);
  ReportMemoryDeallocation(ptr, size+_page_size);
}

uint32_t palPageAllocator::GetSize(void* ptr) const {
  unsigned char* p = reinterpret_cast<unsigned char*>(ptr);
  p -= _page_size;
  uint32_t magic = *((uint32_t*)p);
  p += 4;
  uint32_t size = *((uint32_t*)p);
  palAssert(magic == PAGE_ALLOCATOR_MAGIC);
  return size;
}

uint32_t palPageAllocator::GetPageSize() const {
  return _page_size;
}

#define MAX_SIZE_T (~(size_t)0)
#define MFAIL ((void*)(MAX_SIZE_T))
void* palPageAllocator::mmap(size_t size) {
  void* ptr = VirtualAlloc(0, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
  if (ptr) {
    ReportMemoryAllocation(ptr, size);
    return ptr;
  } else {
    return MFAIL;
  }
}

int palPageAllocator::munmap(void* ptr, size_t size) {
  MEMORY_BASIC_INFORMATION minfo;
  char* cptr = (char*)ptr;
  while (size) {
    if (VirtualQuery(cptr, &minfo, sizeof(minfo)) == 0)
      return -1;
    if (minfo.BaseAddress != cptr || minfo.AllocationBase != cptr ||
      minfo.State != MEM_COMMIT || minfo.RegionSize > size)
      return -1;
    if (VirtualFree(cptr, 0, MEM_RELEASE) == 0)
      return -1;
    ReportMemoryDeallocation(cptr, minfo.RegionSize);
    cptr += minfo.RegionSize;
    size -= minfo.RegionSize;
  }
  return 0;
}
