#ifndef __PAL_INT_H__
#define __PAL_INT_H__

#include "libpal/pal_platform.h"

#if defined(PAL_PLATFORM_WINDOWS)
#include "MSVCfixes/stdint.h"
#include "MSVCfixes/inttypes.h"
#include "MSVCfixes/stdbool.h"
#else
#include <stdint.h>
#endif

#endif
