#pragma once

#include "vec3.h"

/**
 * @struct Box<T> box.h
 * @brief A tempalated box struct.
 * @details A box is identified by a pair of
 * points. Box is assumed to be axis-aligned
 */
template<typename T = float32>
struct Box
{
public:
	/**
	 * @brief Box min and max vertices
	 * @{
	 */
	Vec3<T> min, max;
	/** @} */

public:
	/**
	 * @brief Default constructor
	 */
	FORCE_INLINE Box();

	/**
	 * @brief Corner-constructor
	 * 
	 * @param [in] min,max min and max vertices
	 */
	FORCE_INLINE Box(const Vec3<T> & min, const Vec3<T> & max);

	/**
	 * @brief Get box size
	 * 
	 * @return size
	 */
	FORCE_INLINE Vec3<T> getSize() const;

	/**
	 * @brief Get box center
	 * 
	 * @return center
	 */
	FORCE_INLINE Vec3<T> getCenter() const;

	/**
	 * @brief Get box extent
	 * 
	 * @return extent
	 */
	FORCE_INLINE Vec3<T> getExtent() const;

	/**
	 * @brief Get box volume
	 * 
	 * @return volume
	 */
	FORCE_INLINE T getVolume() const;

	/**
	 * @brief Comparison operators
	 * 
	 * @param [in] b other box
	 * 
	 * @return @c true if box are equal (same min and max), false otherwise
	 * @{
	 */
	FORCE_INLINE bool operator==(const Box<T> & b) const;
	FORCE_INLINE bool operator!=(const Box<T> & b) const;
	/** @} */

	/**
	 * @brief Extend this bounding box to include given point
	 * 
	 * @param [in] v point to include
	 * @{
	 */
	/** @return extended box */
	FORCE_INLINE Box<T> operator+(const Vec3<T> & v) const;
	/** @return self */
	FORCE_INLINE Box<T> & operator+=(const Vec3<T> & v);
	/** @} */

	/**
	 * @brief Extend this bounding box to include given box
	 * 
	 * @param [in] b other box
	 * @{
	 */
	/** @return exteded box */
	FORCE_INLINE Box<T> operator+(const Box<T> & b) const;
	/** @return self */
	FORCE_INLINE Box<T> & operator+=(const Box<T> & b);
	/** @} */

	/**
	 * @brief Check if is inside[contains] another box
	 * 
	 * @param [in] b other box
	 * 
	 * @return @c true if inside[contains], @c false otherwise
	 * @{
	 */
	FORCE_INLINE bool inside(const Box<T> & b) const;
	FORCE_INLINE bool contains(const Box<T> & b) const;
	/** @} */

	/**
	 * @brief Check if box contains a point
	 * 
	 * @param [in] v point
	 * 
	 * @return @c true if point is inside, @c false otherwise
	 */
	FORCE_INLINE bool contains(const Vec3<T> & v) const;
	
	/**
	 * @brief Check if two boxes overlaps
	 * 
	 * @param [in] b other box
	 * 
	 * @return @c true if overlap, @c false otherwise
	 */
	FORCE_INLINE bool intersect(const Box<T> & b) const;
	
	/**
	 * @brief Returns the overlap between two boxes
	 * 
	 * @param [in] b other box
	 * 
	 * @return overlap box
	 */
	inline Box<T> overlap(const Box<T> & b) const;
	
	/**
	 * @brief Print box vertices to stream
	 * 
	 * @param [in] stream output stream
	 */
	FORCE_INLINE void print(FILE * stream = stdout) const;
};

template<typename T>
Box<T>::Box() : min(0.f), max(0.f) {};

template<typename T>
Box<T>::Box(const Vec3<T> & min, const Vec3<T> & max) : min(min), max(max) { }

template<typename T>
Vec3<T> Box<T>::getSize() const
{
	return max - min;
}

template<typename T>
Vec3<T> Box<T>::getCenter() const
{
	Vec3<T> center = min + max;
	center *= 0.5f;
	return center;
}

template<typename T>
Vec3<T> Box<T>::getExtent() const
{
	Vec3<T> extent = max - min;
	extent *= 0.5f;
	return extent;
}

template<typename T>
T Box<T>::getVolume() const
{
	Vec3<T> s = max - min;
	return s.x * s.y * s.z;
}

template<typename T>
bool Box<T>::operator==(const Box<T> & b) const
{
	return min == b.min && max == b.max;
}

template<typename T>
bool Box<T>::operator!=(const Box<T> & b) const
{
	return min != b.min || max != b.max;
}

template<typename T>
Box<T> Box<T>::operator+(const Vec3<T> & v) const
{
	return Box<T>(Math::min(min, v), Math::max(max, v));
}

template<typename T>
Box<T> & Box<T>::operator+=(const Vec3<T> & v)
{
	min = Math::min(min, v), max = Math::max(max, v);
	return *this;
}

template<typename T>
Box<T> Box<T>::operator+(const Box<T> & b) const
{
	return Box<T>(Math::min(min, b.min), Math::max(max, b.max));
}

template<typename T>
Box<T> & Box<T>::operator+=(const Box<T> & b)
{
	min = Math::min(min, b.min), max = Math::max(max, b.max);
	return *this;
}

template<typename T>
bool Box<T>::inside(const Box<T> & b) const
{
	return min > b.min & max < b.max;
}

template<typename T>
bool Box<T>::contains(const Box<T> & b) const
{
	return min < b.min & max > b.max;
}

template<typename T>
bool Box<T>::contains(const Vec3<T> & v) const
{
	return min < v & v < max;
}

template<typename T>
bool Box<T>::intersect(const Box<T> & b) const
{
	return (max > b.min & b.max > min) | (b.max > min & max > b.min);
}

template<typename T>
Box<T> Box<T>::overlap(const Box<T> & b) const
{
	if (max > b.min & b.max > min)
		return Box<T>(b.min, max);
	else if (b.max > min & max > b.min)
		return Box<T>(min, b.max);
	else
		return Box<T>();
}

/////////////////////////////////////////////////
// Float-32 bit specialization                 //
/////////////////////////////////////////////////

template<>
void Box<float32>::print(FILE * stream) const
{
	fprintf(stream, "min(%.3f, %.3f, %.3f) : max(%.3f, %.3f, %.3f)\n", min.x, min.y, min.z, max.x, max.y, max.z);
}
<<<<<<< HEAD

/// @brief Common type definitions
/// @{
typedef Box<float32>	box;
typedef Box<int32>		ibox;
typedef Box<uint32>		ubox;
/// @}

=======
>>>>>>> math
