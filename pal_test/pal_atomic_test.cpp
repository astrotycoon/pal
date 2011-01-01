
#include "libpal/libpal.h"

bool PalAtomicTest ()
{
  palAtomic a;
  palAtomicSet(4, &a);
  if (palAtomicGet(&a) != 4)
  {
    palBreakHere();
    return false;
  }
  if (palAtomicInc(&a) != 5)
  {
    palBreakHere();
    return false;
  }
  if (palAtomicDec(&a) != 4)
  {
    palBreakHere();
    return false;
  }
  if (palAtomicAdd(40, &a) != 44)
  {
    palBreakHere();
    return false;
  }
  if (palAtomicSub(40, &a) != 4)
  {
    palBreakHere();
    return false;
  }
  if (palAtomicCAS(4, 7, &a) == false)
  {
    palBreakHere();
    return false;
  }
  if (palAtomicCAS(4, 44, &a) == true)
  {
    palBreakHere();
    return false;
  }
  return true;
}