#pragma once

#include "libpal/pal_platform.h"

// error codes are 32-bit signed integers
// no-error is 0
#define palMakeErrorCode(code) ((uint32_t)(1<<31)|0x8000|code)
