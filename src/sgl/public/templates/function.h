#pragma once

#include "core_types.h"
#include <functional>

/**
 * @class Function templates/function.h
 * @brief Binds a functor or a lambda to this object
 */
template<typename FuncT>
using Function = std::function<FuncT>;

/**
 * @struct IsFunction templates/function.h
 * @brief Checks if T is a @ref Function
 * @{
 */
template<typename T> struct IsFunction				{ enum {value = false}; };
template<typename T> struct IsFunction<Function<T>>	{ enum {value = true}; };

#define IsFunctionV(T)	IsFunction<T>::value
/// @}

