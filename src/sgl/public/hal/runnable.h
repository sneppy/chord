#pragma once

#include "core_types.h"

/**
 * @class Runnable hal/runnable.h
 * @brief A runnable is a short-term task that
 * can be run by a @ref RunnableThread
 */
class Runnable
{
public:
	/// @brief Virtual-destructor
	virtual FORCE_INLINE ~Runnable() {};

	/**
	 * @brief Initialized the Runnable
	 * 
	 * @return if initialization fails, thread is killed
	 */
	virtual FORCE_INLINE bool init() { return true; }

	/**
	 * @brief Runnable body
	 * 
	 * @return exit code
	 */
	virtual FORCE_INLINE int32 run() { return 0; }

	/**
	 * @brief Stop the runnable objects gently
	 * 
	 * Called when the thread requests to terminate early
	 */
	virtual FORCE_INLINE void stop() {}

	/**
	 * @brief Normally exits after run
	 */
	virtual FORCE_INLINE void exit() {}
};

