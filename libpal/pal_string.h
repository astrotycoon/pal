/*
  Copyright (c) 2009 John McCutchan <john@johnmccutchan.com>

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

#ifndef LIBPAL_PAL_STRING_H_
#define LIBPAL_PAL_STRING_H_

#include <stdarg.h>
#include "libpal/pal_types.h"
#include "libpal/pal_memory.h"

char* palAsprintfInternal(const char* format, va_list args);
int palSnprintfInternal(char* str, uint32_t size, const char* format, va_list args);

bool  palIsAlpha(char ch);
bool  palIsDigit(char ch);
bool  palIsHexDigit(char ch);
bool  palIsWhiteSpace(char ch);
int   palDigitValue(char ch);
int   palHexDigitValue(char ch);
int   palStrlen(const char* str);
char* palStrdup(const char* str);
char* palStrcpy(char* dest, const char* src);
int   palStrcmp(const char* a, const char* b);
int   palStrncmp(const char* a, const char* b, int n);
int   palSnprintf(char* str, uint32_t size, const char* format, ...);
char* palAsprintf(const char* format, ...);
int   palFindch(const char* str, char ch);
bool  palFindStr(const char* str, const char* findstr, int* start, int* end);


// ideally palString should be 64 bytes
// on 32-bit architectures the struct adds up to: 12 bytes (52 bytes to spare)
// on 64-bit architectures it is 16 bytes (48 bytes to spare)
// sizeof(palString) = PAL_STRING_DEFAULT_CAPACITY + (12 or 16)


template<int PAL_STRING_DEFAULT_CAPACITY = 48>
class palString {
 public:
  palString() {
    str_ = &default_buffer_[0];
    str_capacity_ = PAL_STRING_DEFAULT_CAPACITY;
    str_len_ = 0;
    str_[0] = '\0';
  }

  explicit palString(const char* init_string) {
    str_ = &default_buffer_[0];
    str_capacity_ = PAL_STRING_DEFAULT_CAPACITY;
    str_len_ = 0;
    str_[0] = '\0';
    Set(init_string);
  }

  ~palString() {
    if (str_ != &default_buffer_[0]) {
      palFree(str_);
    }
  }

  template<int OTHER_STRING_CAPACITY> friend class palString;

  // copy
  explicit palString(const palString<PAL_STRING_DEFAULT_CAPACITY>& that);
  // assignment
  palString<PAL_STRING_DEFAULT_CAPACITY>& operator = (const palString<PAL_STRING_DEFAULT_CAPACITY>& that);
  template<int OTHER_STRING_CAPACITY>
  palString<PAL_STRING_DEFAULT_CAPACITY>& operator = (const palString<OTHER_STRING_CAPACITY>& that);

  bool operator == (const palString<PAL_STRING_DEFAULT_CAPACITY>& that);
  template<int OTHER_STRING_CAPACITY>
  bool operator == (const palString<OTHER_STRING_CAPACITY>& that);
  bool operator != (const palString<PAL_STRING_DEFAULT_CAPACITY>& that);
  template<int OTHER_STRING_CAPACITY>
  bool operator != (const palString<OTHER_STRING_CAPACITY>& that);
  

  void Set(const char* str);
  void SetSize(int new_size);
  int Truncate(int len);
  const char* C() const;
  int Length() const;
  bool Equals(const char* test) const;
  void Clear();
  void Append(const char* append_str);
  void AppendPrintf(const char* format, ...);
  void AppendChar(const char ch);
  void AppendLength(const char* append_str, int length);
  void Prepend(const char* prepend_str);
  void PrependPrintf(const char* format, ...);
  void PrependChar(const char ch);
  void PrependLength(const char* prepend_str, int length);
  void Insert(int position, const char* str);
  void InsertPrintf(int position, const char* format, ...);
  void InsertChar(int position, const char ch);
  void InsertLength(int position, const char* insert_str, int length);
  void Remove(int position, int length);

  int capacity() { return str_capacity_; };
 protected:
  void expandIfNeeded(int extraLength);
  void resize(int newSize);

  char* str_;  // 4 or 8 bytes
  int str_len_;  // 4 bytes
  int str_capacity_;  // 4 bytes
  char default_buffer_[PAL_STRING_DEFAULT_CAPACITY];
};

#include "pal_string_inl.h"  // template implementation of palString class


#endif  // LIBPAL_PAL_STRING_H_
