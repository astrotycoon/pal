#include "libpal/libpal.h"

#include "pal_pool_alloc_test.h"

// 4K static buffer
static unsigned char static_pool_buffer[4096];

bool PalPoolAllocTest()
{
  palPoolAllocator small_pool;
  palPoolAllocator static_pool;

  small_pool.Create(palMalloc(64*200), 64*200, 64, 1);
  static_pool.Create(&static_pool_buffer[0], sizeof(static_pool_buffer), 32, 32);
  
  for (int i = 0; i < 100; i++)
  {
    void* p = small_pool.Allocate(64);
    if (p == NULL)
    {
      palBreakHere();
    }
  }
  
  small_pool.FreeAll();
  
  return true;
}