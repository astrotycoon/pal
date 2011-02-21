
#include "libpal/libpal.h"

#include "pal_object_id_table_test.h"

bool PalObjectIdTableTest() {
  palObjectIdTable<const char> table;

  const char* a_str = "A";
  const char* b_str = "B";
  uint32_t a_id = table.AddObject(a_str);
  uint32_t b_id = table.AddObject(b_str);

  palAssertBreak(a_str == table.MapObject(a_id));
  palAssertBreak(b_str == table.MapObject(b_id));

  int count;

  count = 0;
  for (uint32_t id = table.GetFirstObjectID(); id != table.GetCapacity(); id = table.GetNextObjectID(id)) {
    const char* str = table.MapObject(id);
    palAssertBreak(str == a_str || str == b_str);
    count++;
  }
  palAssertBreak(count == 2);

  table.RemoveObject(a_id);
  table.RemoveObject(b_id);

  palAssertBreak(NULL == table.MapObject(a_id));
  palAssertBreak(NULL == table.MapObject(b_id));

  a_id = table.AddObject(a_str);
  b_id = table.AddObject(b_str);

  palAssertBreak(a_id == table.FindObjectID(a_str));
  palAssertBreak(b_id == table.FindObjectID(b_str));

  count = 0;
  for (uint32_t id = table.GetFirstObjectID(); id != table.GetCapacity(); id = table.GetNextObjectID(id)) {
    const char* str = table.MapObject(id);
    palAssertBreak(str == a_str || str == b_str);
    count++;
  }
  
  palAssertBreak(count == 2);

  return true;
}