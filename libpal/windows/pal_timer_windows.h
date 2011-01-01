/*
	Copyright (c) 2010 John McCutchan <john@johnmccutchan.com>

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

#ifndef LIBPAL_PAL_TIMER_WINDOWS_H_
#define LIBPAL_PAL_TIMER_WINDOWS_H_

#include <windows.h>
typedef LONGLONG palTimerTick;

PAL_INLINE palTimerTick palTimerGetFrequency() {
  palTimerTick frequency;
  QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
  return frequency;
}

PAL_INLINE palTimerTick palTimerGetTicks() {
  palTimerTick t;
  QueryPerformanceCounter((LARGE_INTEGER*)&t);
  return t;
}

PAL_INLINE float palTimerGetSeconds(palTimerTick ticks) {
  static palTimerTick frequency = palTimerGetFrequency();
  float dt_s = static_cast<float>(ticks)/static_cast<float>(frequency);
  return dt_s;
}

#endif  // LIBPAL_PAL_TIMER_WINDOWS_H_