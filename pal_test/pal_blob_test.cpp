#include "libpal/pal_string.h"
#include "libpal/pal_mem_blob.h"

#include "pal_test/pal_blob_test.h"

#define BUFF_SIZE 1024
uint8_t buff[BUFF_SIZE];

bool TestRingBufferBasic() {
  int r;
  palRingBlob rb;
  rb.Reset(&buff[0], BUFF_SIZE);

  uint64_t num8 = 0xdeadcafebabef00d;
  float pi = 3.14159f;
  const char* str = "test";
  r = rb.Append(&pi);
  palAssertBreak(r == 0);
  rb.Append(&num8);
  palAssertBreak(r == 0);
  rb.Append(str, palStringLength(str));
  palAssertBreak(r == 0);

  uint32_t zz[32];

  r = rb.Peek((void*)&zz[0], sizeof(float));
  palAssertBreak(r == 0);
  float* pi_test = (float*)(&zz[0]);
  palAssertBreak(*pi_test == pi);

  r = rb.Consume((void*)&zz[0], sizeof(float));
  palAssertBreak(r == 0);
  pi_test = (float*)(&zz[0]);
  palAssertBreak(*pi_test == pi);

  r = rb.Consume((void*)&zz[0], sizeof(uint64_t));
  palAssertBreak(r == 0);
  uint64_t* num8_test = (uint64_t*)(&zz[0]);
  palAssertBreak(*num8_test == num8);
  
  r = rb.Consume((void*)&zz[0], (uint64_t)palStringLength(str));
  palAssertBreak(r == 0);
  char* str_test = (char*)&zz[0];
  palAssertBreak(str_test[0] == str[0] && str_test[1] == str[1] && str_test[2] == str[2] && str_test[3] == str[3]);

  return true;
}

bool TestRingBufferFillEmpty() {
  int r;
  uint8_t small_buffer[16];
  palRingBlob rb;
  rb.Reset(&small_buffer[0], 16);
  uint64_t num_a = 0xf00dd00fcafebabe;
  uint64_t num_b = 0xdeadbeeffeedbeeb;
  uint8_t la = 'z';

  r = rb.Append(&num_a);
  palAssertBreak(r == 0);
  r = rb.Append(&num_b);
  palAssertBreak(r == 0);
  r = rb.Append(&la);
  palAssertBreak(r == PAL_RING_BLOB_NO_ROOM);

  uint64_t c;
  r = rb.Consume(&c);
  palAssertBreak(r == 0);
  palAssertBreak(c == num_a);
  r = rb.Consume(&c);
  palAssertBreak(r == 0);
  palAssertBreak(c == num_b);
  r = rb.Consume(&c);
  palAssertBreak(r == PAL_RING_BLOB_NO_DATA);

  return true;
}

bool TestRingBufferWrap() {
  int r;
  uint8_t small_buffer[16];
  palRingBlob rb;
  rb.Reset(&small_buffer[0], 16);
  uint64_t num_a = 0xf00dd00fcafebabe;
  uint64_t num_b = 0xdeadbeeffeedbeeb;
  uint8_t la = 'z';

  uint64_t check_num_a;
  uint8_t check_la;

  r = rb.Append(&la);
  palAssertBreak(r == 0);
  r = rb.Append(&num_a);
  palAssertBreak(r == 0);
  r = rb.Append(&num_b);
  palAssertBreak(r == PAL_RING_BLOB_NO_ROOM);

  r = rb.Consume(&check_la);
  palAssertBreak(r == 0);
  palAssertBreak(check_la == la);

  r = rb.Append(&num_b);
  palAssertBreak(r == 0);
  
  r = rb.Consume(&check_num_a);
  palAssertBreak(r == 0);
  palAssertBreak(check_num_a == num_a);
  r = rb.Consume(&check_num_a);
  palAssertBreak(r == 0);
  palAssertBreak(check_num_a == num_b);

  return true;
}

bool PalBlobTest() {
  TestRingBufferBasic();
  TestRingBufferFillEmpty();
  TestRingBufferWrap();
  return true;
}
