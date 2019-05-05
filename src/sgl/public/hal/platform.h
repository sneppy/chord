#pragma once

/// Define all other platforms to be 0
#ifndef PLATFORM_WINDOWS
	#define PLATFORM_WINDOWS 0
#endif
#ifndef PLATFORM_APPLE
	#define PLATFORM_APPLE 0
#endif
#ifndef PLATFORM_UNIX
	#define PLATFORM_UNIX 0
#endif

/// Include generic platform header
#include "generic/generic_platform.h"

/// Include specific platform headers
#if PLATFORM_WINDOWS
	#include "windows/windows_platform.h"
#elif PLATFORM_APPLE
	#include "apple/apple_platform.h"
#elif PLATFORM_UNIX
	#include "unix/unix_platform.h"
#else
	#error "Unkown platform"
#endif

/// Finalize defines

#ifndef PLATFORM_64
	#define PLATFORM_64 0
#endif
#ifndef PLATFORM_32
	#define PLATFORM_32 !PLATFORM_64
#endif
#ifndef PLATFORM_LITTLE_ENDIAN
	#define PLATFORM_LITTLE_ENDIAN 0
#endif
#ifndef PLATFORM_ENABLE_SIMD
	#define PLATFORM_ENABLE_SIMD 0
#endif
//#define PLATFORM_ENABLE_SIMD 0
#ifndef PLATFORM_HAS_TOUCH_MAIN_SCREEN
	#define PLATFORM_HAS_TOUCH_MAIN_SCREEN 0
#endif
#ifndef PLATFORM_USE_PTHREADS
	#define PLATFORM_USE_PTHREADS 1
#endif

/// Compiler attributes

#ifndef FORCE_INLINE
	#define FORCE_INLINE inline
#endif
#ifndef LIKELY
	#define LIKELY(condition) (condition)
#endif
#ifndef UNLIKELY
	#define UNLIKELY(condition) (condition)
#endif
#ifndef RESTRICT
	#define RESTRICT
#endif

/// Method modifiers

#ifndef ABSTRACT
	#define ABSTRACT
#endif
#ifndef CONSTEXPR
	#define CONSTEXPR constexpr
#endif
#ifndef VOID_FUNC
	#define VOID_FUNC {}
#endif

/// Alignment

#ifndef GCC_PACK
	#define GCC_PACK(n)
#endif
#ifndef GCC_ALIGN
	#define GCC_ALIGN(n) alignas(n)
#endif

/// Assertions

#include <cassert>

#ifndef ASSERT
	#define ASSERT(cond, text) assert(cond && text)
#endif

/// Expose global plaftorm type definitions

/// @brief Integer types
/// @{
typedef PlatformTypes::byte		byte;
typedef PlatformTypes::int8		int8;
typedef PlatformTypes::int16	int16;
typedef PlatformTypes::int32	int32;
typedef PlatformTypes::int64	int64;

typedef PlatformTypes::ubyte	ubyte;
typedef PlatformTypes::uint8	uint8;
typedef PlatformTypes::uint16	uint16;
typedef PlatformTypes::uint32	uint32;
typedef PlatformTypes::uint64	uint64;
/// @}

/// @brief Floating-point types
/// @{
typedef PlatformTypes::float32	float32;
typedef PlatformTypes::float64	float64;
typedef PlatformTypes::float128	float128;
/// @}

/// @brief Pointer types
/// @{
typedef PlatformTypes::intP		intP;
typedef PlatformTypes::uintP	uintP;

typedef PlatformTypes::ssizet	ssizet;
typedef PlatformTypes::sizet	sizet;
/// @}

/// @brief Character types
/// @{
typedef PlatformTypes::ansichar	ansichar;
typedef PlatformTypes::widechar	widechar;
typedef PlatformTypes::tchar	tchar;
typedef PlatformTypes::char8	char8;
typedef PlatformTypes::char16	char16;
typedef PlatformTypes::char32	char32;
/// @}

