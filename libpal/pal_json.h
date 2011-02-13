/*
	Copyright (c) 2011 John McCutchan <john@johnmccutchan.com>

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.

	3. This notice may not be removed or altered from any source
	distribution.
*/

#ifndef LIBPAL_PAL_JSON_H_
#define LIBPAL_PAL_JSON_H_

enum palJSONTokenType {
  kJSONTokenTypeParseError = 0,
  kJSONTokenTypeMap,
  kJSONTokenTypeMapEntry,
  kJSONTokenTypeArray,
  kJSONTokenTypeValueString,
  kJSONTokenTypeValueNumber,
  kJSONTokenTypeValueTrue,
  kJSONTokenTypeValueFalse,
  kJSONTokenTypeValueNull,
};

struct palJSONToken {
  /* Type of token */
  palJSONTokenType type;

  /* String containing token */
  const char* JSON_str;

  /* Start and end of token in string */
  int first_index;
  int length;

  /* Token name, maybe -1 */
  int name_first_index;
  int name_length;

  /* The token value */
  int value_first_index;
  int value_length;

  /* If this token represents an object(map) or an array,
     this count tells you how many children there are
   */
  int num_children;

  bool NameMatch(const char* name);

  float GetAsFloat();
  int GetAsInt();
  bool GetAsBool();
  void* GetAsPointer();
};

palJSONToken* palJSONFindTokenWithName(palJSONToken* token_buffer, int token_buffer_length, const char* name);

class palJSONParser {
  const char* JSON_str_;
  int JSON_str_len_;
  int parse_current_index_;
  int parse_end_index_;
public:
  palJSONParser();

  /* Initialize this JSON parser with a string to parse */
  void Init(const char* JSON_str);

  /* Start parsing the whole string */
  void StartParse();
  /* Start parsing at start_index and parse until end_index */
  void StartParse(int start_index, int end_index);

  /* Returns the number of tokens detected,
   * if the return is the size of the tokens buffer,
   * you should call parse again */

  /*
  An example,
  palJSONToken token_buffer[8];
  json.Init(string);
  json.StartParse();
  do {
    int num_parsed = json.Parse(&token_buffer[0], 8);

    // do something with parsed tokens

    if (num_parsed < 8)
      break;
  } while(true);
  */
  int Parse(palJSONToken* token_buffer, int token_buffer_size);
};

#endif  // LIBPAL_PAL_JSON_H_