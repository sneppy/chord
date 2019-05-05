#pragma once

/**
 * Sets Type to a constant reference if
 * sizeof type is greater than sizeof pointer
 */
template<typename T, bool = (sizeof(T) > sizeof(void*))>
struct ConstRef {};

template<typename T> struct ConstRef<T, false>	{ using Type = T; };
template<typename T> struct ConstRef<T, true>	{ using Type = const T&; };
template<typename T> struct ConstRef<T&&, true>	{ using Type = T&&; };
template<typename T> struct ConstRef<T*, false>	{ using Type = T*; };