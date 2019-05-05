#pragma once

#include "vec4.h"

/**
 * @struct Quat quat.h
 * @brief Quaternion implementation (used for rotation)
 * based on Vec4 implementation
 */
template<typename T>
struct Quat : public Vec4<T>
{
public:
	/**
	 * @brief Default constructor
	 */
	inline Quat();

	/**
	 * @brief Inherited constructors
	 */
	using Vec4<T>::Vec4;

	/**
	 * @brief Rotation-constructor
	 * 
	 * @param [in]	v	rotation axis
	 * @param [in]	a	rotation angle
	 */
	inline Quat(const Vec3<T> & v, const T a);

	/**
	 * @brief Get rotation angle
	 * 
	 * @return rotation angle
	 */
	inline T getAngle() const;

	/**
	 * @brief Get rotation axis
	 * 
	 * @return rotation axis
	 */
	inline Vec3<T> getAxis() const;

	/**
	 * @brief Compare two vectors
	 * 
	 * @param [in] v other vector
	 * 
	 * @return Comparison result
	 * @{
	 */
	inline bool operator==(const Quat<T> & v) const;
	inline bool operator!=(const Quat<T> & v) const;
	/** @} */

	/**
	 * @brief Invert quaternion (rotation)
	 * 
	 * @return inverted rotation
	 */
	inline Quat<T> operator!() const;

	/**
	 * @brief Invert quaternion (rotation) in-place
	 * 
	 * @return self
	 */
	inline Quat<T> & invert();

	/**
	 * @brief Quat-quat multiplication
	 * 
	 * @param [in] q other quaternion
	 * 
	 * @return combined quaternion
	 */
	inline Quat operator*(const Quat<T> & q) const;

	/**
	 * @brief Quat-vector multiplication (rotates vector)
	 * 
	 * @param [in] v vector
	 * 
	 * @return rotated vector
	 */
	inline Vec3<T> operator*(const Vec3<T> & v) const;

	/**
	 * @brief Direction-vector conversions
	 * 
	 * @return rotated direction vector
	 * @{
	 */
	inline Vec3<T> forward() const;
	inline Vec3<T> backward() const;
	inline Vec3<T> down() const;
	inline Vec3<T> up() const;
	inline Vec3<T> left() const;
	inline Vec3<T> right() const;
	/** @} */

	/**
	 * @brief Get angular distance between two quaternions
	 * 
	 * @param [in] q other quaternion
	 * 
	 * @return min. angular distance
	 * @{
	 */
	inline T operator^(const Quat<T> & q) const;
	/** @} */

	/**
	 * @copydoc Vec4<T>::print()
	 */
	inline void print(FILE * stream = stdout) const;
};

template<typename T>
Quat<T>::Quat() : Vec4<T>() {}

template<typename T>
T Quat<T>::getAngle() const
{
	return T(2) * acosf(this->w);
}

template<typename T>
Vec3<T> Quat<T>::getAxis() const
{
	const T s = sqrtf(1.f - this->w * this->w);
	return Vec3<T>(this->data) / s;
}

template<typename T>
bool Quat<T>::operator==(const Quat<T> & q) const
{
	return Vec4<T>::operator==(static_cast<Vec4<T>>(q)) || Vec4<T>::operator==(-static_cast<Vec4<T>>(q));
}

template<typename T>
bool Quat<T>::operator!=(const Quat<T> & q) const
{
	return Vec4<T>::operator!=(static_cast<Vec4<T>>(q)) && Vec4<T>::operator!=(-static_cast<Vec4<T>>(q));
}

template<typename T>
Quat<T> Quat<T>::operator!() const
{
	return Quat<T>(-this->x, -this->y, -this->z, this->w);
}

template<typename T>
Quat<T> & Quat<T>::invert()
{
	this->x = -this->x, this->y = -this->y, this->z = -this->z;
	return *this;
}

template<typename T>
Vec3<T> Quat<T>::operator*(const Vec3<T> & v) const
{
	/** @see http://people.csail.mit.edu/bkph/articles/Quaternions.pdf */
	const Vec3<T> q(this->data);
	const Vec3<T> t = T(2) * (q ^ v);
	return v + (this->w * t) + (q ^ t);
}

/////////////////////////////////////////////////
// Direction-vector conversions                //
/////////////////////////////////////////////////

template<typename T>
Vec3<T> Quat<T>::forward() const	{ return operator*(Vec3<T>::forward); }

template<typename T>
Vec3<T> Quat<T>::backward() const	{ return operator*(Vec3<T>::backward); }

template<typename T>
Vec3<T> Quat<T>::down() const		{ return operator*(Vec3<T>::down); }

template<typename T>
Vec3<T> Quat<T>::up() const			{ return operator*(Vec3<T>::up); }

template<typename T>
Vec3<T> Quat<T>::left() const		{ return operator*(Vec3<T>::left); }

template<typename T>
Vec3<T> Quat<T>::right() const		{ return operator*(Vec3<T>::right); }

/////////////////////////////////////////////////
// Float 32-bit specialization                 //
/////////////////////////////////////////////////

#if PLATFORM_ENABLE_SIMD
template<>
Quat<float32>::Quat(const Vec3<float32> & v, const float32 a) : Vec4(v.getNormal() * sinf(a / 2.f), cosf(a / 2.f)) {}

template<>
Quat<float32> & Quat<float32>::invert()
{
	data = _mm_xor_ps(data, _mm_set_ps(-0.f, -0.f, -0.f, 0.f));
	return *this;
}

template<>
Quat<float32> Quat<float32>::operator*(const Quat<float32> & q) const
{
	#define shuffle(v, x, y, z, w) _mm_shuffle_ps(v, v, _MM_SHUFFLE(x, y, z, w))
	#define mul(v1, v2) _mm_mul_ps(v1, v2)
	#define add(v1, v2) _mm_add_ps(v1, v2)
	#define sgn(v1, v2) _mm_xor_ps(v1, v2)
	#define set(x, y, z, w) _mm_set_ps(x, y, z, w)

	return Quat<float32>(add(
		add(
			mul(
				shuffle(q.data, 0, 0, 0, 0),
				data
			),
			mul(
				sgn(
					set(0.f, -0.f, 0.f, -0.f),
					shuffle(q.data, 3, 3, 3, 3)
				),
				shuffle(data, 0, 1, 2, 3)
			)
		),
		add(
			mul(
				sgn(
					set(0.f, 0.f, -0.f, -0.f),
					shuffle(q.data, 2, 2, 2, 2)
				),
				shuffle(data, 1, 0, 3, 2)
			),
			mul(
				sgn(
					set(-0.f, 0.f, 0.f, -0.f),
					shuffle(q.data, 1, 1, 1, 1)
				),
				shuffle(data, 2, 3, 0, 1)
			)
		)
	));

	#undef shuffle
	#undef mul
	#undef add
	#undef sgn
	#undef set
}
#endif

template<>
void Quat<float32>::print(FILE * stream) const
{
	fprintf(stream, "q(%.3f, %.3f, %.3f, %.3f)\n", x, y, z, w);
}

/////////////////////////////////////////////////
// Type definitions                            //
/////////////////////////////////////////////////

/**
 * @brief Type definitions for common quaternion types
 * @{
 */
typedef Quat<float32> quat;
/** @} */
