#pragma once

#include "core_types.h"
#include "hal/event.h"
#include "hal/critical_section.h"
#include "hal/platform_process.h"
#include "templates/atomic.h"
#include "templates/function.h"
#include "templates/reference.h"
#include "templates/shared_ptr.h"

/**
 * @class GenericFutureState async/future.h
 * @brief Agnostic-type future state (container for async values)
 */
class GenericFutureState
{
public:
	/// @brief Default constructor
	FORCE_INLINE GenericFutureState() :
		completionEvent(PlatformProcess::getEvent()),
		bComplete(false) {}

	/// @brief Callback-constructor
	FORCE_INLINE GenericFutureState(Function<void()> && _callback) :
		completionEvent(PlatformProcess::getEvent()),
		bComplete(false),
		callback(::move(_callback)) {}


	/// @brief Destructor
	FORCE_INLINE ~GenericFutureState()
	{
		// Release completion event
		PlatformProcess::releaseEvent(completionEvent);
		completionEvent = nullptr;
	}

	/// @brief Returns completeness flag
	FORCE_INLINE bool isComplete() const { return bComplete; }

	/// @brief Set callback function
	FORCE_INLINE void setCallback(Function<void()> && _callback)
	{
		bool bAlreadyCompleted = isComplete();
		if (!bAlreadyCompleted)
		{
			ScopeLock _(&mutex);
			if (!isComplete()) // Check again, maybe it changed
				callback = ::move(_callback);
		}

		// If it's already completed, run it
		if (bAlreadyCompleted && callback) _callback();
	}

	/**
	 * @brief Blocks calling thread until result is available
	 * 
	 * @param [in]	the max amount of time to wait (ms)
	 * 
	 * @return @c false if timed out, @c true otherwise (result available)
	 */
	FORCE_INLINE bool wait(uint32 waitTime = ((uint32)-1)) const { return completionEvent->wait(waitTime); }
	
	/// @brief Reset state, can be waited again
	FORCE_INLINE void reset()
	{
		{
			ScopeLock _(&mutex);
			bComplete = false;
		}
		completionEvent->reset();
	}

protected:
	/// @brief Mark as complete and signal waiting threads
	FORCE_INLINE void complete()
	{
		{
			ScopeLock _(&mutex);
			bComplete = true;
		}

		completionEvent->trigger(true);
		if (callback) callback();
	}

protected:
	/// @brief Controls access to this future
	CriticalSection mutex;

	/// @brief Callback function, called when future is complete
	Function<void()> callback;

	/// @brief Event triggered when the result is available
	Event * completionEvent;

	/// @brief Indicates if the result is available
	Atomic<bool> bComplete;
};

/**
 * @class FutureState async/future.h
 * @brief State of an async calculated future result
 */
template<typename T>
class FutureState : public GenericFutureState
{
protected:
	/// @brief Calculated result
	T result;

public:
	/// @brief Default-constructor
	FORCE_INLINE FutureState() : GenericFutureState() {}

	/// @brief Inherit constructors
	//using GenericFutureState::GenericFutureState;

	/// @brief Get future result (waits for result)
	FORCE_INLINE const T & getResult() const
	{
		while (!isComplete()) wait();
		return result;
	}

	/// @brief Set result and mark complete
	FORCE_INLINE void setResult(const T & _result)
	{
		if (!isComplete())
		{
			result = _result;
			complete();
		}
	}
};

/// @todo Some more work to do here

/**
 * @class BasePromise async/future.h
 * @brief Base class for future classes
 */
template<typename T>
class BasePromise
{
protected:
	typedef SharedPtr<FutureState<T>> StateRef;

	/// @brief Ref to the future state
	StateRef state;

public:
	/// @brief Returns true if result is ready and state is valid
	FORCE_INLINE bool isReady() const { return state & state->isComplete(); }

	/// @brief Waits for the result to be ready
	FORCE_INLINE void wait(uint32 waitTime = ((uint32)-1)) const { return state && state->wait(waitTime); }

protected:
	/// @brief Default-cosntructor, internal use only
	FORCE_INLINE BasePromise() : state(std::make_shared<FutureState<T>>()) {}

	/// @brief State-constructor, internal use only
	FORCE_INLINE BasePromise(const StateRef & _state) : state(_state) {}
};

/**
 * @class Promise async/future.h
 * @brief Provides a way to subscribe to async calculated future results
 */
template<typename T>
class Promise : BasePromise<T>
{
public:
	/// @brief Inherit constructors
	using BasePromise<T>::BasePromise;

	/// @brief Default-constuctor
	Promise() = default;

	/// @brief Returns future result (waits for the result to be available)
	FORCE_INLINE const T & get() const { return this->state->getResult(); }

	/// @brief Sets future result (and signal waiting threads)
	FORCE_INLINE void set(const T & result) { this->state->setResult(result); }

	/// @brief Reset state
	FORCE_INLINE void reset() { this->state->reset(); }
};

/// @brief Void promise specialization
template<>
class Promise<void> : BasePromise<int32>
{
public:
	/// @brief Inherit constructors
	using BasePromise<int32>::BasePromise;

	/// @brief Default-constructor
	Promise() = default;

	/// @brief Returns future result
	FORCE_INLINE void get() const { this->state->wait(); }

	/// @brief Mark complete
	FORCE_INLINE void set() { this->state->setResult(0); }
};

