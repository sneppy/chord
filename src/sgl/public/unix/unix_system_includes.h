#pragma once

#include "core_types.h"

/////////////////////////////////////////////////
// Unix low-level includes                     //
/////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <wchar.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <utime.h>
#if PLATFORM_ENABLE_SIMD
#include <immintrin.h>
#endif
 // PLATFORM_ENABLE_SIMD

#include <cassert>