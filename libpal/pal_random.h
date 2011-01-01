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

#ifndef __PAL_RANDOM_H
#define __PAL_RANDOM_H

/* Internally the numbers are generated with
 * SIMD-oriented Fast Mersenne Twister (SFMT): 
 * http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/SFMT/
 * If integer to float conversion is expensive, might be worth looking
 * Double precision SIMD-oriented Fast Mersenne Twister (dSFMT):
 * http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/SFMT/
 */

#include "pal_types.h"

void      palSeedRandom(uint32_t seed);
uint32_t  palGenerateRandom();
float     palGenerateRandomFloat();  // between 0.0 and 1.0

#endif
