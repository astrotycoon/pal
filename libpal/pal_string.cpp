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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "libpal/pal_platform.h"
#include "libpal/pal_algorithms.h"
#include "libpal/pal_string.h"
#include "libpal/pal_memory.h"
#include "libpal/pal_debug.h"

bool    palIsAlpha(char ch) {
  return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

bool    palIsDigit(char ch) {
  return ch >= '0' && ch <= '9';
}

bool    palIsHexDigit(char ch) {
  return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
}

bool  palIsWhiteSpace(char ch) {
  return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

int palDigitValue(char ch) {
  if (ch >= '0' && ch <= '9')
    return ch - '0';
  return 0;
}

int palHexDigitValue(char ch) {
  if (ch >= '0' && ch <= '9')
    return ch - '0';
  else if (ch >= 'a' && ch <= 'f')
    return 10 + (ch - 'a');
  else if (ch >= 'A' && ch <= 'F')
    return 10 + (ch - 'A');
  return 0;
}

int palStrlen(const char* str) {
  if (!str || *str == '\0')
    return 0;
  return strlen(str);
}

char* palStrdup(const char* str) {
  if (!str)
    return NULL;

#if defined(PAL_PLATFORM_WINDOWS)
  return _strdup(str);
#else
  return strdup(str);
#endif
}

char* palStrcpy(char* dest, const char* src) {
#if defined(PAL_COMPILER_MICROSOFT)
#pragma warning(push)
#pragma warning(disable : 4996)
  return strcpy(dest, src);
#pragma warning(pop)
#else
  return strcpy(dest, src);
#endif
}

int palStrcmp(const char* a, const char* b) {
  return strcmp(a, b);
}

int palStrncmp(const char* a, const char* b, int n) {
  return strncmp(a, b, n);
}

int internal_pal_printf_upper_bound(const char* format, va_list args) {
#if defined(PAL_COMPILER_MICROSOFT)
  int n = _vscprintf(format, args);
#else
  int n = vsnprintf(NULL, 0, format, args);
#endif
  /* on some versions of windows vsnprintf will return -1 instead of the number of characters needed to be written */
  palAssert(n >= 0);
  return n;
}

int pal_printf_upper_bound(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  int n = internal_pal_printf_upper_bound(format, ap);
  va_end(ap);
  return n;
}

char* palAsprintfInternal(const char* format, va_list args) {
  int len = internal_pal_printf_upper_bound(format, args)+1;
  palAssert(len >= 0);
  char* str = static_cast<char*>(malloc(len+1));
  int n = palSnprintfInternal(str, len, format, args);
  return str;
}

char* palAsprintf(const char* format, ...) {
  va_list args;
  va_start(args, format);
  char* str = palAsprintfInternal(format, args);
  va_end(args);
  return str;
}

int palSnprintfInternal(char* str, uint32_t size, const char* format, va_list args) {
#if defined(PAL_COMPILER_MICROSOFT)
#pragma warning(push)
#pragma warning(disable : 4996)
  int n = _vsnprintf(str, size, format, args);
#pragma warning(pop)
#else
  int n = vsnprintf(str, size, format, args);
#endif
  str[size-1] = '\0';
  return n;
}

int palSnprintf(char* str, uint32_t size, const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  int n = palSnprintfInternal(str, size, format, ap);
  va_end(ap);
  return n;
}

int palFindch(const char* str, char ch) {
  if (!str)
    return -1;

  int index = 0;
  while (*str != 0) {
    if (*str == ch) {
      return index;
    }
    index++;
    str++;
  }
  return -1;
}

bool palFindStr(const char* str, const char* findstr, int* start_out, int* end_out) {
  int str_len = palStrlen(str);
  int findstr_len = palStrlen(findstr);
  int start;
  int end;

  if (findstr_len > str_len) {
    return false;
  } else if (findstr_len == str_len) {
    if (palStrncmp(str, findstr, findstr_len) == 0) {
      start = 0;
      end = str_len;
      if (start_out)
        *start_out = start;
      if (end_out)
        *end_out = end;
      return true;
    }
    return false;
  }

  const char* search_point = str;
  for (int i = 0; i <= str_len - findstr_len; i++) {
    if (palStrncmp(search_point, findstr, findstr_len) == 0) {
      start = search_point - str;
      end = start + findstr_len;
      if (start_out)
        *start_out = start;
      if (end_out)
        *end_out = end;
      return true;
    }
    search_point++;
  }
  return false;
}
