#pragma once

#include "core_types.h"
#include <type_traits>

/**
 * Just a typedef.
 * And it's gonna stay like that
 */
template<class Base, class Derived>
using IsBaseOf = std::is_base_of<Base, Derived>;

/// @brief Quick value of @ref IsPointer
#define IsBaseOfV(base, derived) IsBaseOf<base, derived>::value

