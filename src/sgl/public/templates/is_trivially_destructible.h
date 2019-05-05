#pragma once

#include "core_types.h"

/**
 * Sets value to true if destructor is trivial or deleted
 */
template<typename T>
struct IsTriviallyDestructible { enum {value = __has_trivial_destructor(T)}; };