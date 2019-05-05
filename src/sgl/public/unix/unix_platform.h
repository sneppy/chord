#pragma once

#include "generic/generic_platform.h"

/**
 * @struct UnixPlatformTypes unix/unix_platform.h
 * @brief Unix specific type definitions
 */
using PlatformTypes = struct UnixPlatformTypes : public GenericPlatformTypes
{
	// Empty
};

/// Architecture

#if defined(_LINUX64) || defined(_LP64)
	#define PLATFORM_64 1
#else
	#define PLATFORM_64 0
#endif

#define PLATFORM_LITTLE_ENDIAN	1
#define PLATFORM_USE_PTHREADS	1

/// Enable SIMD intrinsics on x86[-64] only

#if defined(_M_IX86) || defined(__i386) || defined(_M_X64) || defined(__x86_64) || defined(__amd64)
	#define PLATFORM_ENABLE_SIMD 1
#else
	#define PLATFORM_ENABLE_SIMD 0
#endif

/// Compiler attributes macros

#define ABSTRACT					abstract
#if SGL_BUILD_DEBUG
	#define FORCE_INLINE			inline // Let compiler decide
#else
	#define FORCE_INLINE			__attribute__((always_inline)) inline
#endif
#define FORCE_NOINLINE				__attribute__((noinline))
#define RESTRICT					__restrict
#define LIKELY(expr)				__builtin_expect(static_cast<bool>(expr), 1)
#define UNLIKELY(expr)				__builtin_expect(static_cast<bool>(expr), 0)
#define FUNCTION_CHECK_RETURN_END	__attribute__((warn_unused_return))
#define GCC_PACK(n)					__attribute__((packed,aligned(n)))
#define GCC_ALIGN(n)				__attribute__((aligned(n)))
