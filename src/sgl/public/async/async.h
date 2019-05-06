#pragma once

#include "core_types.h"
#include "hal/runnable_thread.h"
#include "hal/runnable.h"
#include "templates/function.h"
#include "templates/reference.h"
#include "future.h"

/// @brief Enumerate available async execution contexts
enum class AsyncExecutionMethod
{
	/// Executed in a separate thread
	Runnable
};

/**
 * @class AsyncRunnable async/async.h
 * @brief A runnable that executes a user function
 */
template<typename RetT>
class AsyncRunnable : public Runnable
{
protected:
	/// @brief User function to run
	Function<RetT()> function;

	/// @brief Result of the function
	Promise<RetT> result;

	/// @brief Runnable thread on which we are running (promise)
	Promise<RunnableThread*> threadPromise;

public:
	/// @brief Default-constructor
	FORCE_INLINE AsyncRunnable(
		Function<RetT()> && _function,
		Promise<RetT> && _result,
		Promise<RunnableThread*> && _threadPromise
	) :
		function(_function),
		result(_result),
		threadPromise(_threadPromise) {}

	/// @copydoc Runnable::run()
	virtual uint32 run() override
	{
		if (function)
			// Run function
			result.set(function());
		
		//RunnableThread * thread = threadPromise.get();

		// @todo Unreal enqueues this deletion on a different thread.
		// Don't care for now

		return 0;
	}
};

/**
 * @brief Execute a function in an async fashion
 * 
 * @param [in]	executionMethod	async execution method
 * @param [in]	function		executed function
 * @param [in]	onComplete		completion callback
 * 
 * @return result promise
 */
template<typename RetT>
Promise<RetT> async(
	AsyncExecutionMethod executionMethod,
	Function<RetT()> && function,
	Function<void()> && onComplete = Function<void()>()
)
{
	Promise<RetT> result;

	switch (executionMethod)
	{
		case AsyncExecutionMethod::Runnable:
		{
			/// @todo We don't need this right, but we'll need it.
			/// Here we'll find the runnable thread created for
			/// this task
			Promise<RunnableThread*> threadPromise;
			AsyncRunnable<RetT> * runnable = new AsyncRunnable<RetT>(::move(function), ::move(result), ::move(threadPromise));

			// Create runnable thread and run the async runnable
			RunnableThread * thread = RunnableThread::create(runnable, "AsyncRunnable");
			threadPromise.set(thread);

			break;
		}
	}

	// Return promise of result
	return result;
}

