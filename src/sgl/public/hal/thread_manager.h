#pragma once

#include "core_types.h"
#include "templates/singleton.h"
#include "hal/critical_section.h"
#include "containers/string.h"
#include "containers/map.h"

/// Forward declare
class RunnableThread;

/**
 * @class ThreadManager hal/thread_manager.h
 * @brief Manages threads
 */
class ThreadManager : public Singleton<ThreadManager>
{
protected:
	/// @brief List of thread objects
	Map<uint64, RunnableThread*, Compare<uint64>, MallocBinned> threads;

	/// @brief Critical section for threads list access
	CriticalSection threadsCS;

public:
	/// @brief Returns name of the thread with id
	const String & getThreadName(uint64 id);

	/// @brief Adds a new thread to the list
	FORCE_INLINE void add(uint64 id, RunnableThread * thread)
	{
		// Acquire exclusive lock
		ScopeLock scopeLock(&threadsCS);
		threads.insert(id, thread);
	}

	/// @brief Adds a new thread to the list
	/// @{
	FORCE_INLINE void remove(uint64 id)
	{
		// Acquire lock
		ScopeLock scopeLock(&threadsCS);
		/** Not implemented */
	};
	void remove(RunnableThread * thread);
	/// @}
};

