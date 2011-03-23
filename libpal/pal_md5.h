#pragma once

/* Adapted for pal 2011 */

/* MD5.H - header file for MD5C.C
 */

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 */

#include "libpal/pal_types.h"

class palMD5 {
  uint32_t state[4]; /* state (ABCD) */
  uint32_t count[2]; /* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64]; /* input buffer */
public:
  palMD5();

  void Reset();

  void Update(unsigned char* buf, unsigned int buf_length);

  void Finalize();

  // md5 is 16-bytes long
  void GetMD5(unsigned char* md5);
};


