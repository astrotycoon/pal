
#include "libpal/libpal.h"

bool PalAtomicTest ()
{
  palAtomicInt32 a;
  a.Exchange(4);

  if (a.Load() != 4)
  {
    palBreakHere();
    return false;
  }
  if (++a != 5)
  {
    palBreakHere();
    return false;
  }
  if (--a != 4)
  {
    palBreakHere();
    return false;
  }
  a.FetchAdd(40);
  if (a.Load() != 44)
  {
    palBreakHere();
    return false;
  }
  a.FetchSub(40);
  if (a.Load() != 4)
  {
    palBreakHere();
    return false;
  }
  int32_t compare = 4;
  if (a.CompareExchange(compare, 7) == false)
  {
    palBreakHere();
    return false;
  }
  compare = 4;
  if (a.CompareExchange(compare, 44) == true)
  {
    palBreakHere();
    return false;
  }
  return true;
}