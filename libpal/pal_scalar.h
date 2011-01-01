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

#ifndef __PAL_SCALAR_H
#define __PAL_SCALAR_H

#include "libpal/pal_platform.h"

#include <math.h>
#include <float.h>

typedef float scalar;
#define SCALAR_FLOAT

#if defined(PAL_PLATFORM_PS3) || defined(PAL_PLATFORM_APPLE)
//const scalar M_PI = (3.14159265358979323846f);
const scalar M_HALF_PI(1.57079632679489661923f);
const scalar M_RADS_PER_DEG(3.14159265358979323846f / 180.0f);
const scalar M_DEGS_PER_RAD(180.0f / 3.14159265358979323846f);
const scalar SCALAR_EPSILON(FLT_EPSILON);
const scalar SCALAR_MATRIX_INVERSE_EPSILON(1e-6f);
const scalar SCALAR_INFINITY(FLT_MAX);
#else
const scalar M_PI = (3.14159265358979323846f);
const scalar M_HALF_PI(1.57079632679489661923f);
const scalar M_RADS_PER_DEG(3.14159265358979323846f / 180.0f);
const scalar M_DEGS_PER_RAD(180.0f / 3.14159265358979323846f);
const scalar SCALAR_EPSILON(FLT_EPSILON);
const scalar SCALAR_MATRIX_INVERSE_EPSILON(1e-6f);
const scalar SCALAR_INFINITY(FLT_MAX);
#endif

#endif

