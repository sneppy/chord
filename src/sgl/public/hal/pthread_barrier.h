#pragma once

#include "core_types.h"
#include <pthread.h>

#if !PLATFORM_USE_PTHREADS
	#error "Trying to include pthreads in a platform that does not support them"
#endif

/**
 * @brief PThreadBarrier hal/pthread_barrier
 * @brief A barrier used to synchronize multiple pthreads
 */
class PThreadBarrier
{
protected:
	/// @brief The barrier resource
	pthread_barrier_t barrier;

	/// @brief Number of threads
	const uint32 numThreads;

public:
	/// @brief Default-constructor
	FORCE_INLINE PThreadBarrier(uint32 _numThreads) : numThreads(_numThreads)
	{
		pthread_barrierattr_t options;
		pthread_barrierattr_init(&options);
		pthread_barrier_init(&barrier, &options, (int32)numThreads);
		pthread_barrierattr_destroy(&options);
	}

	/// @brief Copy-constructor, removed
	PThreadBarrier(const PThreadBarrier&) = delete;

	/// @brief Copy-assignment, removed
	PThreadBarrier & operator=(const PThreadBarrier&) = delete;

	/// @brief Destructor
	FORCE_INLINE ~PThreadBarrier() { pthread_barrier_destroy(&barrier); }

	/// @brief Wait for other threads at this barrier
	FORCE_INLINE void wait() { pthread_barrier_wait(&barrier); }
};

