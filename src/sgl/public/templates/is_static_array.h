#pragma once

#include "core_types.h"
#include <type_traits>

/**
 * Just a typedef.
 * And it's gonna stay like that
 */
template<typename T>
using IsStaticArray = std::is_array<T>;

/// @brief Quick value of @ref IsPointer
#define IsStaticArrayV(type) IsArray<type>::value

