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

void palJSONToken::DebugPrintf() const {
  palString<> debug_str;

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
    debug_str.AppendLength(JSON_str+name_first_index, name_length);
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
      debug_str.Clear();
      debug_str.AppendLength(JSON_str+value_first_index, value_length);
      palPrintf("%s", debug_str.C());
    } else {
      palPrintf("%s : \n", debug_str.C());
    }
    break;
  case kJSONTokenTypeValueNumber:
    palPrintf("%f", GetAsFloat());
    break;
  case kJSONTokenTypeValueString:
    debug_str.AppendLength(JSON_str+value_first_index, value_length);
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
  JSON_str_len_ = palStrlen(JSON_str_);
  parse_current_index_ = 0;
  parse_end_index_ = 0;
}

void palJSONParser::StartParse() {
  buffer_offset_ = 0;
  parse_current_index_ = 0;
  parse_end_index_ = JSON_str_len_+1;
  tokenizer_.UseReadOnlyBuffer(JSON_str_, JSON_str_len_);
}
  
void palJSONParser::StartParse(int start_index, int length) {
  buffer_offset_ = start_index;
  parse_current_index_ = start_index;
  parse_end_index_ = start_index+length;
  tokenizer_.UseReadOnlyBuffer(JSON_str_+start_index, length);
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
  int len = palStrlen(JSON_str);
  palJSONPrettyPrintInternal(JSON_str, 0, 0, len);
}