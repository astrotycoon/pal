#include "libpal/libpal.h"

#include "pal_pool_alloc_test.h"

// 4K static buffer
static unsigned char static_pool_buffer[4096];

bool PalPoolAllocTest()
{
  palPoolAllocator<32> small_pool;
  palPoolAllocator<> static_pool;

  small_pool.Create(64, 200, palMalloc(64*200));
  static_pool.Create(32, sizeof(static_pool_buffer)/32, &static_pool_buffer[0]);
  
  for (int i = 0; i < 100; i++)
  {
    void* p = small_pool.Malloc();
    if (p == NULL)
    {
      palBreakHere();
    }
  }
  
  small_pool.FreeAll();
  
  return true;
}