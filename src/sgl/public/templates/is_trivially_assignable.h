#pragma once

#include "core_types.h"

/**
 * Sets value to true if assignment is trivial or deleted
 */
template<typename T>
struct IsTriviallyAssignable { enum {value = __has_trivial_assign(T)}; };