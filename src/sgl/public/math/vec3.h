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
struct Vec3
{
public:
	/// Constant vectors
	/// @{
	static const Vec3<T, false> zero;
	static const Vec3<T, false> unit;
	static const Vec3<T, false> right;
	static const Vec3<T, false> left;
	static const Vec3<T, false> up;
	static const Vec3<T, false> down;
	static const Vec3<T, false> forward;
	static const Vec3<T, false> backward;
	/// @}

public:
	union
	{
		/// Data buffer
		T buffer[3];

		struct
		{
			/// Vector components
			/// @{
			T x, y, z;
			/// @}
		};

		struct
		{
			/// Color components
			/// @{
			T r, g, b;
			/// @}
		};
	};

public:
	/// Default constructor, zero-initialize
	CONSTEXPR FORCE_INLINE Vec3() : buffer{T(0), T(0), T(0)} {}

	/// Components constructor
	CONSTEXPR FORCE_INLINE Vec3(T _x, T _y, T _z) : buffer{_x, _y, _z} {}

	/// Scalar constructor
	CONSTEXPR FORCE_INLINE Vec3(T s) : buffer{s, s, s} {}

	/// Convert @ref vec2
	CONSTEXPR FORCE_INLINE Vec3(const Vec2<T> & v, T z = T(0)) : buffer{v.x, v.y, z} {}

	/// Convert @ref Vec4
	template<bool bHasVectorIntrinsics>
	CONSTEXPR FORCE_INLINE Vec3(const Vec4<T, bHasVectorIntrinsics> & v) : buffer{v.x, v.y, v.z} {}

	/// Buffer-access operator
	/// @{
	FORCE_INLINE const T & operator[](uint8 i) const { return buffer[i]; }
	FORCE_INLINE T & operator[](uint8 i) { return buffer[i]; }
	/// @}

	/// Returns squared size of the vector
	FORCE_INLINE T getSquaredSize() const
	{
		return x * x + y * y + z * z;
	}

	/// Returns size of the vector
	FORCE_INLINE T getSize() const
	{
		return PlatformMath::sqrt(x * x + y * y + z * z);
	}

	/// Returns normal vector
	FORCE_INLINE Vec3<T, false> getNormal() const
	{
		const T size = PlatformMath::sqrt(x * x + y * y + z * z);
		return Vec3<T, false>(x / size, y / size, z / size);
	}

	/// Normalizes in place
	FORCE_INLINE Vec3<T, false> & normalize()
	{
		const T size = PlatformMath::sqrt(x * x + y * y + z * z);
		x /= size, y /= size, z /= size;
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
	FORCE_INLINE bool isEqual(const Vec3<T, false> & v) const;

	/**
	 * Vector-vector comparison operators
	 * 
	 * @param [in] v vector operand
	 * @returns result of comparison
	 * @{
	 */
	/// Use @ref isNearlyZero() for floating points
	FORCE_INLINE bool operator==(const Vec3<T, false> & v) const
	{
		return x == v.x & y == v.y & z == v.z;
	}
	FORCE_INLINE bool operator!=(const Vec3<T, false> & v) const
	{
		return x != v.x | y != v.y | z != v.z;
	}
	/// Strictly less, all components must be less
	FORCE_INLINE bool operator<(const Vec3<T, false> & v) const
	{
		return x < v.x & y < v.y & z < v.z;
	}
	/// Strictly greater, all components must be greater
	FORCE_INLINE bool operator>(const Vec3<T, false> & v) const
	{
		return x > v.x & y > v.y & z > v.z;
	}
	/// Weakly less
	FORCE_INLINE bool operator<=(const Vec3<T, false> & v) const
	{
		return x <= v.x & y <= v.y & z <= v.z;
	}
	/// Weakly greater
	FORCE_INLINE bool operator>=(const Vec3<T, false> & v) const
	{
		return x >= v.x & y >= v.y & z >= v.z;
	}
	/// @}

	//////////////////////////////////////////////////
	// Arithmetic operations
	//////////////////////////////////////////////////
	
	/// Invert vector direction
	FORCE_INLINE Vec3<T, false> operator-() const
	{
		return Vec3<T, false>(-x, -y, -z);
	}

	/**
	 * Vector-vector compound assignment operators
	 * 
	 * @param [in] v second operand
	 * @return self
	 * @{
	 */
	FORCE_INLINE Vec3<T, false> & operator+=(const Vec3<T, false> & v)
	{
		x += v.x, y += v.y, z += v.z;
		return *this;
	}
	FORCE_INLINE Vec3<T, false> & operator-=(const Vec3<T, false> & v)
	{
		x -= v.x, y -= v.y, z -= v.z;
		return *this;
	}
	FORCE_INLINE Vec3<T, false> & operator*=(const Vec3<T, false> & v)
	{
		x *= v.x, y *= v.y, z *= v.z;
		return *this;
	}
	FORCE_INLINE Vec3<T, false> & operator/=(const Vec3<T, false> & v)
	{
		x /= v.x, y /= v.y, z /= v.z;
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
	FORCE_INLINE Vec3<T, false> & operator+=(T s)
	{
		x += s, y += s, z += s;
		return *this;
	}
	FORCE_INLINE Vec3<T, false> & operator-=(T s)
	{
		x -= s, y -= s, z -= s;
		return *this;
	}
	FORCE_INLINE Vec3<T, false> & operator*=(T s)
	{
		x *= s, y *= s, z *= s;
		return *this;
	}
	FORCE_INLINE Vec3<T, false> & operator/=(T s)
	{
		x /= s, y /= s, z /= s;
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
	CONSTEXPR FORCE_INLINE Vec3<T, false> operator+(const Vec3<T, false> & v) const
	{
		return Vec3<T, false>(x + v.x, y + v.y, z + v.z);
	}
	CONSTEXPR FORCE_INLINE Vec3<T, false> operator-(const Vec3<T, false> & v) const
	{
		return Vec3<T, false>(x - v.x, y - v.y, z - v.z);
	}
	CONSTEXPR FORCE_INLINE Vec3<T, false> operator*(const Vec3<T, false> & v) const
	{
		return Vec3<T, false>(x * v.x, y * v.y, z * v.z);
	}
	CONSTEXPR FORCE_INLINE Vec3<T, false> operator/(const Vec3<T, false> & v) const
	{
		return Vec3<T, false>(x / v.x, y / v.y, z / v.z);
	}
	/// @}

	/**
	 * Vector-scalar operations
	 * 
	 * @param [in] s scalar operand
	 * @return new vector
	 * @{
	 */
	CONSTEXPR FORCE_INLINE Vec3<T, false> operator+(T s) const
	{
		return Vec3<T, false>(x + s, y + s, z + s);
	}
	CONSTEXPR FORCE_INLINE Vec3<T, false> operator-(T s) const
	{
		return Vec3<T, false>(x - s, y - s, z - s);
	}
	CONSTEXPR FORCE_INLINE Vec3<T, false> operator*(T s) const
	{
		return Vec3<T, false>(x * s, y * s, z * s);
	}
	CONSTEXPR FORCE_INLINE Vec3<T, false> operator/(T s) const
	{
		return Vec3<T, false>(x / s, y / s, z / s);
	}
	/// @}
	
	/**
	 * Vector dot (scalar) product
	 * 
	 * @param [in] v vector operand
	 * @return dot product as T
	 */
	CONSTEXPR FORCE_INLINE Vec3<T, false> operator&(const Vec3<T, false> & v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}
	
	/**
	 * Vector cross product
	 * 
	 * @param [in] v vector operand
	 * @return vector orthogonal to the (v1 x v2) plane
	 */
	CONSTEXPR FORCE_INLINE Vec3<T, false> operator^(const Vec3<T, false> & v) const
	{
		return Vec3<T, false>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

	/// Convert to another underlying type
	template<typename U, bool bHasVectorIntrinsics>
	FORCE_INLINE operator Vec3<U, bHasVectorIntrinsics>() const
	{
		return Vec3<U, bHasVectorIntrinsics>(U(x), U(y), U(z));
	}

	/// Print vector to stdout or to specified file
	void print(FILE * out = stdout) const;
};

#if PLATFORM_ENABLE_SIMD
	#include "vec3_simd.h"
#endif


template<>
FORCE_INLINE bool Vec3<float32, false>::isNearlyZero() const
{
	return PlatformMath::abs(x) <= FLT_EPSILON
		& PlatformMath::abs(y) <= FLT_EPSILON
		& PlatformMath::abs(z) <= FLT_EPSILON;
}

template<>
FORCE_INLINE bool Vec3<float32, false>::isEqual(const Vec3<float32, false> & v) const
{
	return PlatformMath::abs(x - v.x) <= FLT_EPSILON
		& PlatformMath::abs(y - v.y) <= FLT_EPSILON
		& PlatformMath::abs(z - v.z) <= FLT_EPSILON;
}

template<>
FORCE_INLINE void Vec3<float32, false>::print(FILE * out) const
{
	fprintf(out, "v3f(%.3f, %.3f, %.3f)\n", x, y, z);
}

template<>
FORCE_INLINE void Vec3<int32, false>::print(FILE * out) const
{
	fprintf(out, "v3i(%d, %d, %d)\n", x, y, z);
}

template<>
FORCE_INLINE void Vec3<uint32, false>::print(FILE * out) const
{
	fprintf(out, "v3u(%u, %u, %u)\n", x, y, z);
}

/**
 * Scalar-vector operations
 * 
 * @param [in] s,v scalar and vector operands
 * @return new vector
 * @{
 */
template<typename T, bool bHVI>
FORCE_INLINE Vec3<T, bHVI> operator+(T s, const Vec3<T, bHVI> & v)
{
	return v + s;
}
template<typename T, bool bHVI>
FORCE_INLINE Vec3<T, bHVI> operator-(T s, const Vec3<T, bHVI> & v)
{
	return Vec3<T, bHVI>(s) - v;
}
template<typename T, bool bHVI>
FORCE_INLINE Vec3<T, bHVI> operator*(T s, const Vec3<T, bHVI> & v)
{
	return v * s;
}
template<typename T, bool bHVI>
FORCE_INLINE Vec3<T, bHVI> operator/(T s, const Vec3<T, bHVI> & v)
{
	return Vec3<T, bHVI>(s) / v;
}
/// @}
