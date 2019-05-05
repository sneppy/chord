#pragma once

#include "core_types.h"
#include "hal/platform_math.h"
#include "templates/is_void.h"
#include "templates/simd.h"

/**
 * A fixed-length mathematical vector
 */
template<typename T, uint32 N, bool = hasVectorIntrinsics(T, 8) & (N > 32)>
struct Vec
{
public:
	/// Array data
	T array[N];

public:
	/// Default constructor, uninitialized
	FORCE_INLINE Vec() {};
};

#if PLATFORM_ENABLE_SIMD
	#include "vec_simd.h"
#endif