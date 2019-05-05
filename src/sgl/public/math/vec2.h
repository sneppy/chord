#pragma once

#include "core_types.h"
#include "math/math_fwd.h"
#include "hal/platform_math.h"
#include "templates/is_void.h"
#include "templates/simd.h"

/**
 * A 2-components vector
 */
template<typename T = float32>
struct Vec2
{
public:
	union
	{
		/// Array data
		T buffer[2];

		struct
		{
			/// Vector components
			/// @{
			T x, y;
			/// @}
		};
	};

public:
	/// Default constructor
	CONSTEXPR FORCE_INLINE Vec2() : buffer{T(0), T(0)} {}

	/// Components constructor
	CONSTEXPR FORCE_INLINE Vec2(T _x, T _y) : buffer{_x, _y} {}

	/// Scalar constructor
	CONSTEXPR FORCE_INLINE Vec2(T s) : buffer{s, s} {}

	/// Convert from @ref Vec3
	template<bool bHasVectorIntrinsics>
	CONSTEXPR FORCE_INLINE Vec2(const Vec3<T, bHasVectorIntrinsics> & v) : buffer{v.x, v.y} {}

	/// Convert from @ref Vec4
	template<bool bHasVectorIntrinsics>
	CONSTEXPR FORCE_INLINE Vec2(const Vec4<T, bHasVectorIntrinsics> & v) : buffer{v.x, v.y} {}

	/// Array random-access operator
	FORCE_INLINE const T & operator[](uint8 i) const { return buffer[i]; }
	FORCE_INLINE T & operator[](uint8 i) { return buffer[i]; }

	/// Get vector squared size
	FORCE_INLINE T getSquaredSize() const { return x * x + y * y; }

	/// Get vector size
	FORCE_INLINE T getSize() const { return PlatformMath::sqrt(x * x + y * y); }

	/// Get normal vector
	FORCE_INLINE Vec2<T> getNormal() const
	{
		const T size = getSize();
		return Vec2<T>(x / size, y / size);
	}

	/// Normalize in place
	FORCE_INLINE Vec2<T> & normalize()
	{
		const T size = getSize();
		x /= size, y /= size;
		return *this;
	}

	/// Return inverse vector
	CONSTEXPR FORCE_INLINE Vec2<T> operator!()
	{
		return Vec2<T>(-x, -y);
	}

	/// Return true if vector is nearly zero
	/// Only float specialization
	FORCE_INLINE bool isNearlyZero() const;

	/**
	 * Vector-vector comparison operators
	 * 
	 * Strict comparison, all components must evaluate to true
	 * 
	 * @param [in] v vector operand
	 * @return comparison result as @c bool
	 * @{
	 */
	/// Compare with float tolerance
	FORCE_INLINE bool isEqual(const Vec2<T> & v) const;

	FORCE_INLINE bool operator==(const Vec2<T> & v) const
	{
		return x == v.x & y == v.y;
	}
	FORCE_INLINE bool operator!=(const Vec2<T> & v) const
	{
		return x != v.x | y != v.y;
	}
	FORCE_INLINE bool operator<(const Vec2<T> & v) const
	{
		return x < v.x & y < v.y;
	}
	FORCE_INLINE bool operator>(const Vec2<T> & v) const
	{
		return x > v.x & y > v.y;
	}
	FORCE_INLINE bool operator<=(const Vec2<T> & v) const
	{
		return x <= v.x & y <= v.y;
	}
	FORCE_INLINE bool operator>=(const Vec2<T> & v) const
	{
		return x >= v.x & y >= v.y;
	}
	/// @}

	/**
	 * Vector-vector compound assignment
	 * 
	 * @param [in] v vector operand
	 * @return self
	 * @{
	 */
	FORCE_INLINE Vec2<T> & operator+=(const Vec2<T> & v)
	{
		x += v.x, y += v.y;
		return *this;
	}
	FORCE_INLINE Vec2<T> & operator-=(const Vec2<T> & v)
	{
		x -= v.x, y -= v.y;
		return *this;
	}
	FORCE_INLINE Vec2<T> & operator*=(const Vec2<T> & v)
	{
		x *= v.x, y *= v.y;
		return *this;
	}
	FORCE_INLINE Vec2<T> & operator/=(const Vec2<T> & v)
	{
		x /= v.x, y /= v.y;
		return *this;
	}
	/// @}

	/**
	 * Vector-vector compound assignment
	 * 
	 * @param [in] v scalar operand
	 * @return self
	 * @{
	 */
	FORCE_INLINE Vec2<T> & operator+=(T s)
	{
		x += s, y += s;
		return *this;
	}
	FORCE_INLINE Vec2<T> & operator-=(T s)
	{
		x -= s, y -= s;
		return *this;
	}
	FORCE_INLINE Vec2<T> & operator*=(T s)
	{
		x *= s, y *= s;
		return *this;
	}
	FORCE_INLINE Vec2<T> & operator/=(T s)
	{
		x /= s, y /= s;
		return *this;
	}
	/// @}

	/**
	 * Vector-vector element-wise arithmetic operations
	 * 
	 * @param [in] v vector operand
	 * @return new vector
	 * @{
	 */
	CONSTEXPR FORCE_INLINE Vec2<T> operator+(const Vec2<T> & v) const
	{
		return Vec2<T>(x + v.x, y + v.y);
	}
	CONSTEXPR FORCE_INLINE Vec2<T> operator-(const Vec2<T> & v) const
	{
		return Vec2<T>(x - v.x, y - v.y);
	}
	CONSTEXPR FORCE_INLINE Vec2<T> operator*(const Vec2<T> & v) const
	{
		return Vec2<T>(x * v.x, y * v.y);
	}
	CONSTEXPR FORCE_INLINE Vec2<T> operator/(const Vec2<T> & v) const
	{
		return Vec2<T>(x / v.x, y / v.y);
	}
	/// @}

	/**
	 * Vector-scalar element-wise arithmetic operations
	 * 
	 * @param [in] s scalar operand
	 * @return new vector
	 * @{
	 */
	CONSTEXPR FORCE_INLINE Vec2<T> operator+(T s) const
	{
		return Vec2<T>(x + s, y + s);
	}
	CONSTEXPR FORCE_INLINE Vec2<T> operator-(T s) const
	{
		return Vec2<T>(x - s, y - s);
	}
	CONSTEXPR FORCE_INLINE Vec2<T> operator*(T s) const
	{
		return Vec2<T>(x * s, y * s);
	}
	CONSTEXPR FORCE_INLINE Vec2<T> operator/(T s) const
	{
		return Vec2<T>(x / s, y / s);
	}
	/// @}

	/// Dot product
	CONSTEXPR FORCE_INLINE T operator&(const Vec2<T> & v) const
	{
		return x * v.x + y * v.y;
	}

	/// Cross product
	/// @return @ref Vec3 orthogonal to the xy plane
	CONSTEXPR FORCE_INLINE Vec3<T, hasVectorIntrinsics(T, 4)> operator^(const Vec2<T> & v) const
	{
		return Vec3<T, hasVectorIntrinsics(T, 4)>(T(0), T(0), x * v.y - y * v.x);
	}

	/// Cross product
	/// @return z component of resulting orthogonal vector
	CONSTEXPR FORCE_INLINE T cross(const Vec2<T> & v) const
	{
		return x * v.y - y * v.x;
	}

	/// Convert to another underlying type
	template<typename U>
	FORCE_INLINE operator Vec2<U>() const
	{
		return Vec2<U>(U(x), U(y));
	}

	/// Print vector to stdout or file
	FORCE_INLINE void print(FILE * out = stdout) const;
};

template<>
FORCE_INLINE bool Vec2<float32>::isNearlyZero() const
{
	return PlatformMath::abs(x) <= FLT_EPSILON & PlatformMath::abs(y) <= FLT_EPSILON;
}

template<>
FORCE_INLINE bool Vec2<float32>::isEqual(const Vec2<float32> & v) const
{
	return PlatformMath::abs(x - v.x) <= FLT_EPSILON & PlatformMath::abs(y - v.y) <= FLT_EPSILON;
}
