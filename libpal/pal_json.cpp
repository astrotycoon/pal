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

bool palJSONToken::NameMatch(const char* name) {
  return false;
}

float palJSONToken::GetAsFloat() {
  return 0.0f;
}

int palJSONToken::GetAsInt() {
  return 0;
}

bool palJSONToken::GetAsBool() {
  return false;
}

void* palJSONToken::GetAsPointer() {
  return NULL;
}

palJSONToken* palJSONFindTokenWithName(palJSONToken* token_buffer, int token_buffer_length, const char* name) {
  return NULL;
}

palJSONParser::palJSONParser() {
  JSON_str_ = 0;
  JSON_str_len_ = 0;
  parse_current_index_ = 0;
  parse_end_index_ = 0;
}

void palJSONParser::Init(const char* JSON_str) {
  JSON_str_ = JSON_str;
  JSON_str_len_ = palStrlen(JSON_str_);
  parse_current_index_ = 0;
  parse_end_index_ = 0;
}

void palJSONParser::StartParse() {
  parse_current_index_ = 0;
  parse_end_index_ = JSON_str_len_+1;
}
  
void palJSONParser::StartParse(int start_index, int end_index) {
  parse_current_index_ = start_index;
  parse_end_index_ = end_index;
}

int palJSONParser::Parse(palJSONToken* token_buffer, int token_buffer_size) {
  int token_buffer_insert_index = 0;

  const char kLBrace = '{';
  const char kRBrace = '}';
  const char kLBracket = '[';
  const char kRBracket = ']';
  const char kColon = ':';
  const char kComma = ',';
  const char kQuote = '\"';
  const char kDot = '.';
  const char kN = 'n';
  const char kT = 't';
  const char kF = 'f';
  const char kMinus = '-';

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

  while (parse_current_index_ < parse_end_index_) {
    if (token_buffer_insert_index == token_buffer_size) {
      // we have filled up the buffer, quit.
      return token_buffer_insert_index;
    }

    char ch = JSON_str_[parse_current_index_];

    switch (state) {
    case kStateStart:
      {
        // determine what we are parsing by the first character
        if (ch == kLBrace) {
          state = kStateObject;
          lbrace_count++;
          last_ch = kRBrace;

          token_buffer[token_buffer_insert_index].type = kJSONTokenTypeMap;
          token_buffer[token_buffer_insert_index].first_index = parse_current_index_;
        } else if (ch == kLBracket) {
          state = kStateArray;
          lbracket_count++;
          last_ch = kRBracket;

          token_buffer[token_buffer_insert_index].type = kJSONTokenTypeArray;
          token_buffer[token_buffer_insert_index].first_index = parse_current_index_;
        } else if (ch == kT) {
          // true
          state = kStateValueTrue;

          token_buffer[token_buffer_insert_index].type = kJSONTokenTypeValueTrue;
          token_buffer[token_buffer_insert_index].first_index = parse_current_index_;
        } else if (ch == kF) {
          // false
          state = kStateValueFalse;

          token_buffer[token_buffer_insert_index].type = kJSONTokenTypeValueFalse;
          token_buffer[token_buffer_insert_index].first_index = parse_current_index_;
        } else if (ch == kN) {
          // null
          state = kStateValueNull;

          token_buffer[token_buffer_insert_index].type = kJSONTokenTypeValueNull;
          token_buffer[token_buffer_insert_index].first_index = parse_current_index_;
        } else if (ch == kMinus || palIsDigit(ch)) {
          // a number
          state = kStateValueNumber;

          token_buffer[token_buffer_insert_index].type = kJSONTokenTypeValueNumber;
          token_buffer[token_buffer_insert_index].first_index = parse_current_index_;
        } else if (ch == kQuote) {
          // a string
          state = kStateValueString;

          token_buffer[token_buffer_insert_index].type = kJSONTokenTypeValueString;
          token_buffer[token_buffer_insert_index].first_index = parse_current_index_;
        }
        break;
      }
    case kStateObject:
      {
        // parsing an object(map)
        // need to parse all top level items of the map and stuff them into tokens
        break;
      }
    case kStateArray:
      {
        // parsing an array
        // need to parse all top level items of the array and stuff them into tokens
        break;
      }
    case kStateValueFalse:
      {
        break;
      }
    case kStateValueTrue:
      {
        break;
      }
    case kStateValueNull:
      {
        break;
      }
    case kStateValueNumber:
      {
        break;
      }
    case kStateValueString:
      {
        break;
      }
    }

    parse_current_index_++;
  }

  return token_buffer_insert_index;
}