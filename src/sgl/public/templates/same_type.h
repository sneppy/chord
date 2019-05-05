#pragma once

/**
 * Sets value to true if T and U
 * are the same type
 */
template<typename T, typename U>
struct SameType { enum {value = false}; };

template<typename T>
struct SameType<T, T> { enum {value = true}; };