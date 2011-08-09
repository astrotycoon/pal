#include "libpal/libpal.h"
#include "libpal/pal_string.h"

#if defined(PAL_PLATFORM_WINDOWS)
#define FNAME "c:/temp/blah.txt"
#else
#define FNAME "/tmp/blah.txt"
#endif

const char* string = "John McCutchan";

bool fileWriteTest ()
{
  palFile pf;

  if (!pf.OpenForWritingTruncate(FNAME))
  {
    palBreakHere();
    return false;
  }
  int length = palStringLength(string)+1;
  if (pf.Write(string, length) != length)
  {
    palBreakHere();
    return false;
  }

  pf.Close();

  return true;
}

bool fileReadTest ()
{
  palFile pf;

  if (!pf.OpenForReading(FNAME))
  {
    palBreakHere();
    return false;
  }

  int length = palStringLength(string)+1;
  uint64_t read_length;
  char* b = (char*)pf.CopyContents(&read_length);
  if (!b)
  {
    palBreakHere();
    return false;
  }

  if (length != read_length)
  {
    palBreakHere();
    return false;
  }
  if (palStringCompare(b, string))
  {
    palBreakHere();
    return false;
  }

  g_StdProxyAllocator->Deallocate(b);
  pf.Close();
  return true;
}

bool PalFileTest ()
{
  fileWriteTest();
  fileReadTest();
  return true;
}