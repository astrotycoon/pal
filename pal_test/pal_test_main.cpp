#include "libpal/libpal.h"
#include "pal_string_test.h"
#include "pal_thread_test.h"
#include "pal_file_test.h"
#include "pal_pool_alloc_test.h"
#include "pal_atomic_test.h"
#include "pal_container_test.h"
#include "pal_compacting_allocator_test.h"
#include "pal_algorithms_test.h"
#include "pal_simd_test.h"
#include "pal_event_test.h"
#include "pal_time_line_test.h"

int main (int argc, char** argv)
{
  PalPoolAllocTest();
  PalTimeLineTest();
  PalEventTest();
  PalAlgorithmsTest();
  PalSimdTest();
  PalCompactingAllocatorTest();
  PalFileTest();
  PalStringTest();
  PalStringTest();
  PalThreadTest();
  PalAtomicTest();
  PalContainerTest();
  return 0;
}
