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

#include "libpal/pal_platform.h"
#include "libpal/pal_debug.h"
#include "libpal/pal_thread.h"

#if defined(PAL_PLATFORM_APPLE)

void palMutexLock(palMutex* m)
{
  pthread_mutex_lock(m);
}

bool palMutexTryLock(palMutex* m)
{
  pthread_mutex_trylock(m);
}

void palMutexUnlock(palMutex* m)
{
  pthread_mutex_unlock(m);
}

void palMutexInit(palMutex* m)
{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(m, &attr);
}

void palMutexDestroy(palMutex* m)
{
  pthread_mutex_destroy(m);
}


bool palThreadCreate(uint32_t stack_size, void (*thread_main)(uintptr_t), uintptr_t arg, uint32_t priority, const char* thread_name, palThread* thread)
{
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, stack_size);
  return pthread_create(thread, &attr, reinterpret_cast<void* (*)(void*)>(thread_main), (void*)arg) == 0;
}

void palThreadExit()
{
  pthread_exit(0);
}

void palThreadSetPriority(palThread thread, uint32_t priority)
{
}

uint32_t palThreadGetPriority(palThread thread)
{
  return 0;
}

void palThreadYield ()
{
  pthread_yield_np();
}

void palThreadJoin(palThread thread)
{
  pthread_join(thread, NULL);
}

palThread palThreadGetID()
{
  return pthread_self();
}

#endif  // defined(PAL_PLATFORM_APPLE)