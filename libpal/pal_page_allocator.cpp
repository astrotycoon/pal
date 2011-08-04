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
  ReportMemoryAllocation(size+_page_size);
  return p+_page_size;
}

void palPageAllocator::Deallocate(void* ptr) {
  unsigned char* p = reinterpret_cast<unsigned char*>(ptr);
  p -= _page_size;
  uint32_t magic = *((uint32_t*)p);
  p += 4;
  uint32_t size = *((uint32_t*)p);
  palAssert(magic == PAGE_ALLOCATOR_MAGIC);
  ReportMemoryDeallocation(size+_page_size);
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

void* palPageAllocator::mmap(size_t size) {

}

int palPageAllocator::munmap(void* ptr, size_t size) {

}
