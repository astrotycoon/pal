#pragma once

#include "libpal/pal_platform.h"

#define PAL_OK 0
#define palMakeErrorCode(group, code) (0x80000000|((group & 0xff) << 16)|(code & 0xffff))