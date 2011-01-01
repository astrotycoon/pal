
#include "libpal/libpal.h"

bool PalAlgorithmsTest ()
{
  uint32_t data[] = { 11, 12, 13, 14, 15, 16, 17, 18, 19 };
  uint32_t num_data = sizeof(data)/sizeof(data[0]);
  
  int resultLT, resultLTE, result, resultGTE, resultGT;

  //palBreakHere();
  
  resultLT = palBinarySearchLT(&data[0], num_data, (uint32_t)2);
  resultLTE = palBinarySearchLTE(&data[0], num_data, (uint32_t)2);
  result = palBinarySearch(&data[0], num_data, (uint32_t)2);
  resultGTE = palBinarySearchGTE(&data[0], num_data, (uint32_t)20);
  resultGT = palBinarySearchGT(&data[0], num_data, (uint32_t)20);
  
  //palBreakHere();
  
  // should be 1
  resultLT = palBinarySearchLT(&data[0], num_data, (uint32_t)13);
  // should be 3
  resultLTE = palBinarySearchLTE(&data[0], num_data, (uint32_t)14);
  // should be 4
  result = palBinarySearch(&data[0], num_data, (uint32_t)15);
  // should be 5
  resultGTE = palBinarySearchGTE(&data[0], num_data, (uint32_t)16);
  // should be 7
  resultGT = palBinarySearchGT(&data[0], num_data, (uint32_t)17);

  //palBreakHere();
  
  return true;
}
