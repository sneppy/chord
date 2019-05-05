#pragma once

#include "vec3.h"

/**
 * Vector intrinsics specialization
 */
template<typename T>
struct GCC_ALIGN(16) Vec3<T, true>
{
public:
	/// Constant vectors
	/// @{
	static const Vec3<T, true> zero;
	static const Vec3<T, true> unit;
	static const Vec3<T, true> right;
	static const Vec3<T, true> left;
	static const Vec3<T, true> up;
	static const Vec3<T, true> down;
	static const Vec3<T, true> forward;
	static const Vec3<T, true> backward;
	/// @}

public:
	/// Vector operations class
	using VecOps = Simd::Vector<T, 4>;

	/// Intrinsic type
	using VecT = typename VecOps::Type;

	union
	{
		/// Intrinsic vector
		VecT data;

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

	/// Intrinsic constructor
	CONSTEXPR FORCE_INLINE Vec3(VecT _data) : data(_data) {}

	/// Components constructor
	CONSTEXPR FORCE_INLINE Vec3(T _x, T _y, T _z) : buffer{_x, _y, _z} {}

	/// Scalar constructor
	CONSTEXPR FORCE_INLINE Vec3(T s) : buffer{s, s, s} {}

	/// Convert non-intrinsics vector
	CONSTEXPR FORCE_INLINE Vec3(const Vec3<T, false> & v) : buffer{v.x, v.y, v.z} {}

	/// Convert @ref Vec2
	CONSTEXPR FORCE_INLINE Vec3(const Vec2<T> & v, T z = T(0)) : buffer{v.x, v.y, z} {}

	/// Convert @ref Vec4
	/// @{
	CONSTEXPR FORCE_INLINE Vec3(const Vec4<T, true> & v) : data(v.data) {}
	CONSTEXPR FORCE_INLINE Vec3(const Vec4<T, false> & v) : buffer{v.x, v.y, v.z} {}
	/// @}

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
	FORCE_INLINE Vec3<T> getNormal() const
	{
		return Vec3<T>(VecOps::div(data, VecOps::load(PlatformMath::sqrt(x * x + y * y + z * z))));
	}

	/// Normalizes in place
	FORCE_INLINE Vec3<T> & normalize()
	{
		data = VecOps::div(data, VecOps::load(PlatformMath::sqrt(x * x + y * y + z * z)));
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
	FORCE_INLINE bool isEqual(const Vec3<T> & v) const;

	/**
	 * Vector-vector comparison operators
	 * 
	 * @param [in] v vector operand
	 * @returns result of comparison
	 * @{
	 */
	/// Use @ref isNearlyZero() for floating points
	FORCE_INLINE bool operator==(const Vec3<T> & v) const
	{
		return x == v.x & y == v.y & z == v.z;
	}
	FORCE_INLINE bool operator!=(const Vec3<T> & v) const
	{
		return x != v.x | y != v.y | z != v.z;
	}
	/// Strictly less, all components must be less
	FORCE_INLINE bool operator<(const Vec3<T> & v) const
	{
		return x < v.x & y < v.y & z < v.z;
	}
	/// Strictly greater, all components must be greater
	FORCE_INLINE bool operator>(const Vec3<T> & v) const
	{
		return x > v.x & y > v.y & z > v.z;
	}
	/// Weakly less
	FORCE_INLINE bool operator<=(const Vec3<T> & v) const
	{
		return x <= v.x & y <= v.y & z <= v.z;
	}
	/// Weakly greater
	FORCE_INLINE bool operator>=(const Vec3<T> & v) const
	{
		return x >= v.x & y >= v.y & z >= v.z;
	}
	/// @}

	//////////////////////////////////////////////////
	// Arithmetic operations
	//////////////////////////////////////////////////
	
	/// Invert vector direction
	FORCE_INLINE Vec3<T> operator-() const
	{
		return Vec3<T>(-x, -y, -z);
	}

	/**
	 * Vector-vector compound assignment operators
	 * 
	 * @param [in] v second operand
	 * @return self
	 * @{
	 */
	FORCE_INLINE Vec3<T> & operator+=(const Vec3<T> & v)
	{
		data = VecOps::add(data, v.data);
		return *this;
	}
	FORCE_INLINE Vec3<T> & operator-=(const Vec3<T> & v)
	{
		data = VecOps::sub(data, v.data);
		return *this;
	}
	FORCE_INLINE Vec3<T> & operator*=(const Vec3<T> & v)
	{
		data = VecOps::mul(data, v.data);
		return *this;
	}
	FORCE_INLINE Vec3<T> & operator/=(const Vec3<T> & v)
	{
		data = VecOps::div(data, v.data);
		return *this;
	}
	/// @}

	/**
	 * Vector-scalar compounf assignment
	 * 
	 * @param [in] s scalar value
	 * @return self
	 * @{
	 */
	FORCE_INLINE Vec3<T> & operator+=(T s)
	{
		data = VecOps::add(data, VecOps::load(s));
		return *this;
	}
	FORCE_INLINE Vec3<T> & operator-=(T s)
	{
		data = VecOps::sub(data, VecOps::load(s));
		return *this;
	}
	FORCE_INLINE Vec3<T> & operator*=(T s)
	{
		data = VecOps::mul(data, VecOps::load(s));
		return *this;
	}
	FORCE_INLINE Vec3<T> & operator/=(T s)
	{
		data = VecOps::div(data, VecOps::load(s));
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
	FORCE_INLINE Vec3<T> operator+(const Vec3<T> & v) const
	{
		return Vec3<T>(VecOps::add(data, v.data));
	}
	FORCE_INLINE Vec3<T> operator-(const Vec3<T> & v) const
	{
		return Vec3<T>(VecOps::sub(data, v.data));
	}
	FORCE_INLINE Vec3<T> operator*(const Vec3<T> & v) const
	{
		return Vec3<T>(VecOps::mul(data, v.data));
	}
	FORCE_INLINE Vec3<T> operator/(const Vec3<T> & v) const
	{
		return Vec3<T>(VecOps::div(data, v.data));
	}
	/// @}

	/**
	 * Vector-scalar operations
	 * 
	 * @param [in] s scalar operand
	 * @return new vector
	 * @{
	 */
	FORCE_INLINE Vec3<T> operator+(T s) const
	{
		return Vec3<T>(VecOps::add(data, VecOps::load(s)));
	}
	FORCE_INLINE Vec3<T> operator-(T s) const
	{
		return Vec3<T>(VecOps::sub(data, VecOps::load(s)));
	}
	FORCE_INLINE Vec3<T> operator*(T s) const
	{
		return Vec3<T>(VecOps::mul(data, VecOps::load(s)));
	}
	FORCE_INLINE Vec3<T> operator/(T s) const
	{
		return Vec3<T>(VecOps::div(data, VecOps::load(s)));
	}
	/// @}
	
	/**
	 * Vector dot (scalar) product
	 * 
	 * @param [in] v vector operand
	 * @return dot product as T
	 */
	FORCE_INLINE T operator&(const Vec3<T> & v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}
	
	/**
	 * Vector cross product
	 * 
	 * @param [in] v vector operand
	 * @return vector orthogonal to the (v1 x v2) plane
	 */
	FORCE_INLINE Vec3<T> operator^(const Vec3<T> & v) const
	{
		// This may be faster alone,
		// but with everything vectorized is better like this
		//return Vec3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);

		return Vec3<T>(
			VecOps::sub(
				VecOps::mul(
					VecOps::template shuffle<1, 2, 0, 3>(data),
					VecOps::template shuffle<2, 0, 1, 3>(v.data)
				),
				VecOps::mul(
					VecOps::template shuffle<2, 0, 1, 3>(data),
					VecOps::template shuffle<1, 2, 0, 3>(v.data)
				)
			)
		);
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

/// Vector-scalar operations optimized for float
template<>
FORCE_INLINE Vec3<float32, true> Vec3<float32, true>::operator+(float32 s) const
{
	return Vec3<float32, true>(VecOps::add(data, VecT{s, s, s, s}));
}
template<>
FORCE_INLINE Vec3<float32, true> Vec3<float32, true>::operator-(float32 s) const
{
	return Vec3<float32, true>(VecOps::sub(data, VecT{s, s, s, s}));
}
template<>
FORCE_INLINE Vec3<float32, true> Vec3<float32, true>::operator*(float32 s) const
{
	return Vec3<float32, true>(VecOps::mul(data, VecT{s, s, s, s}));
}
template<>
FORCE_INLINE Vec3<float32, true> Vec3<float32, true>::operator/(float32 s) const
{
	return Vec3<float32, true>(VecOps::div(data, VecT{s, s, s, s}));
}
/// @}

template<>
FORCE_INLINE bool Vec3<float32, true>::isNearlyZero() const
{
	return (VecOps::cmp<Simd::CMP_GE>(VecOps::bor(data, VecOps::neg), VecOps::load(-FLT_EPSILON)) & 0x7) == 0x7;
}

template<>
FORCE_INLINE bool Vec3<float32, true>::isEqual(const Vec3<float32> & v) const
{
	return (VecOps::cmp<Simd::CMP_GE>(VecOps::bor(VecOps::sub(data, v.data), VecOps::neg), VecOps::load(-FLT_EPSILON)) & 0x7) == 0x7;
}

template<>
FORCE_INLINE void Vec3<float32, true>::print(FILE * out) const
{
	fprintf(out, "v3f(%.3f, %.3f, %.3f)\n", x, y, z);
}