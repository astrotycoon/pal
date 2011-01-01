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

#if defined(PAL_PLATFORM_WINDOWS)

#define MS_VC_EXCEPTION 0x406D1388

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
  DWORD dwType; // Must be 0x1000.
  LPCSTR szName; // Pointer to name (in user addr space).
  DWORD dwThreadID; // Thread ID (-1=caller thread).
  DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void __SetThreadName(DWORD dwThreadID, const char* threadName)
{
  THREADNAME_INFO info;
  info.dwType = 0x1000;
  info.szName = threadName;
  info.dwThreadID = dwThreadID;
  info.dwFlags = 0;

  __try
  {
    RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
  }
  __except(EXCEPTION_EXECUTE_HANDLER)
  {
  }
}


void palMutexLock(palMutex* m) {
  EnterCriticalSection(m);
}
bool palMutexTryLock(palMutex* m) {
  return TryEnterCriticalSection(m) != 0;
}
void palMutexUnlock(palMutex* m) {
  LeaveCriticalSection(m);
}
void palMutexInit(palMutex* m) {
  InitializeCriticalSection(m);
}
void palMutexDestroy(palMutex* m) {
  DeleteCriticalSection(m);
}

#ifndef __CLR_OR_STD_CALL
#if defined (_M_CEE) || defined (MRTDLL)
#define __CLR_OR_STD_CALL   __clrcall
#else  /* defined (_M_CEE) || defined (MRTDLL) */
#define __CLR_OR_STD_CALL   __stdcall
#endif  /* defined (_M_CEE) || defined (MRTDLL) */
#endif  /* __CLR_OR_STD_CALL */

bool palThreadCreate(uint32_t stack_size, void (*thread_main)(uintptr_t), uintptr_t arg, uint32_t priority, const char* thread_name, palThread* thread)
{
  palThread thread_local = _beginthreadex(NULL, stack_size, reinterpret_cast<unsigned int (__CLR_OR_STD_CALL*)(void*)>(thread_main), reinterpret_cast<void*>(arg), 0, NULL);
  if (!thread_local)
  {
    *thread = 0;
    return false;
  }

  __SetThreadName(GetThreadId(reinterpret_cast<HANDLE>(thread_local)), thread_name);
  *thread = thread_local;

  return true;
}

void palThreadExit()
{
  _endthreadex(0);
}

void palThreadSetPriority(palThread thread, uint32_t priority)
{

}

uint32_t palThreadGetPriority(palThread thread)
{
  return -1;
}

void palThreadYield ()
{
  Sleep(0); // yield
}

void palThreadJoin(palThread thread)
{
  WaitForSingleObject(reinterpret_cast<HANDLE>(thread), INFINITE);
  CloseHandle(reinterpret_cast<HANDLE>(thread));
}

palThread palThreadGetID()
{
  return GetCurrentThreadId();
}

#endif  // defined(PAL_PLATFORM_WINDOWS)
