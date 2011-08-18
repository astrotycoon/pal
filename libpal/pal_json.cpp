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

#include "libpal/pal_array.h"
#include "libpal/pal_string.h"
#include "libpal/pal_json.h"

const char* json_token_type_strings[NUM_palJSONTokenType] = {
  "parse error",
  "map",
  "map entry",
  "array",
  "value string",
  "value number",
  "value true",
  "value false",
  "value null"
};

palTokenizerKeyword JSON_keywords[] = {
  {"true", kJSONTokenTypeValueTrue},
  {"false", kJSONTokenTypeValueFalse},
  {"null", kJSONTokenTypeValueNull},
  NULL, 0
};

palJSONTokenType palJSONToken::GetTypeOfValue() const {
  char ch = *(JSON_str+value_first_index);

  if (ch == 't') {
    return kJSONTokenTypeValueTrue;
  } else if (ch == 'f') {
    return kJSONTokenTypeValueFalse;
  } else if (ch == 'n') {
    return kJSONTokenTypeValueNull;
  } else if (ch == '[') {
    return kJSONTokenTypeArray;
  } else if (ch == '{') {
    return kJSONTokenTypeMap;
  } else if (ch == '\"') {
    return kJSONTokenTypeValueString;
  } else if (palIsDigit(ch) || ch == '-') {
    return kJSONTokenTypeValueNumber;
  }

  return kJSONTokenTypeParseError;
}

bool palJSONToken::NameMatch(const char* name) const {
  return false;
}

float palJSONToken::GetAsFloat() const {
  return (float)atof(JSON_str+value_first_index);
}

int palJSONToken::GetAsInt() const {
  return atoi(JSON_str+value_first_index);
}

bool palJSONToken::GetAsBool() const {
  return (type == kJSONTokenTypeValueTrue);
}

void* palJSONToken::GetAsPointer() const {
  return NULL;
}

void palJSONToken::GetAsString(palDynamicString* str) const {
  str->Set("");
  str->Append(JSON_str+value_first_index+1, value_length-2);
}

void palJSONToken::GetName(palDynamicString* name) const {
  name->Set("");
  name->Append(JSON_str+name_first_index+1, name_length-2);
}

void palJSONToken::DebugPrintf() const {
  palDynamicString debug_str;

  switch (type) {
  case kJSONTokenTypeValueFalse:
    palPrintf("false");
    break;
  case kJSONTokenTypeValueTrue:
    palPrintf("true");
    break;
  case kJSONTokenTypeValueNull:
    palPrintf("null");
    break;
  case kJSONTokenTypeArray:
    break;
  case kJSONTokenTypeMap:
    break;
  case kJSONTokenTypeMapEntry:
    debug_str.Append(JSON_str+name_first_index, name_length);
    if (GetTypeOfValue() == kJSONTokenTypeValueFalse) {
      palPrintf("%s : false", debug_str.C());
    } else if (GetTypeOfValue() == kJSONTokenTypeValueTrue) {
      palPrintf("%s : true", debug_str.C());
    } else if (GetTypeOfValue() == kJSONTokenTypeValueNull) {
      palPrintf("%s : null", debug_str.C());
    } else if (GetTypeOfValue() == kJSONTokenTypeValueNumber) {
      palPrintf("%s : %f", debug_str.C(), GetAsFloat());
    } else if (GetTypeOfValue() == kJSONTokenTypeValueString) {
      palPrintf("%s : ", debug_str.C());
      debug_str.Reset();
      debug_str.Append(JSON_str+value_first_index, value_length);
      palPrintf("%s", debug_str.C());
    } else {
      palPrintf("%s : \n", debug_str.C());
    }
    break;
  case kJSONTokenTypeValueNumber:
    palPrintf("%f", GetAsFloat());
    break;
  case kJSONTokenTypeValueString:
    debug_str.Append(JSON_str+value_first_index, value_length);
    palPrintf("%s", debug_str.C());
    break;
  case kJSONTokenTypeParseError:
    palPrintf("parsing error\n");
    break;
  default:
    palBreakHere();
    break;
  }
}

palJSONToken* palJSONFindTokenWithName(palJSONToken* token_buffer, int token_buffer_length, const char* name) {
  for (int i = 0; i < token_buffer_length; i++) {
    if (token_buffer[i].name_first_index == -1) {
      break;
    }
    const char* name_string = token_buffer[i].JSON_str+(token_buffer[i].name_first_index+1);
    if (palStringEqualsN(name, name_string, (token_buffer[i].name_length-2))) {
      return &token_buffer[i];
    }
  }

  return NULL;
}

palJSONParser::palJSONParser() {
  JSON_str_ = 0;
  JSON_str_len_ = 0;
  parse_current_index_ = 0;
  parse_end_index_ = 0;
  tokenizer_.SetKeywordArray(&JSON_keywords[0]);
}

void palJSONParser::Init(const char* JSON_str) {
  JSON_str_ = JSON_str;
  JSON_str_len_ = palStringLength(JSON_str_);
  parse_current_index_ = 0;
  parse_end_index_ = 0;
}

void palJSONParser::StartParse() {
  buffer_offset_ = 0;
  parse_current_index_ = 0;
  parse_end_index_ = JSON_str_len_+1;
  _memory_stream.Reset();
  _memory_stream.Create(palMemBlob((void*)JSON_str_, (uint64_t)JSON_str_len_), false);
  tokenizer_.UseStream(&_memory_stream);
}
  
void palJSONParser::StartParse(int start_index, int length) {
  buffer_offset_ = start_index;
  parse_current_index_ = start_index;
  parse_end_index_ = start_index+length;
  _memory_stream.Reset();
  _memory_stream.Create(palMemBlob((void*)(JSON_str_+start_index), (uint64_t)length), false);
  tokenizer_.UseStream(&_memory_stream);
}

palJSONTokenType palJSONParser::SkipValue(int* start_index, int* length) {
  if (!start_index || !length) {
    return kJSONTokenTypeParseError;
  }

  palToken token;

  tokenizer_.FetchNextToken(&token);

  *start_index = buffer_offset_ + token.start_index;

  int nest_count = 0;
  if (token.type == kTokenPunctuation && token.type_flags == kPunctuationBRACE_OPEN) {
    // handle map value
    nest_count = 1;
    while (nest_count > 0) {
      tokenizer_.FetchNextToken(&token);
      if (token.type == kTokenPunctuation && token.type_flags == kPunctuationBRACE_OPEN) {
        nest_count++;
      } else if (token.type == kTokenPunctuation && token.type_flags == kPunctuationBRACE_CLOSE) {
        nest_count--;
      } else if (token.type == kTokenEOS) {
        return kJSONTokenTypeParseError;
      }
    }
    *length = token.start_index + token.length - *start_index + buffer_offset_;
    return kJSONTokenTypeMap;
  } else if (token.type == kTokenPunctuation && token.type_flags == kPunctuationSQUARE_BRACKET_OPEN) {
    // handle array value
    nest_count = 1;
    while (nest_count > 0) {
      tokenizer_.FetchNextToken(&token);
      if (token.type == kTokenPunctuation && token.type_flags == kPunctuationSQUARE_BRACKET_OPEN) {
        nest_count++;
      } else if (token.type == kTokenPunctuation && token.type_flags == kPunctuationSQUARE_BRACKET_CLOSE) {
        nest_count--;
      } else if (token.type == kTokenEOS) {
        return kJSONTokenTypeParseError;
      }
    }

    *length = token.start_index + token.length - *start_index + buffer_offset_;
    return kJSONTokenTypeArray;
  } else {
    *length = token.length;
    if (token.type == kTokenNumber) {
      return kJSONTokenTypeValueNumber;
    } else if (token.type == kTokenString) {
      return kJSONTokenTypeValueString;
    } else if (token.type == kTokenKeyword) {
      return (palJSONTokenType)token.type_flags;
    }

    return kJSONTokenTypeParseError;
  }
}

int palJSONParser::ParseMapEntries(palJSONToken* token_buffer, int token_buffer_size) {
  int token_buffer_insert_index = 0;
  int brace_count = 1;

  palToken token;
  while (brace_count > 0) {  
    tokenizer_.FetchNextToken(&token);

    /* map entries look like:
       "string" : value...,
       "string2" : value...,
     */

    if (token.type == kTokenEOL) {
      continue;
    }
    if (token.type == kTokenPunctuation && token.type_flags == kPunctuationBRACE_CLOSE) {
      brace_count--;
      break;
    }

    if (token.type == kTokenPunctuation || token.type_flags == kPunctuationCOMMA) {
      // next token.
      continue;
    }

    if (token.type != kTokenString) {
      // the first token must be a string
      break;
    }

    // we have a map entry
    token_buffer[token_buffer_insert_index].type = kJSONTokenTypeMapEntry;
    token_buffer[token_buffer_insert_index].JSON_str = NULL;

    token_buffer[token_buffer_insert_index].first_index = buffer_offset_ + token.start_index;

    token_buffer[token_buffer_insert_index].name_first_index = buffer_offset_ + token.start_index;
    token_buffer[token_buffer_insert_index].name_length = token.length;

    tokenizer_.FetchNextToken(&token);

    if (token.type != kTokenPunctuation || token.type_flags != kPunctuationCOLON) {
      // after the name, we require a colon.
      break;
    }

    // skip over the value for this map entry, recording the first index and length of the value
    SkipValue(&token_buffer[token_buffer_insert_index].value_first_index, &token_buffer[token_buffer_insert_index].value_length);

    token_buffer_insert_index++;
    if (token_buffer_insert_index == token_buffer_size) {
      // we have filled up the token buffer, return.
      return token_buffer_insert_index;
    }
  }

  if (brace_count) {
    // add parse error token
    palBreakHere();
  }

  return token_buffer_insert_index;
}

int palJSONParser::ParseArrayEntries(palJSONToken* token_buffer, int token_buffer_size) {
  int token_buffer_insert_index = 0;
  int brace_count = 1;

  palToken token;
  while (brace_count > 0) {  
    tokenizer_.FetchNextToken(&token);

    /* array entries look like
      value1, value2, value3
     */

    if (token.type == kTokenEOL) {
      continue;
    }
    if (token.type == kTokenPunctuation && token.type_flags == kPunctuationSQUARE_BRACKET_CLOSE) {
      brace_count--;
      break;
    }

    if (token.type == kTokenPunctuation && token.type_flags == kPunctuationCOMMA) {
      // next value
      continue;
    }

    // we are ready to parse the value
    // push the last read token onto this buffer
    tokenizer_.PushToken(&token);

    token_buffer[token_buffer_insert_index].type = kJSONTokenTypeParseError;
    token_buffer[token_buffer_insert_index].JSON_str = NULL;

    token_buffer[token_buffer_insert_index].first_index = buffer_offset_ + token.start_index;

    token_buffer[token_buffer_insert_index].name_first_index = -1;
    token_buffer[token_buffer_insert_index].name_length = -1;

    // skip over the value
    token_buffer[token_buffer_insert_index].type = SkipValue(&token_buffer[token_buffer_insert_index].value_first_index, &token_buffer[token_buffer_insert_index].value_length);

    token_buffer_insert_index++;

    if (token_buffer_insert_index == token_buffer_size) {
      // we have filled up the token buffer, return.
      return token_buffer_insert_index;
    }
  }

  if (brace_count) {
    // add parse error token
    palBreakHere();
  }

  return token_buffer_insert_index;
}

int palJSONParser::Parse(palJSONToken* token_buffer, int token_buffer_size) {
  int token_buffer_insert_index = 0;

  const int kStateStart = 1;

  const int kStateObject = 2;
  const int kStateArray = 3;
  const int kStateValueTrue = 4;
  const int kStateValueFalse = 5;
  const int kStateValueNull = 6;
  const int kStateValueNumber = 7;
  const int kStateValueString = 8;

  char last_ch = 0;
  int state = kStateStart;
  int lbrace_count = 0;
  int lbracket_count = 0;

  palToken token;

  tokenizer_.FetchNextToken(&token);

  if (token.type == kTokenEOS || token.type == kTokenERROR) {
    // handle error
    return 0;
  }

  if (token.type == kTokenKeyword) {
    // we parsed a keyword
    token_buffer[token_buffer_insert_index].JSON_str = JSON_str_;
    token_buffer[token_buffer_insert_index].first_index = buffer_offset_ + token.start_index;
    token_buffer[token_buffer_insert_index].length = token.length;
    token_buffer[token_buffer_insert_index].value_first_index = buffer_offset_ + token.start_index;
    token_buffer[token_buffer_insert_index].value_length = token.length;
    token_buffer[token_buffer_insert_index].type = (palJSONTokenType)token.type_flags;
    token_buffer[token_buffer_insert_index].name_first_index = -1;
    token_buffer[token_buffer_insert_index].name_length = -1;
    token_buffer_insert_index++;
    return token_buffer_insert_index;
  }

  if (token.type == kTokenString) {
    // we parsed a string
    token_buffer[token_buffer_insert_index].JSON_str = JSON_str_;
    token_buffer[token_buffer_insert_index].first_index = buffer_offset_ + token.start_index;
    token_buffer[token_buffer_insert_index].length = token.length;
    token_buffer[token_buffer_insert_index].value_first_index = buffer_offset_ + token.start_index;
    token_buffer[token_buffer_insert_index].value_length = token.length;
    token_buffer[token_buffer_insert_index].type = kJSONTokenTypeValueString;
    token_buffer[token_buffer_insert_index].name_first_index = -1;
    token_buffer[token_buffer_insert_index].name_length = -1;
    token_buffer_insert_index++;
    return token_buffer_insert_index;
  }

  if (token.type == kTokenNumber) {
    // we parsed a number
    token_buffer[token_buffer_insert_index].JSON_str = JSON_str_;
    token_buffer[token_buffer_insert_index].first_index = buffer_offset_ + token.start_index;
    token_buffer[token_buffer_insert_index].length = token.length;
    token_buffer[token_buffer_insert_index].value_first_index = buffer_offset_ + token.start_index;
    token_buffer[token_buffer_insert_index].value_length = token.length;
    token_buffer[token_buffer_insert_index].type = kJSONTokenTypeValueNumber;
    token_buffer[token_buffer_insert_index].name_first_index = -1;
    token_buffer[token_buffer_insert_index].name_length = -1;
    token_buffer_insert_index++;
    return token_buffer_insert_index;    
  }

  /* There are only two JSON types left, the map and the array */
  if (token.type == kTokenPunctuation && token.type_flags == kPunctuationBRACE_OPEN) {
    /* First token is a "{", each token we return will be a MAP_ENTRY token. */
    int parsed_tokens = ParseMapEntries(token_buffer, token_buffer_size);
    for (int i = 0; i < parsed_tokens; i++) {
      token_buffer[i].JSON_str = JSON_str_;
    }
    return parsed_tokens;
  } else if (token.type == kTokenPunctuation && token.type_flags == kPunctuationSQUARE_BRACKET_OPEN) {
    /* First token is a "[", each token we return will be an array entry */
    int parsed_tokens = ParseArrayEntries(token_buffer, token_buffer_size);
    for (int i = 0; i < parsed_tokens; i++) {
      token_buffer[i].JSON_str = JSON_str_;
    }
    return parsed_tokens;
  } else {
    palBreakHere();
    return -1;
  }
}

void indent(int depth) {
  for (int i = 0; i < depth; i++) {
    palPrintf("  ");
  }
}

void palJSONPrettyPrintInternal(const char* str, int depth, int start_index, int length) {
  palJSONParser parser;
  parser.Init(str);
  parser.StartParse(start_index, length);

  const int num_json_tokens = 50;
  palJSONToken tokens[num_json_tokens];
  int num_json_tokens_parsed = 0;

  num_json_tokens_parsed = parser.Parse(&tokens[0], num_json_tokens);

  if (num_json_tokens_parsed > 0 && tokens[0].type == kJSONTokenTypeMapEntry) {
    indent(depth);
    palPrintf("{\n");
    depth++;
  }
  for (int i = 0; i < num_json_tokens_parsed; i++) {
    bool need_comma = i < num_json_tokens_parsed-1;
    indent(depth);
    tokens[i].DebugPrintf();
    if (depth >= 0) {
      if (tokens[i].GetTypeOfValue() == kJSONTokenTypeMap) {
        palJSONPrettyPrintInternal(str, depth+1, tokens[i].value_first_index, tokens[i].value_length);
      } else if (tokens[i].GetTypeOfValue() == kJSONTokenTypeArray) {
        indent(depth);
        palPrintf("[\n");
        palJSONPrettyPrintInternal(str, depth+1, tokens[i].value_first_index, tokens[i].value_length);
        indent(depth);
        palPrintf("]");
      } 
    }
    
    if (need_comma) {
      palPrintf(",\n");
    } else {
      palPrintf("\n");
    }
  }

  if (num_json_tokens_parsed > 0 && tokens[0].type == kJSONTokenTypeMapEntry) {
    depth--;
    indent(depth);
    palPrintf("}\n");
  }
}

void palJSONPrettyPrint(const char* JSON_str) {
  int len = palStringLength(JSON_str);
  palJSONPrettyPrintInternal(JSON_str, 0, 0, len);
}

palJSONReader::palJSONReader() {
}

void palJSONReader::Init(const char* JSON_str) {
  _parser.Init(JSON_str);
}

struct JsonQueryNode {
  palDynamicString name;
  // or
  int array_index;
  JsonQueryNode() {
    array_index = -1;
  }
};

static void BuildQueryNodes(const char* expr, palArray<JsonQueryNode>* nodes) {
  if (expr == NULL || *expr == '\0') {
    return;
  }

  if (*expr == '[') {
    JsonQueryNode& node = nodes->AddTail();
    node.array_index = palStringToInteger(expr+1);
    int first_rsquare = palStringFindCh(expr, ']');
    BuildQueryNodes(&expr[first_rsquare+1], nodes);
    return;
  } else if (*expr == '.') {
    //QueryNode& node = nodes->AddTail();
    //node.array_index = 0;
    BuildQueryNodes(expr+1, nodes);
    return;
  }

  int first_dot = palStringFindCh(expr, '.');
  int first_lsquare = palStringFindCh(expr, '[');

  if (first_dot >= 0 && first_lsquare >= 0) {
    if (first_dot < first_lsquare) {
      first_lsquare = -1;
    } else {
      first_dot = -1;
    }
  }

  if (first_dot > 0) {
    JsonQueryNode& node = nodes->AddTail();
    node.name.Append(expr, first_dot);
    BuildQueryNodes(&expr[first_dot], nodes);
    return;
  }

  if (first_lsquare > 0) {
    JsonQueryNode& node = nodes->AddTail();
    node.name.Append(expr, first_lsquare);
    BuildQueryNodes(&expr[first_lsquare], nodes);
    return;
  }

  JsonQueryNode& node = nodes->AddTail();
  node.name.Append(expr, palStringLength(expr));

  return;
}

int palJSONReader::GetPointerToValue(const char* expr, palJSONReaderPointer* pointer) {
  pointer->elements = NULL;
  pointer->size = 0;

  palArray<JsonQueryNode> nodes;

  BuildQueryNodes(expr, &nodes);

#if 0
  for (int i = 0; i < nodes.GetSize(); i++) {
    const QueryNode& node = nodes[i];
    int j = 0;
    while (j < i) {
      palPrintf(" ");
      j++;
    }
    if (node.array_index >= 0) {
      palPrintf("[%d]", node.array_index);
    } else {
      palPrintf("%s", node.name.C());
    }
    palPrintf("\n");
  }
#endif

  const int max_tokens = 32;
  palJSONToken tokens[max_tokens];
  palJSONToken* found_token = NULL;

  _parser.StartParse();
  for (int i = 0; i < nodes.GetSize(); i++) {
    const JsonQueryNode& node = nodes[i];
    int num_tokens = _parser.Parse(&tokens[0], max_tokens);
    if (node.array_index >= 0) {
      // array query
      if (node.array_index >= num_tokens) {
        found_token = NULL;
        break;
      }
      found_token = &tokens[node.array_index];
    } else {
      // name query
      found_token = palJSONFindTokenWithName(&tokens[0], num_tokens, node.name.C());
      if (found_token == NULL) {
        break;
      }
    }
    _parser.StartParse(found_token->value_first_index, found_token->value_length);
  }

  if (found_token) {
    // fill in pointer data
    pointer->elements = found_token->JSON_str+found_token->value_first_index;
    pointer->size = found_token->value_length;
    return 0;
  }

  return -1;
}



#if 0
{
  const char* j_i_name = "c:/temp/blah.json";
  uint64_t len;
  unsigned char* contents = palCopyFileContentsAsString(j_i_name, &len);
  if (!contents) {
    return -1;
  }
  palJSONReader j_reader;
  j_reader.Init((const char*)contents);

  char input[512];
  palStringPrintf(&input[0], 512, "menu.items[1].label");
  while (1) {
    palJSONReaderPointer pointer;
    int r = j_reader.GetPointerToValue(input, &pointer);
    if (r) {
      palPrintf("Error: could not find %s\n", input);
    } else {
      palPrintf("%.*s\n", pointer.size, pointer.elements);
    }

    continue;
  }
  palFree(contents);
  return 0;
}
#endif