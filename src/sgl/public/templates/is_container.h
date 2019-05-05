#pragma once

#include "core_types.h"
#include "core_fwd.h"

/**
 * @struct IsArray templates/is_container.h
 * @brief Decides if a type is an array
 * @{
 */
template<typename T> struct IsArray				{ enum {value = false}; };
template<typename T> struct IsArray<Array<T>>	{ enum {value = true}; };
/// @}

#define IsArrayV(T) IsArray<T>::value

/**
 * @struct IsContainer templates/is_container.h
 * @brief Template to decide if a type is a container type
 */
template<typename T>
struct IsContainer
{
	enum {
		value
			= IsArray<T>::value
	};
};

#define IsContainerV(T) IsContainer<T>::value

/**
 * @struct ContainerType templates/is_container.h
 * @brief Expose type of the elements of array
 * @{
 */
template<typename T> struct ContainerType			{};
template<typename T> struct ContainerType<Array<T>>	{ typedef T Type; };
/// @}

#define ContainerTypeT(T) typename ContainerType<T>::Type

