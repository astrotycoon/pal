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
#include "pal_json_test.h"
#include "pal_object_id_table_test.h"
#include "pal_web_socket_server_test.h"

int main(int argc, char** argv) {
  palStartup(windows_debugger_print_function);
  
  PalStringTest();
  PalThreadTest();
  return 0;

  palWebSocketServerTest();
  PalObjectIdTableTest();
  PalEventTest();
  PalJsonTest();
  PalPoolAllocTest();
  PalTimeLineTest();
  PalAlgorithmsTest();
  PalSimdTest();
  PalCompactingAllocatorTest();
  PalFileTest();
  PalStringTest();
  PalStringTest();
  PalThreadTest();
  PalAtomicTest();
  PalContainerTest();

  palSocketFini();

  return 0;
}
