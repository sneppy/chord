#pragma once

#include "core_types.h"

#include "generic/generic_platform_atomics.h"
#if PLATFORM_WINDOWS
	#include "windows/windows_platform_atomics.h"
#elif PLATFORM_APPLE
	#include "apple/apple_platform_atomics.h"
#elif PLATFORM_UNIX
	#include "unix/unix_platform_atomics.h"
#else
	#error "Unkown platform"
#endif

/// @brief Specify memory read/write restriction
enum class AtomicOrder
{
	Relaxed,	// Relaxed, weaker
	Sequential	// Sequentially consistent, stronger	
};

