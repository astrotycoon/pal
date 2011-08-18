#include "libpal/libpal.h"
#include "libpal/pal_string.h"

#if defined(PAL_PLATFORM_WINDOWS)
#define FNAME "c:/temp/blah.txt"
#else
#define FNAME "/tmp/blah.txt"
#endif

const char* string = "John McCutchan";

bool fileWriteTest() {
  palFile pf;

  if (pf.OpenForWritingTruncate(FNAME) != 0) {
    palBreakHere();
    return false;
  }

  int length = palStringLength(string)+1;
  if (pf.Write(string, length) != length) {
    palBreakHere();
    return false;
  }

  pf.Close();

  return true;
}

bool fileReadTest() {
  palFile pf;

  if (pf.OpenForReading(FNAME) != 0) {
    palBreakHere();
    return false;
  }

  int length = palStringLength(string)+1;
  palMemBlob blob;
  pf.CopyContents(&blob);
  if (blob.GetPtr(0) == NULL) {
    palBreakHere();
    return false;
  }

  if (length != blob.GetBufferSize()) {
    palBreakHere();
    return false;
  }

  if (palStringCompare(blob.GetPtr<const char>(0), string)) {
    palBreakHere();
    return false;
  }

  palFile::FreeFileContents(&blob);
  pf.Close();
  return true;
}

bool PalFileTest() {
  fileWriteTest();
  fileReadTest();
  return true;
}