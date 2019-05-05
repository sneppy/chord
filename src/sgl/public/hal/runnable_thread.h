#pragma once

#include "core_types.h"
#include "containers/array.h"
#include "containers/string.h"

/// Forward declarations
/// @{
class Runnable;
/// @}

/**
 * @class RunnableThread hal/runnable_thread.h
 * @brief Interface to manage a thread
 */
class RunnableThread
{
protected:
	/// @brief Thread identifier, set during creation
	uint64 id;

	/// @brief Thread name
	String name;

	/// @brief The runnable to execute on this thread
	Runnable * runnable;

public:
	/// @brief Default-constructor
	RunnableThread();

	/// @brief Virtual-destructor
	virtual ~RunnableThread();

	/// @brief Returns thread id
	FORCE_INLINE uint64 getThreadId() { return id; }

	/// @brief Returns thread name
	FORCE_INLINE const String & getThreadName() { return name; }

	/**
	 * @brief Kills the thread
	 * 
	 * @param [in]	bShouldWait	if true, halt execution until
	 * 							thread is actually killed. This
	 * 							way the caller knows it
	 */
	virtual void kill(bool bShouldWait = true) = 0;

	
	/// @brief Halts caller execution, and waits for process completion
	virtual void join() = 0;

	/**
	 * Factory method for the creation of a new thread
	 * 
	 * @copyparam create_internal
	 */
	static RunnableThread * create(Runnable * runnable, const ansichar * _name, uint32 stackSize = 0);

protected:
	/**
	 * @brief Implementation-specific thread creation
	 * 
	 * @param [in]	_runnable	the runnable to execute
	 * @param [in]	_name		thread name
	 * @param [in]	stackSize	override default stack size
	 * 
	 * @return status flag
	 */
	virtual bool create_internal(Runnable * _runnable, const ansichar * _name, uint32 stackSize = 0) = 0;
};

