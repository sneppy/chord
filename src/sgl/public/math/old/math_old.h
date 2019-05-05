#pragma once

#include "math.hpp"

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "quat.h"
#include "mat4.h"
#include "box.h"

template<typename T>
T Math::findDeltaAngle(T a, T b)
{
	T delta = b - a;

	// Flip angle
	if (delta > PI)
		delta -= T(2) * PI;
	else if (delta < -PI)
		delta += T(2) * PI;
	
	return delta;
}

template<typename T>
T Math::unwindAngle(T a)
{
	while (a > PI)
		a -= T(2) * PI;
	while (a < -PI)
		a += T(2) * PI;
	
	return a;
}

template<typename T>
Vec2<T> Math::min(const Vec2<T> & v1, const Vec2<T> & v2)
{
	return Vec2<T>(
		v1.x < v2.x ? v1.x : v2.x,
		v1.y < v2.y ? v1.y : v2.y
	);
}

template<typename T>
Vec3<T> Math::min(const Vec3<T> & v1, const Vec3<T> & v2)
{
	return Vec3<T>(
		v1.x < v2.x ? v1.x : v2.x,
		v1.y < v2.y ? v1.y : v2.y,
		v1.z < v2.z ? v1.z : v2.z
	);
}

template<typename T>
Vec4<T> Math::min(const Vec4<T> & v1, const Vec4<T> & v2)
{
	return Vec4<T>(
		v1.x < v2.x ? v1.x : v2.x,
		v1.y < v2.y ? v1.y : v2.y,
		v1.z < v2.z ? v1.z : v2.z,
		v1.w < v2.w ? v1.w : v2.w
	);
}

template<typename T>
Vec2<T> Math::max(const Vec2<T> & v1, const Vec2<T> & v2)
{
	return Vec2<T>(
		v1.x > v2.x ? v1.x : v2.x,
		v1.y > v2.y ? v1.y : v2.y
	);
}

template<typename T>
Vec3<T> Math::max(const Vec3<T> & v1, const Vec3<T> & v2)
{
	return Vec3<T>(
		v1.x > v2.x ? v1.x : v2.x,
		v1.y > v2.y ? v1.y : v2.y,
		v1.z > v2.z ? v1.z : v2.z
	);
}

template<typename T>
Vec4<T> Math::max(const Vec4<T> & v1, const Vec4<T> & v2)
{
	return Vec4<T>(
		v1.x > v2.x ? v1.x : v2.x,
		v1.y > v2.y ? v1.y : v2.y,
		v1.z > v2.z ? v1.z : v2.z,
		v1.w > v2.w ? v1.w : v2.w
	);
}

template<typename T, typename U>
T Math::lerp(const T & a, const T & b, U alpha)
{
	T res = b;
	res -= a, res *= alpha, res += a;
	return res;
}

template<typename T, typename U>
T Math::bilerp(const T & a, const T & b, const T & c, const T & d, U a1, U a2)
{
	return lerp(
		lerp(a, b, a1),
		lerp(c, d, a1),
		a2
	);
}

template<typename T>
Vec3<T> Math::getReflectedVector(const Vec3<T> & v, const Vec3<T> & n)
{
	// Normalize surface normal
	const Vec3<T> normal = n.getNormal();

	return  v - (T(2) * (normal & v) * normal);
}

template<typename T>
Vec3<T> Math::rayPlaneIntersect(const Vec3<T> & ro, const Vec3<T> & rd, const Vec3<T> & po, const Vec3<T> & pn)
{
	/** @see https://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection */
	return ((po - ro) & pn) / (rd & pn) * rd + ro;
}

template<typename T>
Vec3<T> Math::linePlaneIntersect(const Vec3<T> & p0, const Vec3<T> & p1, const Vec3<T> & po, const Vec3<T> & pn)
{
	return rayPlaneIntersect(p0, p1 - p0, po, pn);
}

template<typename T>
bool Math::intersect(const Vec3<T> & p0, const Vec3<T> & p1, const Vec3<T> & po, const Vec3<T> & pn, Vec3<T> & hit)
{
	const Vec3<T> pd = (p1 - p0);
	const T d = ((po - p0) & pn) / (pd & pn);

	// If point is outside segment, return false
	if (d < T(0) || d > T(1)) return false;

	// Calc hit point and return true
	hit = d * pd + p0;
	return true;
}

/////////////////////////////////////////////////
// Float 32-bit specialization                 //
/////////////////////////////////////////////////

// No specialization for Vec2<float32>

#if PLATFORM_ENABLE_SIMD
template<>
Vec3<float32> Math::min(const Vec3<float32> & v1, const Vec3<float32> & v2)
{
	return Vec3<float32>(_mm_min_ps(v1.data, v2.data));
}

template<>
Vec4<float32> Math::min(const Vec4<float32> & v1, const Vec4<float32> & v2)
{
	return Vec4<float32>(_mm_min_ps(v1.data, v2.data));
}

template<>
Vec3<float32> Math::max(const Vec3<float32> & v1, const Vec3<float32> & v2)
{
	return Vec3<float32>(_mm_max_ps(v1.data, v2.data));
}

template<>
Vec4<float32> Math::max(const Vec4<float32> & v1, const Vec4<float32> & v2)
{
	return Vec4<float32>(_mm_max_ps(v1.data, v2.data));
}

template<>
Vec3<float32> Math::rayPlaneIntersect(const Vec3<float32> & ro, const Vec3<float32> & rd, const Vec3<float32> & po, const Vec3<float32> & pn)
{
	// Create bit mask for floats
	float32 _nan; memset(&_nan, 0xffffffff, sizeof(float32));
	const __m128 mask = _mm_set_ps(_nan, _nan, _nan, 0.f);
	
	__m128	a = _mm_mul_ps(_mm_sub_ps(po.data, ro.data), pn.data);
			a = _mm_and_ps(a, mask);
			a = _mm_hadd_ps(a, a);
			a = _mm_hadd_ps(a, a);
	__m128	b = _mm_mul_ps(rd.data, pn.data);
			b = _mm_and_ps(b, mask);
			b = _mm_hadd_ps(b, b);
			b = _mm_hadd_ps(b, b);
	__m128	d = _mm_div_ps(a, b);

	return Vec3<float32>(_mm_add_ps(
		_mm_mul_ps(d, rd.data),
		ro.data
	));
}

template<>
bool Math::intersect(const Vec3<float32> & p0, const Vec3<float32> & p1, const Vec3<float32> & po, const Vec3<float32> & pn, Vec3<float32> & hit)
{
	// Create bit mask for floats
	float32 _nan; memset(&_nan, 0xffffffff, sizeof(float32));
	const __m128 mask = _mm_set_ps(_nan, _nan, _nan, 0.f);

	// Line direction
	const __m128 ld = _mm_sub_ps(p1.data, p0.data);
	
	__m128	a = _mm_mul_ps(_mm_sub_ps(po.data, p0.data), pn.data);
			a = _mm_and_ps(a, mask);
			a = _mm_hadd_ps(a, a);
			a = _mm_hadd_ps(a, a);
	__m128	b = _mm_mul_ps(ld, pn.data);
			b = _mm_and_ps(b, mask);
			b = _mm_hadd_ps(b, b);
			b = _mm_hadd_ps(b, b);
	__m128	d = _mm_div_ps(a, b);

	float32 _d[4]; _mm_store1_ps(_d, d);
	if (*_d < 0.f || *_d > 0.f) return false;

	// Calc hit point
	hit.data = _mm_add_ps(
		_mm_mul_ps(d, ld),
		p0.data
	);

	return true;
}
#endif
<<<<<<< HEAD

=======
>>>>>>> math
