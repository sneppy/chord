#pragma once

#include "core_types.h"

/// Sets member 'value' to true if void,
/// false otherwise
template<typename T>	struct IsVoid		{ enum {value = false}; };
template<>				struct IsVoid<void>	{ enum {value = true}; };

/// Macro for quick access to value
#define IsVoidV(T) IsVoid<T>::value