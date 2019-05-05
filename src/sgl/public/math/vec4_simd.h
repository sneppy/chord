#pragma once

#include "vec4.h"

/**
 * Vector intrinsics specialization
 */
template<typename T>
struct GCC_ALIGN(16) Vec4<T, true>
{
public:
	/// Vector operations class
	using VecOps = Simd::Vector<T, 4>;

	/// Intrinsic type
	using VecT = typename VecOps::Type;

	union
	{
		/// Intrinsic data
		VecT data;

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
			Vec3<T, true> xyz;
		};
	};

public:
	/// Default constructor, zero-initialize
	CONSTEXPR FORCE_INLINE Vec4() : buffer{T(0), T(0), T(0), T(0)} {}

	/// Intrinsic constructor
	CONSTEXPR FORCE_INLINE Vec4(VecT _data) : data(_data) {}

	/// Components constructor
	CONSTEXPR FORCE_INLINE Vec4(T _x, T _y, T _z, T _w) : buffer{_x, _y, _z, _w} {}

	/// Scalar constructor
	CONSTEXPR FORCE_INLINE Vec4(T s) : buffer{s, s, s, s} {}

	/// Convert non-intrinsic vector
	CONSTEXPR FORCE_INLINE Vec4(const Vec4<T, false> & v) : buffer{v.x, v.y, v.z, v.w} {}

	/// Convert @ref Vec3
	template<bool bHasVectorIntrinsics>
	CONSTEXPR FORCE_INLINE Vec4(const Vec3<T, bHasVectorIntrinsics> & v, float w = 0.f) : buffer{v.x, v.y, v.z, w} {}

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
	FORCE_INLINE Vec4<T> getNormal() const
	{
		return Vec4<T>(VecOps::div(data, VecOps::load(getSize())));
	}

	/// Normalizes in place
	FORCE_INLINE Vec4<T> & normalize()
	{
		data = VecOps::div(data, VecOps::load(getSize()));
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
	FORCE_INLINE bool isEqual(const Vec4<T> & v) const;

	/**
	 * Vector-vector comparison operators
	 * 
	 * @param [in] v vector operand
	 * @returns result of comparison
	 * @{
	 */
	/// Use @ref isNearlyZero() for floating points
	FORCE_INLINE bool operator==(const Vec4<T> & v) const
	{
		return VecOps::template cmp<Simd::CMP_EQ>(data, v.data) == 0xf;
	}
	FORCE_INLINE bool operator!=(const Vec4<T> & v) const
	{
		return VecOps::template cmp<Simd::CMP_NE>(data, v.data) != 0x0;
	}
	/// Strictly less, all components must be less
	FORCE_INLINE bool operator<(const Vec4<T> & v) const
	{
		return VecOps::template cmp<Simd::CMP_LT>(data, v.data) == 0xf;
	}
	/// Strictly greater, all components must be greater
	FORCE_INLINE bool operator>(const Vec4<T> & v) const
	{
		return VecOps::template cmp<Simd::CMP_GT>(data, v.data) == 0xf;
	}
	/// Weakly less
	FORCE_INLINE bool operator<=(const Vec4<T> & v) const
	{
		return VecOps::template cmp<Simd::CMP_LE>(data, v.data) == 0xf;
	}
	/// Weakly greater
	FORCE_INLINE bool operator>=(const Vec4<T> & v) const
	{
		return VecOps::template cmp<Simd::CMP_GE>(data, v.data) == 0xf;
	}
	/// @}

	//////////////////////////////////////////////////
	// Arithmetic operations
	//////////////////////////////////////////////////
	
	/// Invert vector direction
	FORCE_INLINE Vec4<T> operator-() const
	{
		return Vec4<T>(VecOps::bxor(data, VecOps::neg));
	}

	/**
	 * Vector-vector compound assignment operators
	 * 
	 * @param [in] v second operand
	 * @return self
	 * @{
	 */
	FORCE_INLINE Vec4<T> & operator+=(const Vec4<T> & v)
	{
		data = VecOps::sub(data, v.data);
		return *this;
	}
	FORCE_INLINE Vec4<T> & operator-=(const Vec4<T> & v)
	{
		data = VecOps::sub(data, v.data);
		return *this;
	}
	FORCE_INLINE Vec4<T> & operator*=(const Vec4<T> & v)
	{
		data = VecOps::mul(data, v.data);
		return *this;
	}
	FORCE_INLINE Vec4<T> & operator/=(const Vec4<T> & v)
	{
		data = VecOps::div(data, v.data);
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
	FORCE_INLINE Vec4<T> & operator+=(T s)
	{
		data = VecOps::add(data, VecOps::load(s));
		return *this;
	}
	FORCE_INLINE Vec4<T> & operator-=(T s)
	{
		data = VecOps::sub(data, VecOps::load(s));
		return *this;
	}
	FORCE_INLINE Vec4<T> & operator*=(T s)
	{
		data = VecOps::mul(data, VecOps::load(s));
		return *this;
	}
	FORCE_INLINE Vec4<T> & operator/=(T s)
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
	FORCE_INLINE Vec4<T> operator+(const Vec4<T> & v) const
	{
		return Vec4<T>(VecOps::add(data, v.data));
	}
	FORCE_INLINE Vec4<T> operator-(const Vec4<T> & v) const
	{
		return Vec4<T>(VecOps::sub(data, v.data));
	}
	FORCE_INLINE Vec4<T> operator*(const Vec4<T> & v) const
	{
		return Vec4<T>(VecOps::mul(data, v.data));
	}
	FORCE_INLINE Vec4<T> operator/(const Vec4<T> & v) const
	{
		return Vec4<T>(VecOps::div(data, v.data));
	}
	/// @}

	/**
	 * Vector-scalar operations
	 * 
	 * @param [in] s scalar operand
	 * @return new vector
	 * @{
	 */
	FORCE_INLINE Vec4<T> operator+(T s) const
	{
		return Vec4<T>(VecOps::add(data, VecOps::load(s)));
	}
	FORCE_INLINE Vec4<T> operator-(T s) const
	{
		return Vec4<T>(VecOps::sub(data, VecOps::load(s)));
	}
	FORCE_INLINE Vec4<T> operator*(T s) const
	{
		return Vec4<T>(VecOps::mul(data, VecOps::load(s)));
	}
	FORCE_INLINE Vec4<T> operator/(T s) const
	{
		return Vec4<T>(VecOps::div(data, VecOps::load(s)));
	}
	/// @}
	
	/**
	 * Vector dot (scalar) product
	 * 
	 * @param [in] v vector operand
	 * @return dot product as T
	 */
	FORCE_INLINE T operator&(const Vec4<T> & v) const
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

template<>
FORCE_INLINE bool Vec4<float32, true>::isNearlyZero() const
{
	return VecOps::template cmp<Simd::CMP_GE>(VecOps::bor(data, VecOps::neg), VecT{-FLT_EPSILON, -FLT_EPSILON, -FLT_EPSILON, -FLT_EPSILON}) == 0xf;
}

template<>
FORCE_INLINE bool Vec4<float32, true>::isEqual(const Vec4<float32> & v) const
{
	return VecOps::template cmp<Simd::CMP_GE>(VecOps::bor(VecOps::sub(data, v.data), VecOps::neg), VecT{-FLT_EPSILON, -FLT_EPSILON, -FLT_EPSILON, -FLT_EPSILON}) == 0xf;
}

template<>
FORCE_INLINE void Vec4<float32, true>::print(FILE * out) const
{
	fprintf(out, "v4f(%.3f, %.3f, %.3f, %.3f)\n", x, y, z, w);
}