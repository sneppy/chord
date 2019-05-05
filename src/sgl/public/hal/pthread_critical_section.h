#pragma once

#include "core_types.h"
#include <pthread.h>

#if !PLATFORM_USE_PTHREADS
	#error "Trying to include pthreads in a platform that does not support them"
#endif

/**
 * @class PThreadCriticalSection hal/pthread_critical_section
 * @brief Critical section implementation using pthreads
 */
class PThreadCriticalSection
{
protected:
	/// @brief Underlying pthread mutex
	pthread_mutex_t mutex;

public:
	/// @brief Default-constructor
	FORCE_INLINE PThreadCriticalSection()
	{
		// Create recursive mutex
		pthread_mutexattr_t options;
		pthread_mutexattr_init(&options);
		pthread_mutexattr_settype(&options, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&mutex, &options);
		pthread_mutexattr_destroy(&options);
	}

	/// @brief Copy-constructor, removed
	PThreadCriticalSection(const PThreadCriticalSection&) = delete;

	/// @brief Assignment-operator, removed
	PThreadCriticalSection & operator=(const PThreadCriticalSection&) = delete;

	/// @brief Destructor, releases critical section
	FORCE_INLINE ~PThreadCriticalSection() { pthread_mutex_destroy(&mutex); }

	/// @brief Performs lock
	/// @{
	FORCE_INLINE void lock() { pthread_mutex_lock(&mutex); }
	/// @return @c true if lock was effectively acquired
	FORCE_INLINE bool tryLock() { return pthread_mutex_lock(&mutex) == 0; }
	/// @}

	/// @brief Unlocks this mutex
	FORCE_INLINE void unlock() { pthread_mutex_unlock(&mutex); }
};

/**
 * @class PThreadRWLock hal/pthread_critical_section.h
 * @brief Provides shared-exclusive lock using pthreads
 */
class PThreadRWLock
{
protected:
	pthread_rwlock_t mutex;

public:
	/// @brief Default-constructor
	FORCE_INLINE PThreadRWLock() { pthread_rwlock_init(&mutex, nullptr); }

	/// @brief Destructor, releases lock
	FORCE_INLINE ~PThreadRWLock() { pthread_rwlock_destroy(&mutex); }

	/// @brief Acquire a (shared) read lock
	FORCE_INLINE void readLock() { pthread_rwlock_rdlock(&mutex); }

	/// @brief Acquire a (shared) read lock
	FORCE_INLINE void writeLock() { pthread_rwlock_wrlock(&mutex); }

	/// @brief Acquire a (shared) read lock
	FORCE_INLINE void unlock() { pthread_rwlock_unlock(&mutex); }

	/// @brief Acquire a (shared) read lock
	FORCE_INLINE void readUnlock() { pthread_rwlock_unlock(&mutex); }

	/// @brief Acquire a (shared) read lock
	FORCE_INLINE void writeUnlock() { pthread_rwlock_unlock(&mutex); }
};

