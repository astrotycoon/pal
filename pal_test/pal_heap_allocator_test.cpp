#include "libpal/libpal.h"

#define MB(x) (x*1024*1024)
unsigned char g_buffer[MB(2)];

bool StaticHeapTest() {
  palHeapAllocator ha("heap allocator");
  int r;
  r = ha.Create(&g_buffer[0], MB(2));
  palAssertBreak(r == 0);
  ha.Destroy();
  return true;
}

bool PageHeapTest() {  
  palHeapAllocator ha("heap allocator");
  int r;
  r = ha.Create((palPageAllocator*)g_PageAllocator);
  palAssertBreak(r == 0);
  ha.Destroy();
  return true;
}

bool palHeapAllocatorTest() {
  StaticHeapTest();
  PageHeapTest();
  return true;
}