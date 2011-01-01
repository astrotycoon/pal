#include "libpal/libpal.h"

#include "pal_thread_test.h"

//palMutex static_mutex = palMutexStaticInit;

void thread_args_test(uintptr_t seven)
{
  if (seven != 7)
  {
    palBreakHere();
  }
  palThreadExit();
}

static palAtomic countdown = { 44 };

void thread_countdown(uintptr_t seven)
{
  palAtomic* a = reinterpret_cast<palAtomic*>(seven);

  bool exit = false;
  do 
  {
    int d = palAtomicDec(a);
    palPrintf("[%d] *a = %d\n", palThreadGetID(), d);
    if (d <= 0)
      exit = true;
  } while (exit == false);
  palPrintf("[%d] EXIT\n", palThreadGetID());
  palThreadExit();
}

bool PalThreadTest ()
{
  palMutex my_mutex;

  palMutexInit(&my_mutex);
  palMutexLock(&my_mutex);
  palMutexUnlock(&my_mutex);
  palMutexDestroy(&my_mutex);

  // mutexes are recursive
  palMutexInit(&my_mutex);
  palMutexLock(&my_mutex);
  palMutexLock(&my_mutex);
  palMutexUnlock(&my_mutex);
  palMutexUnlock(&my_mutex);
  palMutexDestroy(&my_mutex);

  palThread t;

  if (palThreadCreate(0x800000, thread_args_test, 0x7, kPalThreadPriorityNormal, "fun thread", &t) == false)
  {
    palBreakHere();
    return false;
  }
  palThreadJoin(t);

  palThread ta[3];
  for (int tc = 0; tc < 3; tc++)
  {
    palThreadCreate(16 * 1024, thread_countdown, reinterpret_cast<uintptr_t>(&countdown), kPalThreadPriorityNormal, "countdown thread N", &ta[tc]);
  }
  for (int tc = 0; tc < 3; tc++)
  {
    palThreadJoin(ta[tc]);
  }
  return true;
}