
#include "libpal/libpal.h"

#include "pal_json_test.h"

bool PalJsonTest() {
  palFile f;

  bool r = f.OpenForReading("test2.json");

  if (!r) {
    // couldn't open file
    palBreakHere();
  }

  uint64_t buf_len;
  unsigned char* buf = f.CopyContentsAsString(&buf_len);
  f.Close();

  palJSONParser parser;

  parser.Init((const char*)buf);

  // start parsing the whole buffer
  parser.StartParse();

  const int num_json_tokens = 20;
  int num_json_tokens_parsed = 0;
  palJSONToken tokens[num_json_tokens];

  num_json_tokens_parsed = parser.Parse(&tokens[0], num_json_tokens);

  for (int i = 0; i < num_json_tokens_parsed; i++) {
    tokens[i].DebugPrintf();
  }

  

#if 0
  printf("Parsing funk map\n");

  if (tokens[3].GetTypeOfValue() == kJSONTokenTypeMap) {
    // the third entry is a MAP entry
    parser.StartParse(tokens[3].value_first_index, tokens[3].value_length);
    num_json_tokens_parsed = parser.Parse(&tokens[0], num_json_tokens);
    for (int i = 0; i < num_json_tokens_parsed; i++) {
      tokens[i].DebugPrintf();
    }
  }
#endif

  printf("Parsing arrayboy array\n");
  if (tokens[4].GetTypeOfValue() == kJSONTokenTypeArray) {
    // the fourth entry is an array
    parser.StartParse(tokens[4].value_first_index, tokens[4].value_length);
    num_json_tokens_parsed = parser.Parse(&tokens[0], num_json_tokens);
    for (int i = 0; i < num_json_tokens_parsed; i++) {
      tokens[i].DebugPrintf();
    }
  }
  return true;
}