#pragma once

#include "core_types.h"

/**
 * @struct RemoveReference templates/reference.h
 * @brief Struct to strip the reference from a data type
 * @{
 */
template<typename T> struct RemoveReference			{ using Type = T; };
template<typename T> struct RemoveReference<T&>		{ using Type = T; };
template<typename T> struct RemoveReference<T&&>	{ using Type = T; };
/// @}

#define RemoveReferenceT(T) typename RemoveReference<T>::Type

/// @brief Casts a reference to an rvalue reference
template<typename T>
FORCE_INLINE CONSTEXPR RemoveReferenceT(T) && move(T && obj)
{
	return (RemoveReferenceT(T)&&)obj;
}

/// Forwards lvalues as lvalues or as rvalues
/// @{
template<typename T>
FORCE_INLINE CONSTEXPR T && forward(typename RemoveReference<T>::Type & obj)
{
	return static_cast<T&&>(obj);
}
template<typename T>
FORCE_INLINE CONSTEXPR T && forward(typename RemoveReference<T>::Type && obj)
{
	return static_cast<T&&>(obj);
}
/// @}
