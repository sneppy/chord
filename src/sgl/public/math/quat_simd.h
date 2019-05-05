#pragma once

#include "quat.h"

/**
 * Vector intrinsics specialization
 */
template<typename T>
struct Quat<T, true> : Vec4<T, true>
{
public:
	/// Vector intrinsics operations class
	using VecOps = typename Vec4<T, true>::VecOps;

	/// Vector intrinsics data type
	using VecT = typename Vec4<T, true>::VecT;

public:
	/// Default constructor
	CONSTEXPR FORCE_INLINE Quat() : Quat<T>(T(0), Vec3<T, true>::up) {};

	/// Convert @ref Vec4
	CONSTEXPR FORCE_INLINE Quat(const Vec4<T, true> & v) : Vec4<T, true>(v) {};

	/// Inherit all @ref Vec4 constructors
	using Vec4<T, true>::Vec4;

	/// Angle and axis constructor
	template<bool bHVI>
	CONSTEXPR FORCE_INLINE Quat(T angle, const Vec3<T, bHVI> & axis) :
		Vec4<T, true>(axis.getNormal() * PlatformMath::sin(angle / 2.f), PlatformMath::cos(angle / 2.f)) {}

	/// Returns angle and axis
	template<bool bHVI>
	FORCE_INLINE void getAngleAndAxis(T & angle, Vec3<T, bHVI> & axis) const
	{
		angle = PlatformMath::acos(this->w);
		axis = this->xyz / PlatformMath::sin(angle);
		angle *= 2.f;
	}

	/// Returns angle of rotation
	FORCE_INLINE T getAngle() const
	{
		return PlatformMath::acos(this->w) * 2.f;
	}

	/// Returns axis of rotation
	FORCE_INLINE Vec3<T, true> getAxis() const
	{
		return this->xyz / PlatformMath::sqrt(1 - this->w * this->w);
	}

	/// Override normalization methods to return quaternion
	/// @{
	FORCE_INLINE Quat<T> getNormal() const
	{
		const T size = this->getSize();
		return Quat<T>(VecOps::div(this->data, VecOps::load(size)));
	}
	/// In-place
	FORCE_INLINE Quat<T> & normalize()
	{
		const T size = this->getSize();
		this->data = VecOps::div(this->data, VecOps::load(size));
		return *this;
	}
	/// @}

	/// Get inverse quaternion
	FORCE_INLINE Quat<T> operator!() const
	{
		return Quat<T>(-this->x, -this->y, -this->z, this->w);
	}

	/**
	 * Quat-quat multiplication
	 * 
	 * Like matrix multiplication, it's not commutative
	 * 
	 * @param [in] q quat operand
	 * @return combined rotation quaternion
	 */
	CONSTEXPR FORCE_INLINE Quat<T> operator*(const Quat<T> & q) const
	{
		/// @ref https://en.wikipedia.org/wiki/Quaternion#Hamilton_product
		/// Wikipedia uses (angle, axis<i, j, k>) notation
		/// Here we use (axis<x, y, z>, angle) notation,
		/// thus everything is inverted
		return Quat<T, true>(
			VecOps::add(
				VecOps::sub(
					VecOps::mul(
						VecOps::template shuffle<3, 3, 3, 3>(this->data),
						q.data
					),
					VecOps::mul(
						VecOps::template shuffle<2, 0, 1, 0>(this->data),
						VecOps::template shuffle<1, 2, 0, 0>(this->data)
					)
				),
				VecOps::add(
					VecOps::bxor(
						VecT{0.f, 0.f, 0.f, -0.f},
						VecOps::mul(
							VecOps::template shuffle<1, 1, 0, 1>(this->data),
							VecOps::template shuffle<2, 3, 1, 1>(this->data)
						)
					),
					VecOps::bxor(
						VecT{0.f, 0.f, 0.f, -0.f},
						VecOps::mul(
							VecOps::template shuffle<0, 2, 2, 2>(this->data),
							VecOps::template shuffle<3, 0, 3, 2>(this->data)
						)
					)
				)
			)
		);
	}
	
	/**
	 * Quat-vector multiplication
	 * 
	 * @param [in] v vector operand
	 * @return rotated vector
	 * @{
	 */
	CONSTEXPR FORCE_INLINE Vec3<T, true> operator*(const Vec3<T, true> & v) const
	{
		/** @see http://people.csail.mit.edu/bkph/articles/Quaternions.pdf */
		const Vec3<T, true> q(this->data);
		const Vec3<T, true> t = 2.f * (q ^ v);
		return v + (this->w * t) + (q ^ t);
	}
	CONSTEXPR FORCE_INLINE Vec4<T, true> operator*(const Vec4<T, true> & v) const { return Vec4<T, true>(operator*(Vec3<T, true>(v)), v.w); }
	/// @}

	/// Direction vectors
	/// @{
	FORCE_INLINE Vec3<T, true> right() const	{ return operator*(Vec3<T, true>::right); };
	FORCE_INLINE Vec3<T, true> left() const		{ return operator*(Vec3<T, true>::left); };
	FORCE_INLINE Vec3<T, true> up() const		{ return operator*(Vec3<T, true>::up); };
	FORCE_INLINE Vec3<T, true> down() const		{ return operator*(Vec3<T, true>::down); };
	FORCE_INLINE Vec3<T, true> forward() const	{ return operator*(Vec3<T, true>::forward); };
	FORCE_INLINE Vec3<T, true> backward() const	{ return operator*(Vec3<T, true>::backward); };
	/// @}

	/// Override print method
	/// @see Vec4<T, true>::print()
	FORCE_INLINE void print(FILE * out = stdout) const;
};

template<>
FORCE_INLINE void Quat<float32, true>::print(FILE * out) const
{
	float32 angle;
	Vec3<float32, true> axis;
	getAngleAndAxis(angle, axis);

#define _DEG(x) x * 180.f / M_PI
	fprintf(out, "qf(%.1f deg @ <%.2f, %.2f, %.2f>)\n", _DEG(angle), axis.x, axis.y, axis.z);
#undef _DEG
}