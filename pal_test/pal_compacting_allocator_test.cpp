
#include "libpal/libpal.h"

static uint32_t memory_chunk_size = 1000 * 1000;
static unsigned char memory_chunk[1000 * 1000];

bool PalCompactingAllocatorTest() {
  palCompactingAllocator gca(memory_chunk_size, &memory_chunk[0]);

  gca.SetAllocator(g_DefaultHeapAllocator);

  //gca.DiagnosticDump();

  palGCAHandle handle1 = gca.Malloc(1000);

  //gca.DiagnosticDump();

  palGCAHandle handle2 = gca.Malloc(1000);
  //gca.DiagnosticDump();

  palGCAHandle handle3 = gca.Malloc(1000);
  //gca.DiagnosticDump();

  gca.Free(handle1);
  gca.DiagnosticDump();
  gca.Compact(4);
  gca.DiagnosticDump();
  gca.Free(handle2);
  gca.DiagnosticDump();
  gca.Compact(1);
  gca.DiagnosticDump();
  return true;
}
