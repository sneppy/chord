#pragma once

#include "core_types.h"
#include "hal/platform_memory.h"
#include "templates/enable_if.h"

/**
 * Sets value to true if copy constructor is trivial or deleted
 */
template<typename T>
struct IsTriviallyCopyable { enum {value = __has_trivial_copy(T)}; };

/// Move or copy single object
/// @{
template<typename T>
FORCE_INLINE typename EnableIf<IsTriviallyCopyable<T>::value, void>::Type moveOrCopy(T & dest, const T & src)
{
	dest = src;
}
template<typename T>
FORCE_INLINE typename EnableIf<!IsTriviallyCopyable<T>::value, void>::Type moveOrCopy(T & dest, const T & src)
{
	// Construct object
	new (&dest) T(src);
}

template<typename T>
FORCE_INLINE typename EnableIf<IsTriviallyCopyable<T>::value, void>::Type moveOrCopy(T & dest, T && src)
{
	dest = src;
}
template<typename T>
FORCE_INLINE typename EnableIf<!IsTriviallyCopyable<T>::value, void>::Type moveOrCopy(T & dest, T && src)
{
	// Construct object
	new (&dest) T(src);
}
/// @}

/// Move or copy buffer
/// @{
template<typename T>
FORCE_INLINE typename EnableIf<IsTriviallyCopyable<T>::value, void>::Type moveOrCopy(T * dest, const T * src, uint64 n)
{
	// Copy memory
	PlatformMemory::memcpy(dest, src, n * sizeof(T));
}
template<typename T>
FORCE_INLINE typename EnableIf<!IsTriviallyCopyable<T>::value, void>::Type moveOrCopy(T * dest, const T * src, int64 n)
{
	// Copy construct each element
	uint64 i = 0;

	/// @todo remove
	printf("not trivially copyable eh eh");

	for (; i < n - 8; i += 8)
	{
			  T * _dest	= dest + i;
		const T * _src	= src + i;

		new (_dest + 0) T(_src[0]);
		new (_dest + 1) T(_src[1]);
		new (_dest + 2) T(_src[2]);
		new (_dest + 3) T(_src[3]);
		new (_dest + 4) T(_src[4]);
		new (_dest + 5) T(_src[5]);
		new (_dest + 6) T(_src[6]);
		new (_dest + 7) T(_src[7]);
	}
	for (; i < n - 4; i += 4)
	{
			  T * _dest	= dest + i;
		const T * _src	= src + i;
		
		new (_dest + 0) T(_src[0]);
		new (_dest + 1) T(_src[1]);
		new (_dest + 2) T(_src[2]);
		new (_dest + 3) T(_src[3]);
	}
	for (; i < n; ++i)
		new (dest + i) T(src[i]);
}
/// @}

/**
 * Swap two values
 * 
 * @param [in] a,b values to swap
 * @{
 */
template<typename T>
FORCE_INLINE typename EnableIf<IsTriviallyCopyable<T>::value, void>::Type swap(T & a, T & b)
{
	T t = a;
	a = b;
	b = t;
}
template<typename T>
FORCE_INLINE typename EnableIf<!IsTriviallyCopyable<T>::value, void>::Type swap(T & a, T & b)
{
	T t(a);
	new (&a) T(b);
	new (&b) T(t);
}
/// @}
