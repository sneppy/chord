#pragma once

#include "core_types.h"
//#include "hal/platform_memory.h"
#include "math_fwd.h"
#include "templates/is_void.h"
#include "templates/simd.h"

/**
 * A 4x4 templated matrix
 * with support for vector intrinsics
 */
template<typename T, bool = hasVectorIntrinsics(T, 4) && hasVectorIntrinsics(T, 8)>
struct Mat4
{
public:
	union
	{
		/// Matrix buffer
		T matrix[4][4];

		/// Linear buffer
		T array[16];

		struct
		{
			/// Single components
			/// @todo Maybe unnecessary
			/// @{
			T _a, _b, _c, _d, _e, _f, _g, _h, _i, _j, _k, _l, _m, _n, _o, _p;
			/// @}
		};
	};

public:
	/// Default constructor
	FORCE_INLINE Mat4() {}

	/// Elements constructor
	FORCE_INLINE Mat4(
		T __a, T __b, T __c, T __d,
		T __e, T __f, T __g, T __h,
		T __i, T __j, T __k, T __l,
		T __m, T __n, T __o, T __p
	) : array{
		__a, __b, __c, __d,
		__e, __f, __g, __h,
		__i, __j, __k, __l,
		__m, __n, __o, __p
	} {}

	/// Scalar constructor, fill matrix
	FORCE_INLINE Mat4(T s) : array{
		s, s, s, s,
		s, s, s, s,
		s, s, s, s,
		s, s, s, s
	} {}

	/// Array constructor
	FORCE_INLINE Mat4(const T _array[16]) { memcpy(array, _array, sizeof(array)); /** @todo Use @ref Memory::memcpy instead */ }
};

#if PLATFORM_ENABLE_SIMD
	#include "mat4_simd.h"
#endif