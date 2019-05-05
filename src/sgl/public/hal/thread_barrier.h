#pragma once

#if PLATFORM_WINDOWS
	#include "windows/windows_platform_thread_barrier.h"
#elif PLATFORM_APPLE
	#include "apple/apple_platform_thread_barrier.h"
#elif PLATFORM_UNIX
	#include "unix/unix_platform_thread_barrier.h"
#else
	#error "Unkown platform"
#endif

/**
 * @class ScopeBarrier hal/thread_barrier.h
 * @brief Synchronizes a block of code between separate threads
 */
class ScopeBarrier
{
protected:
	/// @brief Barrier used for synchronization
	Barrier * barrier;

public:
	/// @brief Default-constructor, do nothing
	FORCE_INLINE ScopeBarrier(Barrier * _barrier) : barrier(_barrier) {}

	/// @brief Destructor, wait for other threads to reach barrier
	FORCE_INLINE ~ScopeBarrier() { barrier->wait(); }
};

