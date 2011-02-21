
#include "libpal/libpal.h"

class CounterClass {
  int count;

public:
  CounterClass() { count = 0; }

  void IncrementCount() { count++; }
  void DecrementCount() { count--; }
  void AddToCount(int x) { count += x; }
  void DecToCount(int x) { count -= x; }
  void PrintCount() const { printf("%d\n", count); }
  int GetCount() { return count; }

  static int StrangeFunction(int z, float f) { palPrintf("Strange."); return 77; }

};

bool PalEventTest() {

  typedef palDelegate<void ()> IncrementDelegate;
  typedef palDelegate<void ()> DecrementDelegate;
  typedef palDelegate<void (int)> AddDelegate;
  typedef palDelegate<int ()> GetDelegate;
  typedef palDelegate<int (int, float)> StrangeDelegate;

  //printf("%d %d %d\n", sizeof(IncrementDelegate), sizeof(AddDelegate), sizeof(StrangeDelegate));
  typedef palEvent<void ()> SimpleEvent;
  typedef palEvent<void (int)> IntEvent;

  /* Member function delegate test */
  {
    CounterClass cc;

    IncrementDelegate id(&cc, &CounterClass::IncrementCount);

    palAssertBreak(cc.GetCount() == 0);
    id();
    palAssertBreak(cc.GetCount() == 1);
    id();
    palAssertBreak(cc.GetCount() == 2);

    // assignment operator
    IncrementDelegate id2 = id;

    id2();
    palAssertBreak(cc.GetCount() == 3);
    id2();
    palAssertBreak(cc.GetCount() == 4);

    // copy constructor
    IncrementDelegate id3(id2);

    id3();
    palAssertBreak(cc.GetCount() == 5);
    id3();
    palAssertBreak(cc.GetCount() == 6);
  }

  /* Static function delegate test */
  {
    StrangeDelegate sd;

    sd = StrangeDelegate(&CounterClass::StrangeFunction);

    int result = sd(3, 3.14159f);

    palAssertBreak(result == 77);
  }

  /* Event */
  {
    CounterClass cc;
    SimpleEvent event;

    SimpleEvent::DelegateType id(&cc, &CounterClass::IncrementCount);

    event.Register(id);
    event.Register(&cc, &CounterClass::DecrementCount);
    event.Register(&cc, &CounterClass::PrintCount);

    event.Fire();

    event.Unregister(&cc, &CounterClass::PrintCount);
    event.Unregister(&cc, &CounterClass::DecrementCount);
    event.Unregister(id);

    palAssertBreak(cc.GetCount() == 0);

  }

  {
    /* Event with a parameter */
    CounterClass cc;
    IntEvent event;

    IntEvent::DelegateType dc(&cc, &CounterClass::DecToCount);
    event.Register(&cc, &CounterClass::AddToCount);

    event.Fire(3);

    palAssertBreak(cc.GetCount() == 3);

    event.Register(dc);

    event.Fire(3);

    palAssertBreak(cc.GetCount() == 3);

    event.Unregister(&cc, &CounterClass::AddToCount);

    event.Fire(3);

    palAssertBreak(cc.GetCount() == 0);
  }
  
  palObjectIdTable<char> table;

  //palBreakHere();
  return true;
}