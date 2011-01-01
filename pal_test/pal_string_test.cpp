#include "libpal/pal_debug.h"
#include "libpal/pal_string.h"

#include "pal_string_test.h"


bool PalStringTest ()
{
  {
    palString<28> s28_2;
    palString<28> s28;
    palString<> s48;

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
  palString<64> empty64;

  if (empty64.capacity() != 64)
  {
    palBreakHere();
    return false;
  }

  if (empty64.Length() != 0)
  {
    palBreakHere();
    return false;
  }

  if (empty64.Equals("") == false)
  {
    palBreakHere();
    return false;
  }

  palString<> str("123456789");
  if (str.Length() != 9)
  {
    palBreakHere();
    return false;
  }

  if (str.Equals("123456789") == false)
  {
    palBreakHere();
    return false;
  }

  palString<> copy_str(str);
  if (str.Length() != copy_str.Length())
  {
    palBreakHere();
    return false;
  }

  if (str.Equals(copy_str.C()) == false)
  {
    palBreakHere();
    return false;
  }

  copy_str.Clear();
  if (copy_str.Length() != 0)
  {
    palBreakHere();
    return false;
  }

  if (copy_str.Equals("") == false)
  {
    palBreakHere();
    return false;
  }

  str.Truncate(4);

  if (str.Length() != 4)
  {
    palBreakHere();
    return false;
  }

  if (str.Equals("1234") == false)
  {
    palBreakHere();
    return false;
  }

  palString<> printf_str;

  printf_str.AppendPrintf("%s - %d", "foo", 37);

  if (printf_str.Equals("foo - 37") == false)
  {
    palBreakHere();
    return false;
  }

  if (printf_str.Length() != 8)
  {
    palBreakHere();
    return false;
  }

  printf_str.PrependChar(' ');
  if (printf_str.Equals(" foo - 37") == false)
  {
    palBreakHere();
    return false;
  }

  if (printf_str.Length() != 9)
  {
    palBreakHere();
    return false;
  }

  printf_str.Prepend("zz ");
  if (printf_str.Equals("zz  foo - 37") == false)
  {
    palBreakHere();
    return false;
  }

  if (printf_str.Length() != 12)
  {
    palBreakHere();
    return false;
  }

  printf_str.InsertPrintf(3, "%s", "INSERTED");

  if (printf_str.Equals("zz INSERTED foo - 37") == false)
  {
    palBreakHere();
    return false;
  }

  if (printf_str.Length() != 20)
  {
    palBreakHere();
    return false;
  }

  printf_str.Remove(3, 8);

  if (printf_str.Equals("zz  foo - 37") == false)
  {
    palBreakHere();
    return false;
  }

  if (printf_str.Length() != 12)
  {
    palBreakHere();
    return false;
  }

  return true;
}