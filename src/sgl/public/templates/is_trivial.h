#pragma once

#include "core_types.h"
#include <type_traits>

/**
 * @brief Set the const member value to @c true
 * if type is trivial
 * 
 * Just a typedef for now
 */
template<typename T>
using IsTrivial = std::is_trivial<T>;

#define IsTrivialV(T) IsTrivial<T>::value

