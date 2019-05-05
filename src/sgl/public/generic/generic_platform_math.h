#pragma once

#include "core_types.h"
#include "hal/platform_crt.h"
#include "templates/is_integral.h"

/**
 * @struct GenericPlatformMath generic/generic_platform_math.h
 * @brief Common math functions
 */
struct GenericPlatformMath
{
	/**
	 * @brief Trigonometric functions
	 * @{
	 */
	static FORCE_INLINE float32 sin(float32 s)	{ return ::sinf(s); }
	static FORCE_INLINE float32 cos(float32 s)	{ return ::cosf(s); }
	static FORCE_INLINE float32 tan(float32 s)	{ return ::tanf(s); }
	static FORCE_INLINE float32 asin(float32 s)	{ return ::asinf(s); }
	static FORCE_INLINE float32 acos(float32 s)	{ return ::acosf(s); }
	static FORCE_INLINE float32 atan(float32 s)	{ return ::atanf(s); }
	/** @} */

	/**
	 * @brief Power functions
	 * @{
	 */
	static FORCE_INLINE float32 sqrt(float32 s)				{ return ::sqrtf(s); }
	static FORCE_INLINE float32 pow(float32 a, float32 b)	{ return ::powf(a, b); }
	/** @} */

	/**
	 * @brief Random functions
	 * @{
	 */
	static FORCE_INLINE void initRand(int32 seed)	{ ::srand(seed); }
	static FORCE_INLINE int32 rand()				{ return ::rand(); }
	static FORCE_INLINE float32 randf()				{ return ::rand() / static_cast<float32>(RAND_MAX); }
	/** @} */

	/**
	 * @brief Templated common functions
	 * @{
	 */
	template<typename T>
	static CONSTEXPR FORCE_INLINE T abs(const T s)
	{
		return s < T(0) ? -s : s;
	}

	template<typename T>
	static CONSTEXPR FORCE_INLINE T sign(const T s)
	{
		return s < T(0) ? T(-1) : (s > T(0) ? T(1) : T(0));
	}

	template<typename T>
	static CONSTEXPR FORCE_INLINE T max(const T a, const T b)
	{
		return a > b ? a : b;
	}

	template<typename T>
	static CONSTEXPR FORCE_INLINE T min(const T a, const T b)
	{
		return a < b ? a : b;
	}
	/** @} */

	/**
	 * @brief Align up
	 * 
	 * @param [in]	n			number to align
	 * @param [in]	alignment	alignment to force
	 * 
	 * @return aligned result
	 */
	template<typename IntT, typename IntU>
	static CONSTEXPR FORCE_INLINE IntT alignUp(IntT n, IntU alignment)
	{
		// T must be an integer type
		static_assert(IsIntegral<IntT>::value, "Cannot align non-integral type");

		// Align up
		const IntU a = alignment - 1;
		return n & a ? (n | a) + 1 : n;
	}

	/**
	 * Get the index of the most significant `1` bit
	 * 
	 * @param [in] n upper bound
	 * @return index of msb
	 */
	template<typename IntT>
	static CONSTEXPR FORCE_INLINE uint32 getP2Index(IntT n, uint32 bits = sizeof(IntT) * 8)
	{
		if (bits == 1) return 0;

		uint32 hBits = bits >> 1;
		IntT upper = n >> hBits;
		IntT lower = n ^ (upper << hBits);

		return upper ? hBits + getP2Index(upper, hBits) : lower ? getP2Index(lower, hBits) : 0;
	}

	/**
	 * Get index of smallest power of two greater than
	 * the provided lower bound
	 * 
	 * @param [in] n lower bound
	 * @return next p2 index
	 */
	template<typename IntT>
	static CONSTEXPR FORCE_INLINE uint32 getNextP2Index(IntT n, uint32 bits = sizeof(IntT) * 8)
	{
		return getP2Index(n, bits) + 1;
	}

	/**
	 * Get smallest power of two greater than n
	 * 
	 * @param n lower bound
	 * @return next p2
	 */
	template<typename IntT>
	static CONSTEXPR FORCE_INLINE IntT getNextP2(IntT n)
	{
		return 1 << getNextP2Index(n);
	}

	/**
	 * Like shift, but outsiders are pushed on the other side
	 * 
	 * @param [in] n value to rotate
	 * @param [in] k shift range
	 * @{
	 */
	template<typename IntT>
	static CONSTEXPR FORCE_INLINE IntT shiftRotateRight(IntT n, uint32 k)
	{
		return (n >> k) | (n << (sizeof(IntT) * 8 - k));
	}

	template<typename IntT>
	static CONSTEXPR FORCE_INLINE IntT shiftRotateLeft(IntT n, uint32 k)
	{
		return (n << k) | (n >> (sizeof(IntT) * 8 - k));
	}
	/// @}
};

/// Float-32 specialization
template<>
FORCE_INLINE float32 GenericPlatformMath::abs(float32 s)
{
	return fabsf(s);
}
