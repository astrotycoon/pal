#include "libpal/pal_debug.h"
#include "libpal/pal_string.h"

#include "pal_string_test.h"


bool PalStringTest() {
  {
    palDynamicString s28_2;
    s28_2;
    palDynamicString s28;
    palDynamicString s48;

    s28_2.Append("Foo");
    s28.Append("Foo");
    s48 = s28;



    if (s28_2 != s28)
    {
      palBreakHere();
      return false;
    }

    if (s48 != s28)
    {
      palBreakHere();
      return false;
    }

    if ((s48 == s28) == false)
    {
      palBreakHere();
      return false;
    }

    if ((s28_2 == s28) == false)
    {
      palBreakHere();
      return false;
    }

    if (s28.Equals("Foo") == false)
    {
      palBreakHere();
      return false;
    }

    if (s48.Equals("Foo") == false)
    {
      palBreakHere();
      return false;
    }
  }
  return true;
  palDynamicString empty64;
  empty64.SetCapacity(64);
  if (empty64.GetCapacity() != 64) {
    palBreakHere();
    return false;
  }

  if (empty64.GetLength() != 0) {
    palBreakHere();
    return false;
  }

  if (empty64.Equals("") == false) {
    palBreakHere();
    return false;
  }

  palDynamicString str("123456789");
  if (str.GetLength() != 9) {
    palBreakHere();
    return false;
  }

  if (str.Equals("123456789") == false) {
    palBreakHere();
    return false;
  }

  palDynamicString copy_str(str);
  if (str.GetLength() != copy_str.GetLength()) {
    palBreakHere();
    return false;
  }

  if (str.Equals(copy_str.C()) == false) {
    palBreakHere();
    return false;
  }

  copy_str.Reset();
  if (copy_str.GetLength() != 0) {
    palBreakHere();
    return false;
  }

  str.SetLength(4);

  if (str.GetLength() != 4) {
    palBreakHere();
    return false;
  }

  if (str.Equals("1234") == false) {
    palBreakHere();
    return false;
  }

  palDynamicString printf_str;

  printf_str.AppendPrintf("%s - %d", "foo", 37);

  if (printf_str.Equals("foo - 37") == false) {
    palBreakHere();
    return false;
  }

  if (printf_str.GetLength() != 8) {
    palBreakHere();
    return false;
  }

  printf_str.Prepend(' ');
  if (printf_str.Equals(" foo - 37") == false) {
    palBreakHere();
    return false;
  }

  if (printf_str.GetLength() != 9) {
    palBreakHere();
    return false;
  }

  printf_str.Prepend("zz ");
  if (printf_str.Equals("zz  foo - 37") == false) {
    palBreakHere();
    return false;
  }

  if (printf_str.GetLength() != 12) {
    palBreakHere();
    return false;
  }

  printf_str.InsertPrintf(3, "%s", "INSERTED");

  if (printf_str.Equals("zz INSERTED foo - 37") == false) {
    palBreakHere();
    return false;
  }

  if (printf_str.GetLength() != 20) {
    palBreakHere();
    return false;
  }

  printf_str.Cut(3, 8);

  if (printf_str.Equals("zz  foo - 37") == false) {
    palBreakHere();
    return false;
  }

  if (printf_str.GetLength() != 12) {
    palBreakHere();
    return false;
  }

  return true;
}