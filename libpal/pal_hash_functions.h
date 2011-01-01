/*
	Copyright (c) 2009 John McCutchan <john@johnmccutchan.com>

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.

	3. This notice may not be removed or altered from any source
	distribution.
*/

#ifndef __PAL_HASH_FUNCTIONS_H
#define __PAL_HASH_FUNCTIONS_H

#include "libpal/pal_platform.h"
#include "libpal/pal_string.h"
#include "libpal/pal_types.h"

template<class Key> struct palHashEqual
{
	bool operator()(const Key& x, const Key& y) const
	{
		return x == y;
	}
};

template<class Key> struct palHashEqualDereference
{
	bool operator()(const Key& x, const Key& y) const
	{
		return *x == *y;
	}
};


/* We use MurmurHash by Austin Appleby */

#if defined(PAL_PLATFORM_WINDOWS)
// this version is faster, but may not work on some architectures
#define mmix(h,k) { k *= m; k ^= k >> r; k *= m; h *= m; h ^= k; }
PAL_INLINE unsigned int palMurmurHashSeed ( const void * key, int len, unsigned int seed) {
  const unsigned int m = 0x5bd1e995;
  const int r = 24;
  unsigned int l = len;
  const unsigned char * data = (const unsigned char *)key;
  unsigned int h = seed;
  while(len >= 4) {
    unsigned int k = *(unsigned int*)data;
    mmix(h,k);
    data += 4;
    len -= 4;
  }
  unsigned int t = 0;
  switch(len) {
  case 3: t ^= data[2] << 16;
  case 2: t ^= data[1] << 8;
  case 1: t ^= data[0];
  };

  mmix(h,t);
  mmix(h,l);
  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;
  return h;
}
#undef mmix
#else
PAL_INLINE unsigned int palMurmurHashSeed( const void* key, int len, unsigned int seed) {
  const unsigned int m = 0x5bd1e995;
  const int r = 24;
  unsigned int h = seed ^ len;
  const unsigned char * data = (const unsigned char *)key;
  while(len >= 4) {
    unsigned int k;

    k  = data[0];
    k |= data[1] << 8;
    k |= data[2] << 16;
    k |= data[3] << 24;

    k *= m; 
    k ^= k >> r; 
    k *= m;

    h *= m;
    h ^= k;

    data += 4;
    len -= 4;
  }

  switch(len) {
  case 3: h ^= data[2] << 16;
  case 2: h ^= data[1] << 8;
  case 1: h ^= data[0];
          h *= m;
  };
  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;
  return h;
} 
#endif

PAL_INLINE unsigned int palMurmurHash (const void* key, int len)
{
  return palMurmurHashSeed(key, len, 0xc58f1a7b);
}

// base palHashFunction template
template<typename T>  struct palHashFunction
{
	unsigned int operator()(const T& key) const;
};

/* Standard types uint64_t, uint32_t, const void* and const char* implementations */
template<>
struct palHashFunction<uint64_t>
{
	unsigned int operator()(const uint64_t& key) const
	{
    return palMurmurHash(reinterpret_cast<const void*>(&key), sizeof(uint64_t));
	}
};

template<> 
struct palHashFunction<uint32_t>
{
	unsigned int operator()(const uint32_t& key) const
	{
    return palMurmurHash(reinterpret_cast<const void*>(&key), sizeof(uint32_t));
	}
};

template<> 
struct palHashFunction<int>
{
  unsigned int operator()(const int& key) const
  {
    return palMurmurHash(reinterpret_cast<const void*>(&key), sizeof(int));
  }
};

/* conflicts with uint32_t/uint64_t hash functions
template<>
struct palHashFunction<uintptr_t> {
  unsigned int operator()(const uintptr_t& key) const {
    return palMurmurHash(reinterpret_cast<void*>(&key), sizeof(uintptr_t));
  }
};
*/

template<>
struct palHashFunction<const void*> {
  unsigned int operator()(const void* const& key) const {
    return palMurmurHash(key, sizeof(void*));
  }
};

template<>
struct palHashFunction<const char*>
{
	unsigned int operator()(const char* const& key) const
	{
    return palMurmurHash(key, palStrlen(key));
	}
};

#endif
