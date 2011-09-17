
#include "libpal/libpal.h"

bool PalAlgorithmsTest() {
  uint32_t data[] = { 11, 12, 13, 14, 15, 16, 17, 18, 19 };
  uint32_t num_data = sizeof(data)/sizeof(data[0]);
 
  int r;

  {
    // Lower bound
    const uint32_t* result;
    r = palBinarySearchLowerBound(&data[0], num_data, (uint32_t)2, &result);
    palAssertBreak(r != 0 && result == NULL);
    r = palBinarySearchLowerBound(&data[0], num_data, (uint32_t)10, &result);
    palAssertBreak(r != 0 && result == NULL);
    r = palBinarySearchLowerBound(&data[0], num_data, (uint32_t)11, &result);
    palAssertBreak(r != 0 && result == NULL);
    r = palBinarySearchLowerBound(&data[0], num_data, (uint32_t)12, &result);
    palAssertBreak(r == 0 && result == &data[0]);
    r = palBinarySearchLowerBound(&data[0], num_data, (uint32_t)15, &result);
    palAssertBreak(r == 0 && result == &data[3]);
    r = palBinarySearchLowerBound(&data[0], num_data, (uint32_t)19, &result);
    palAssertBreak(r == 0 && result == &data[num_data-2]);
    r = palBinarySearchLowerBound(&data[0], num_data, (uint32_t)20, &result);
    palAssertBreak(r == 0 && result == &data[num_data-1]);
  }

  {
    // Upper bound
    const uint32_t* result;
    r = palBinarySearchUpperBound(&data[0], num_data, (uint32_t)2, &result);
    palAssertBreak(r == 0 && result == &data[0]);
    r = palBinarySearchUpperBound(&data[0], num_data, (uint32_t)10, &result);
    palAssertBreak(r == 0 && result == &data[0]);
    r = palBinarySearchUpperBound(&data[0], num_data, (uint32_t)11, &result);
    palAssertBreak(r == 0 && result == &data[1]);
    r = palBinarySearchUpperBound(&data[0], num_data, (uint32_t)12, &result);
    palAssertBreak(r == 0 && result == &data[2]);
    r = palBinarySearchUpperBound(&data[0], num_data, (uint32_t)16, &result);
    palAssertBreak(r == 0 && result == &data[num_data-3]);
    r = palBinarySearchUpperBound(&data[0], num_data, (uint32_t)18, &result);
    palAssertBreak(r == 0 && result == &data[num_data-1]);
    r = palBinarySearchUpperBound(&data[0], num_data, (uint32_t)19, &result);
    palAssertBreak(r != 0 && result == NULL);
    r = palBinarySearchUpperBound(&data[0], num_data, (uint32_t)999, &result);
    palAssertBreak(r != 0 && result == NULL);
  }

  {
    // Equality
  }


  palBreakHere();
  return true;
}
