
#include "libpal/libpal.h"

#include "pal_json_test.h"

bool PalJsonTest() {
  palFile f;

  int r = f.OpenForReading("test.json");

  if (r != 0) {
    // couldn't open file
    palBreakHere();
  }

  palMemBlob blob;
  f.CopyContentsAsString(&blob);
  f.Close();

#if 0
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
#endif

  palJSONPrettyPrint(blob.GetPtr<const char>(0));

  palFile::FreeFileContents(&blob);
  return true;
}