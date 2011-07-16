#include "libpal/libpal.h"

#include "pal_thread_test.h"

class CountDown {
  palAtomicInt32 countdown;
public:

  CountDown(int32_t iv) {
    countdown.Store(iv);
  }

  void DecrementUntilNegative(uintptr_t ignored) {
    bool exit = false;
    do  {
      int d = countdown.FetchSub(1);
      palPrintf("[%s] *a = %d\n", palThread::GetCurrentThread()->GetName(), d);
      if (d <= 0)
        exit = true;
    } while (exit == false);
    palPrintf("[%s] EXIT\n", palThread::GetCurrentThread()->GetName());
    palThread::Exit(0);
  }
};

void thread_args_test(uintptr_t seven) {
  if (seven != 7) {
    palBreakHere();
  }
  palThread::Exit(0);
}

static CountDown CD0(33);
static CountDown CD1(55);

static palAtomicInt32 countdown(20);

void thread_countdown(uintptr_t seven) {
  palAtomicInt32* a = reinterpret_cast<palAtomicInt32*>(seven);

  bool exit = false;
  do  {
    int d = a->FetchSub(1);
    palPrintf("[%s] *a = %d\n", palThread::GetCurrentThread()->GetName(), d);
    if (d <= 0)
      exit = true;
  } while (exit == false);
  palPrintf("[%s] EXIT\n", palThread::GetCurrentThread()->GetName());
  palThread::Exit(0);
}

bool PalThreadTest () {
  palMutexDescription my_mutex_desc;
  my_mutex_desc.initial_ownership = false;
  my_mutex_desc.name = palString<>("My Mutex");
  palMutex my_mutex(my_mutex_desc);

  my_mutex.Create();
  my_mutex.Acquire();
  my_mutex.Release();
  my_mutex.Destroy();

  // mutexes are recursive
  my_mutex.Create();
  my_mutex.Acquire();
  my_mutex.Acquire();
  my_mutex.Release();
  my_mutex.Release();
  my_mutex.Destroy();

  palThreadDescription t_desc;
  t_desc.name = palString<>("Test thread");
  t_desc.start_method = palThreadStart(thread_args_test);
  palThread t(t_desc);
  t.Start(0x7);
  t.Join(NULL);

  {
    palThreadDescription ta_desc[3];
    ta_desc[0].name = palString<>("Countdown Thread 0");
    ta_desc[0].start_method = palThreadStart(thread_countdown);
    ta_desc[1].name = palString<>("Countdown Thread 1");
    ta_desc[1].start_method = palThreadStart(thread_countdown);
    ta_desc[2].name = palString<>("Countdown Thread 2");
    ta_desc[2].start_method = palThreadStart(thread_countdown);
    palThread t0(ta_desc[0]);
    palThread t1(ta_desc[1]);
    palThread t2(ta_desc[2]);
    palThread* ta[3] = { &t0, &t1, &t2 };
    for (int tc = 0; tc < 3; tc++) {
      ta[tc]->Start(reinterpret_cast<uintptr_t>(&countdown));
    }
    for (int tc = 0; tc < 3; tc++) {
      ta[tc]->Join(NULL);
    }
  }

  {
    palThreadDescription tcd0_desc;
    palThreadDescription tcd1_desc;
    tcd0_desc.start_method = palThreadStart(&CD0, &CountDown::DecrementUntilNegative);
    tcd1_desc.start_method = palThreadStart(&CD1, &CountDown::DecrementUntilNegative);
    tcd0_desc.name = palString<>("Countdown Class Thread 0");
    tcd1_desc.name = palString<>("Countdown Class Thread 1");
    palThread tcd0(tcd0_desc);
    palThread tcd1(tcd1_desc);
    tcd0.Start(0);
    tcd1.Start(2);
    tcd0.Join(NULL);
    tcd1.Join(NULL);
  }
  
  return true;
}
