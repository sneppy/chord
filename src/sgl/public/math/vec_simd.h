#pragma once

#include "vec.h"

/**
 * Vector intrinsics specialization
 */
template<typename T, uint32 N>
struct GCC_ALIGN(32) Vec<T, N, true>
{
public:
	/// Intrinsics operations class
	using VecOps	= Simd::Vector<T, 4>;
	using DVecOps	= Simd::Vector<T, 8>;

	/// Intrinsics data types
	using VecT	= typename VecOps::Type;
	using DVecT	= typename DVecOps::Type;

	/// Array data
	T array[N];

public:
	/// Default constructor, uninitialized
	FORCE_INLINE Vec() {};
};