#pragma once

/// Templates to select a type based on pointer size
/// @{
template<typename T32, typename T64, unsigned char N>
struct SelectIntPointerType {};

template<typename T32, typename T64>
struct SelectIntPointerType<T32, T64, 0x4> { typedef T32 type; };

template<typename T32, typename T64>
struct SelectIntPointerType<T32, T64, 0x8> { typedef T64 type; };
/// @}


/**
 * @struct GenericPlatformTypes generic/generic_platform.h
 * @brief Generic type definitions
 */
struct GenericPlatformTypes
{
	/**
	 * @brief Integer types
	 * @{
	 */
	typedef signed char			byte;
	typedef signed char			int8;
	typedef signed short		int16;
	typedef signed int			int32;
	typedef signed long long	int64;

	typedef unsigned char		ubyte;
	typedef unsigned char		uint8;
	typedef unsigned short		uint16;
	typedef unsigned int		uint32;
	typedef unsigned long long	uint64;
	/// @}

	/**
	 * @brief Floating-point types
	 * @{
	 */
	typedef float		float32;
	typedef double		float64;
	typedef long double	float128;
	/// @}

	/**
	 * @brief Pointer types
	 * @{
	 */
	typedef SelectIntPointerType<int32, int64, sizeof(void*)>::type		intP;
	typedef SelectIntPointerType<uint32, uint64, sizeof(void*)>::type	uintP;

	typedef intP	ssizet;
	typedef uintP	sizet;
	/// @}

	/**
	 * @brief Character types
	 * @{
	 */
	typedef char		ansichar;
	typedef wchar_t		widechar;
	typedef uint8		char8;
	typedef uint16		char16;
	typedef uint32		char32;
	typedef widechar	tchar;
	/// @}
};
