#pragma once

#include "core_types.h"
#include "hal/platform_math.h"

/// Returns true if <T, N> has a vector intrinsics type
#define hasVectorIntrinsics(T, N) !IsVoid<typename Simd::Vector<T, N>::Type>::value

namespace Simd
{
	/**
	 * Expose a vector intrinsic type for the given
	 * type and size and provides convenient
	 * methods to manipulate vector intrinsics
	 */
	template<typename T, uint8 N>
	struct Vector
	{
		using Type = void;
	};
} // Simd

#if PLATFORM_ENABLE_SIMD

namespace Simd
{
	/**
	 * Comparison operations
	 */
	enum CompareOp : int32
	{
		CMP_EQ	= _MM_CMPINT_EQ,
		CMP_NE	= _MM_CMPINT_NE,
		CMP_GT	= _MM_CMPINT_GT,
		CMP_LT	= _MM_CMPINT_LT,
		CMP_GE	= _MM_CMPINT_GE,
		CMP_LE	= _MM_CMPINT_LE,
		CMP_NLT	= _MM_CMPINT_NLT,
		CMP_NLE	= _MM_CMPINT_NLE
	};
} // Simd

#endif

/// Include specializations
#include "simd_float.h"
