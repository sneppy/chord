#pragma once

#include "core_types.h"
#include "hal/platform_math.h"
#include "math_fwd.h"
#include "templates/is_void.h"
#include "templates/simd.h"

/**
 * A 3-component vector
 * 
 * If possible uses vector intrinsics to
 * boost performance
 */
template<typename T = float32, bool = hasVectorIntrinsics(T, 4)>
struct Vec4
{
public:
	union
	{
		/// Data buffer
		T buffer[4];

		struct
		{
			/// Vector components
			/// @{
			T x, y, z, w;
			/// @}
		};

		struct
		{
			/// Color components
			/// @{
			T r, g, b, a;
			/// @}
		};

		struct
		{
			/// Quick access to the xyz @ref Vec3
			Vec3<T, false> xyz;
		};
	};

public:
	/// Default constructor, zero-initialize
	CONSTEXPR FORCE_INLINE Vec4() : buffer{T(0), T(0), T(0), T(0)} {}

	/// Components constructor
	CONSTEXPR FORCE_INLINE Vec4(T _x, T _y, T _z, T _w) : buffer{_x, _y, _z, _w} {}

	/// Scalar constructor
	CONSTEXPR FORCE_INLINE Vec4(T s) : buffer{s, s, s, s} {}

	/// Extend vec3
	template<bool bHasVectorIntrinsics>
	CONSTEXPR FORCE_INLINE Vec4(const Vec3<T, bHasVectorIntrinsics> & v, float32 w = 0.f) : buffer{v.x, v.y, v.z, w} {}

	/// Buffer-access operator
	/// @{
	FORCE_INLINE const T & operator[](uint8 i) const { return buffer[i]; }
	FORCE_INLINE T & operator[](uint8 i) { return buffer[i]; }
	/// @}

	/// Returns squared size of the vector
	FORCE_INLINE T getSquaredSize() const
	{
		return x * x + y * y + z * z + w * w;
	}

	/// Returns size of the vector
	FORCE_INLINE T getSize() const
	{
		return PlatformMath::sqrt(x * x + y * y + z * z + w * w);
	}

	/// Returns normal vector
	FORCE_INLINE Vec4<T, false> getNormal() const
	{
		const T size = PlatformMath::sqrt(x * x + y * y + z * z + w * w);
		return Vec4<T, false>(x / size, y / size, z / size, w / size);
	}

	/// Normalizes in place
	FORCE_INLINE Vec4<T, false> & normalize()
	{
		const T size = PlatformMath::sqrt(x * x + y * y + z * z + w * w);
		x /= size, y /= size, z /= size, w /= size;
		return *this;
	}

	//////////////////////////////////////////////////
	// Comparison operations
	//////////////////////////////////////////////////
	
	/// Returns true if components are almost zero
	/// This functions really has only meaning for floats
	FORCE_INLINE bool isNearlyZero() const;

	/// Returns true if vectors are almost equal
	/// @see isNearlyZero()
	FORCE_INLINE bool isEqual(const Vec4<T, false> & v) const;

	/**
	 * Vector-vector comparison operators
	 * 
	 * @param [in] v vector operand
	 * @returns result of comparison
	 * @{
	 */
	/// Use @ref isNearlyZero() for floating points
	FORCE_INLINE bool operator==(const Vec4<T, false> & v) const
	{
		return x == v.x & y == v.y & z == v.z & z == v.w;
	}
	FORCE_INLINE bool operator!=(const Vec4<T, false> & v) const
	{
		return x != v.x | y != v.y | z != v.z | z != v.w;
	}
	/// Strictly less, all components must be less
	FORCE_INLINE bool operator<(const Vec4<T, false> & v) const
	{
		return x < v.x & y < v.y & z < v.z & z < v.w;
	}
	/// Strictly greater, all components must be greater
	FORCE_INLINE bool operator>(const Vec4<T, false> & v) const
	{
		return x > v.x & y > v.y & z > v.z & z > v.w;
	}
	/// Weakly less
	FORCE_INLINE bool operator<=(const Vec4<T, false> & v) const
	{
		return x <= v.x & y <= v.y & z <= v.z & z <= v.w;
	}
	/// Weakly greater
	FORCE_INLINE bool operator>=(const Vec4<T, false> & v) const
	{
		return x >= v.x & y >= v.y & z >= v.z & z >= v.w;
	}
	/// @}

	//////////////////////////////////////////////////
	// Arithmetic operations
	//////////////////////////////////////////////////
	
	/// Invert vector direction
	CONSTEXPR FORCE_INLINE Vec4<T, false> operator-() const
	{
		return Vec4<T, false>(-x -y, -z, -w);
	}

	/**
	 * Vector-vector compound assignment operators
	 * 
	 * @param [in] v second operand
	 * @return self
	 * @{
	 */
	FORCE_INLINE Vec4<T, false> & operator+=(const Vec4<T, false> & v)
	{
		x += v.x, y += v.y, z += v.z, w += v.w;
		return *this;
	}
	FORCE_INLINE Vec4<T, false> & operator-=(const Vec4<T, false> & v)
	{
		x -= v.x, y -= v.y, z -= v.z, w -= v.w;
		return *this;
	}
	FORCE_INLINE Vec4<T, false> & operator*=(const Vec4<T, false> & v)
	{
		x *= v.x, y *= v.y, z *= v.z, w *= v.w;
		return *this;
	}
	FORCE_INLINE Vec4<T, false> & operator/=(const Vec4<T, false> & v)
	{
		x /= v.x, y /= v.y, z /= v.z, w /= v.w;
		return *this;
	}
	/// @}

	/**
	 * Vector-scalar compound assignments
	 * 
	 * @param [in] s scalar value
	 * @return self
	 * @{
	 */
	FORCE_INLINE Vec4<T, false> & operator+=(T s)
	{
		x += s, y += s, z += s, w += s;
		return *this;
	}
	FORCE_INLINE Vec4<T, false> & operator-=(T s)
	{
		x -= s, y -= s, z -= s, w -= s;
		return *this;
	}
	FORCE_INLINE Vec4<T, false> & operator*=(T s)
	{
		x *= s, y *= s, z *= s, w *= s;
		return *this;
	}
	FORCE_INLINE Vec4<T, false> & operator/=(T s)
	{
		x /= s, y /= s, z /= s, w /= s;
		return *this;
	}
	/// @}

	/**
	 * Vector-vector element-wise operations
	 * 
	 * @param [in] v vector operand
	 * @return new vector
	 * @{
	 */
	CONSTEXPR FORCE_INLINE Vec4<T, false> operator+(const Vec4<T, false> & v) const
	{
		return Vec4<T, false>(x + v.x, y + v.y, z + v.z, w + v.w);
	}
	CONSTEXPR FORCE_INLINE Vec4<T, false> operator-(const Vec4<T, false> & v) const
	{
		return Vec4<T, false>(x - v.x, y - v.y, z - v.z, w - v.w);
	}
	CONSTEXPR FORCE_INLINE Vec4<T, false> operator*(const Vec4<T, false> & v) const
	{
		return Vec4<T, false>(x * v.x, y * v.y, z * v.z, w * v.w);
	}
	CONSTEXPR FORCE_INLINE Vec4<T, false> operator/(const Vec4<T, false> & v) const
	{
		return Vec4<T, false>(x / v.x, y / v.y, z / v.z, w / v.w);
	}
	/// @}

	/**
	 * Vector-scalar operations
	 * 
	 * @param [in] s scalar operand
	 * @return new vector
	 * @{
	 */
	CONSTEXPR FORCE_INLINE Vec4<T, false> operator+(T s) const
	{
		return Vec4<T, false>(x + s, y + s, z + s, w + s);
	}
	CONSTEXPR FORCE_INLINE Vec4<T, false> operator-(T s) const
	{
		return Vec4<T, false>(x - s, y - s, z - s, w - s);
	}
	CONSTEXPR FORCE_INLINE Vec4<T, false> operator*(T s) const
	{
		return Vec4<T, false>(x * s, y * s, z * s, w * s);
	}
	CONSTEXPR FORCE_INLINE Vec4<T, false> operator/(T s) const
	{
		return Vec4<T, false>(x / s, y / s, z / s, w / s);
	}
	/// @}
	
	/**
	 * Vector dot (scalar) product
	 * 
	 * @param [in] v vector operand
	 * @return dot product as T
	 */
	CONSTEXPR FORCE_INLINE T operator&(const Vec4<T, false> & v) const
	{
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}

	/// Convert to another underlying type
	template<typename U, bool bHasVectorIntrinsics>
	FORCE_INLINE operator Vec4<U, bHasVectorIntrinsics>() const
	{
		return Vec4<U, bHasVectorIntrinsics>(U(x), U(y), U(z), U(w));
	}

	/// Print vector to stdout or to specified file
	void print(FILE * out = stdout) const;
};

#if PLATFORM_ENABLE_SIMD
	#include "vec4_simd.h"
#endif

template<>
FORCE_INLINE bool Vec4<float32, false>::isNearlyZero() const
{
	return PlatformMath::abs(x) <= FLT_EPSILON
		& PlatformMath::abs(y) <= FLT_EPSILON
		& PlatformMath::abs(z) <= FLT_EPSILON
		& PlatformMath::abs(w) <= FLT_EPSILON;
}

template<>
FORCE_INLINE bool Vec4<float32, false>::isEqual(const Vec4<float32, false> & v) const
{
	return PlatformMath::abs(x - v.x) <= FLT_EPSILON
		& PlatformMath::abs(y - v.y) <= FLT_EPSILON
		& PlatformMath::abs(z - v.z) <= FLT_EPSILON
		& PlatformMath::abs(w - v.w) <= FLT_EPSILON;
}

template<>
FORCE_INLINE void Vec4<float32, false>::print(FILE * out) const
{
	fprintf(out, "v4f(%.3f, %.3f, %.3f, %.3f)\n", x, y, z, w);
}

template<>
FORCE_INLINE void Vec4<int32, false>::print(FILE * out) const
{
	fprintf(out, "v4i(%d, %d, %d, %d)\n", x, y, z, w);
}

template<>
FORCE_INLINE void Vec4<uint32, false>::print(FILE * out) const
{
	fprintf(out, "v4u(%u, %u, %u, %u)\n", x, y, z, w);
}

/**
 * Scalar-vector operations
 * 
 * @param [in] s,v scalar and vector operands
 * @return new vector
 * @{
 */
template<typename T, bool bHVI>
FORCE_INLINE Vec4<T, bHVI> operator+(T s, const Vec4<T, bHVI> & v)
{
	return v + s;
}
template<typename T, bool bHVI>
FORCE_INLINE Vec4<T, bHVI> operator-(T s, const Vec4<T, bHVI> & v)
{
	return Vec4<T, bHVI>(s) - v;
}
template<typename T, bool bHVI>
FORCE_INLINE Vec4<T, bHVI> operator*(T s, const Vec4<T, bHVI> & v)
{
	return v * s;
}
template<typename T, bool bHVI>
FORCE_INLINE Vec4<T, bHVI> operator/(T s, const Vec4<T, bHVI> & v)
{
	return Vec4<T, bHVI>(s) / v;
}
/// @}
