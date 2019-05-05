#ifndef SGL_MATH_HPP
#define SGL_MATH_HPP

#include "core_types.h"
#include "hal/platform_math.h"

/**
 * @brief Forward declarations of structs
 * that use SIMD data types
 * @{
 */
template<typename>	struct Vec2;
template<typename>	struct Vec3;
template<typename>	struct Vec4;
template<typename>	struct Quat;
template<typename>	struct Mat4;
template<typename>	struct Box;
					struct Math;
/** @} */

/**
 * @brief Pi alias
 */
#define PI M_PI

/**
 * @brief Angle conversion macros
 * @{
 */
#define RAD(alpha) alpha * PI / 180.f
#define DEG(alpha) alpha * 180.f / PI
/** @} */

/**
 * @brief 
 */

/**
 * @struct Math math.h
 * @brief A set of math functions
 */
struct Math : public PlatformMath
{
public:
	/**
	 * @brief Return delta angle (in radians)
	 * 
	 * @param [in] a,b angles values (in radians)
	 * 
	 * @return min delta angle
	 */
	template<typename T> inline static T findDeltaAngle(T a, T b);

	/**
	 * @brief Flip angle if out of bounds [-PI;PI]
	 * 
	 * @param [in] a angle
	 * 
	 * @return adjusted angle
	 */
	template<typename T> inline static T unwindAngle(T a);

	/**
	 * @brief compute min[max] vector element-wise
	 * 
	 * @param [in] v1,v2 two vectors
	 * 
	 * @return vector with min[max] elements
	 * @{
	 */
	template<typename T> FORCE_INLINE static Vec2<T> min(const Vec2<T> & v1, const Vec2<T> & v2);
	template<typename T> FORCE_INLINE static Vec3<T> min(const Vec3<T> & v1, const Vec3<T> & v2);
	template<typename T> FORCE_INLINE static Vec4<T> min(const Vec4<T> & v1, const Vec4<T> & v2);

	template<typename T> FORCE_INLINE static Vec2<T> max(const Vec2<T> & v1, const Vec2<T> & v2);
	template<typename T> FORCE_INLINE static Vec3<T> max(const Vec3<T> & v1, const Vec3<T> & v2);
	template<typename T> FORCE_INLINE static Vec4<T> max(const Vec4<T> & v1, const Vec4<T> & v2);
	/** @} */

	/**
	 * @brief Linear interpolation (templated)
	 * 
	 * @param [in]	a,b		values to interpolate
	 * @param [in]	alpha	interpolation value
	 * 
	 * @return interpolated value
	 */
	template<typename T, typename U> inline static T lerp(const T & a, const T & b, U alpha);

	/**
	 * @brief Perform a 2D lerp
	 * 
	 * @param [in]	a,b,c,d	values to interpolate
	 * @param [in]	a1,a2	interpolation steps
	 */
	template<typename T, typename U> inline static T bilerp(const T & a, const T & b, const T & c, const T & d, U a1, U a2);

	/**
	 * @brief Return reflected vector
	 * 
	 * @param [in]	v	bounced vector
	 * @param [in]	n	surface normal
	 * 
	 * @return reflected vector
	 */
	template<typename T> inline static Vec3<T> getReflectedVector(const Vec3<T> & v, const Vec3<T> & n);

	/**
	 * @brief Return intersection between ray
	 * (origin, direction) and plane
	 * @details Assumes ray and plane not parallel
	 * 
	 * @param [in]	ro,rd	origin and direction of the ray
	 * @param [in]	po,pn	origin and normal of the plane
	 * 
	 * @return point of intersection
	 */
	template<typename T> inline static Vec3<T> rayPlaneIntersect(const Vec3<T> & ro, const Vec3<T> & rd, const Vec3<T> & po, const Vec3<T> & pn);

	/**
	 * @brief Return intersection between line
	 * (p0, p1) and plane
	 * 
	 * @param [in]	p0,p1	two points defining the line
	 * @param [in]	po, pn	origin and normal of the plane
	 * 
	 * @return point of intersection
	 */
	template<typename T> FORCE_INLINE static Vec3<T> linePlaneIntersect(const Vec3<T> & p0, const Vec3<T> & p1, const Vec3<T> & po, const Vec3<T> & pn);

	/**
	 * @brief Intersect segment with plane
	 * 
	 * @param [in]	p0,p1	segment vertices
	 * @param [in]	po,pn	origin and normal of the plane
	 * @param [out]	hit		intersection point
	 * 
	 * @return @c true if intersect, @c false otherwise
	 */
	template<typename T> inline static bool intersect(const Vec3<T> & p0, const Vec3<T> & p1, const Vec3<T> & po, const Vec3<T> & pn, Vec3<T> & hit);
};

#endif