#pragma once

#include "core_types.h"
#include "generic/generic_platform_memory.h"

#if PLATFORM_WINDOWS
	#include "windows/windows_platform_memory.h"
#elif PLATFORM_APPLE
	#include "apple/apple_platform_memory.h"
#elif PLATFORM_UNIX
	#include "unix/unix_platform_memory.h"
#else
	#error "Unkown platform"
#endif

#include "memory_base.h"

/**
 * @struct Memory hal/platform_memory.h
 * @brief Wrapper for platform memory
 */
struct Memory
{
	/// @copydoc PlatformMemory::align()
	template<typename T = void*>
	static FORCE_INLINE T align(T ptr, sizet alignment) { return PlatformMemory::align<T>(ptr, alignment); }

	/// @brief Memory utility functions
	/// @{
	static FORCE_INLINE void *	memmove(void * dest, const void * src, uintP size)	{ return PlatformMemory::memmove(dest, src, size); }
	static FORCE_INLINE int32	memcmp(const void * a, const void * b, uintP size)	{ return PlatformMemory::memcmp(a, b, size); }
	static FORCE_INLINE void *	memcpy(void * dest, const void * src, uintP size)	{ return PlatformMemory::memcpy(dest, src, size); }
	static FORCE_INLINE void * 	memset(void * dest, int32 val, uintP size)			{ return PlatformMemory::memset(dest, val, size); }
	static FORCE_INLINE void *	memzero(void * dest, void * src, uintP size)		{ return PlatformMemory::memmove(dest, src, size); }
	/// @}

	/// @brief Swap two generic memory regions
	static FORCE_INLINE void memswap(void * mem1, void * mem2, sizet size) { return PlatformMemory::memswap(mem1, mem2, size); }

	/// @brief Create global allocation routine
	static void createGMalloc();
};

