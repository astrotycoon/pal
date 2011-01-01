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

#ifndef LIBPAL_PAL_THREAD_APPLE_H_
#define LIBPAL_PAL_THREAD_APPLE_H_

#if defined(PAL_PLATFORM_APPLE)
#include <pthread.h>
#include <sched.h>

const int kPalThreadPriorityLowest = 0;
const int kPalThreadPriorityBelowNormal = 0;
const int kPalThreadPriorityNormal = 0;
const int kPalThreadPriorityAboveNormal = 0;
const int kPalThreadPriorityHighest = 0;

typedef pthread_mutex_t palMutex;
void palMutexLock(palMutex* m);
bool palMutexTryLock(palMutex* m);
void palMutexUnlock(palMutex* m);
void palMutexInit(palMutex* m);
void palMutexDestroy(palMutex* m);

typedef pthread_t palThread;

bool palThreadCreate(uint32_t stack_size, void (*thread_main)(uintptr_t), uintptr_t arg, uint32_t priority, const char* thread_name, palThread* thread);
void palThreadExit();
void palThreadSetPriority(palThread thread, uint32_t priority);
uint32_t palThreadGetPriority(palThread thread);
void palThreadYield ();
void palThreadJoin(palThread thread);
palThread palThreadGetID();

#endif  // defined(PAL_PLATFORM_APPLE)

#endif  // LIBPAL_PAL_THREAD_APPLE_H_