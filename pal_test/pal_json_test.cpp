
#include "libpal/libpal.h"

#include "pal_json_test.h"

bool PalJsonTest() {
  palFile f;

  bool r = f.OpenForReading("test.json");

  if (!r) {
    // couldn't open file
    palBreakHere();
  }

  uint64_t buf_len;
  unsigned char* buf = f.CopyContentsAsString(&buf_len);
  f.Close();

  return true;
}