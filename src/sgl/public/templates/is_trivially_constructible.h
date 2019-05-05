#pragma once

#include "core_types.h"

/**
 * Sets value to true if constructors are trivial or deleted
 */
template<typename T>
struct IsTriviallyConstructible { enum {value = __has_trivial_constructor(T)}; };