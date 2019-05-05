#pragma once

#include "core_types.h"

/**
 * Sets Type to specified type if boolean condition is true
 */
template<bool Cond, typename T = void>
struct EnableIf {};

template<typename T>
struct EnableIf<true, T> { using Type = T; };

/// Quick macro
#define EnableIfT(Cond, T) typename EnableIf<Cond, T>::Type

