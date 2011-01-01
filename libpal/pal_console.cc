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

#if defined(PAL_PLATFORM_WINDOWS)
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#else
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#endif

#include "libpal/pal_platform.h"
#include "libpal/pal_console.h"


static void
default_console_print_function(int level, const char* message) {
  switch (level) {
    case 0:
      break;
    case 1:
      printf("ERROR: ");
      break;
    case 2:
      printf("BUG: ");
      break;
  }

  printf("%s", message);
  fflush(stdout);
}

static console_print_function _console_print = default_console_print_function;

void palPrintf(const char* fmt, ...) {
  char message[1024];
  va_list ap;
  va_start(ap, fmt);
#if defined(PAL_COMPILER_MICROSOFT)
  vsnprintf_s(message, sizeof(message), sizeof(message)-1, fmt, ap);
#else
  vsnprintf(message, sizeof(message), fmt, ap);
#endif
  va_end(ap);
  _console_print(0, message);
}

void palErrorPrintf(const char* fmt, ...) {
  char message[1024];
  va_list ap;
  va_start(ap, fmt);
#if defined(PAL_COMPILER_MICROSOFT)
  vsnprintf_s(message, sizeof(message), sizeof(message)-1, fmt, ap);
#else
  vsnprintf(message, sizeof(message), fmt, ap);
#endif
  va_end(ap);
  _console_print(1, message);
}

void palAbortPrintf(const char* fmt, ...) {
  char message[1024];
  va_list ap;
  va_start(ap, fmt);
#if defined(PAL_COMPILER_MICROSOFT)
  vsnprintf_s(message, sizeof(message), sizeof(message)-1, fmt, ap);
#else
  vsnprintf(message, sizeof(message), fmt, ap);
#endif
  va_end(ap);
  _console_print(2, message);
  abort();
}

void set_console_print_callback(console_print_function callback) {
  _console_print = callback;
}
