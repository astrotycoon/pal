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

#ifndef __PAL_CONSOLE_H
#define __PAL_CONSOLE_H

void palPrintf(const char* fmt, ...);
void palErrorPrintf(const char* fmt, ...);
void palAbortPrintf(const char* fmt, ...);

typedef void (*console_print_function)(int level, const char* message);
void set_console_print_callback(console_print_function callback);

#endif
