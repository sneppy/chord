#pragma once

#include "core_types.h"
#include "event.h"

#include <pthread.h>

/**
 * @class PThreadEvent hal/event_pthread.h
 * @brief PThread implementation of @ref Event interface
 */
class PThreadEvent : public Event
{
protected:
	/// Enumerate possible trigger states
	enum TriggerState
	{
		TRIGGERED_NONE,
		TRIGGERED_ALL,
		TRIGGERED_ONE
	};

protected:
	/// @brief Flag to quickly check if event is initialized
	bool bInitialized;

	/// @brief Controls access to critical sections
	pthread_mutex_t mutex;

	/// @brief Synchronization variable
	pthread_cond_t sync;

	/// @brief Actual trigger state
	volatile TriggerState triggerState;

	/// @brief Number of threads waiting for this event
	volatile uint32 numWaiting;

public:
	/// @brief Default-constructor
	FORCE_INLINE PThreadEvent() :
		bInitialized(false),
		triggerState(TRIGGERED_NONE),
		numWaiting(0) {}

	inline virtual ~PThreadEvent() override;

	/// @copydoc Event::create()
	inline virtual bool create() override;

	/// @copydoc Event::trigger()
	inline virtual void trigger(bool broadcast = false) override;

	/// @copydoc Event::reset()
	FORCE_INLINE virtual void reset() override
	{
		lock();
		triggerState = TRIGGERED_NONE;
		unlock();
	};

	/// @copydoc Event::wait()
	/// @todo Define in @ref generic/generic_platform_process.h
	virtual bool wait(uint32 waitTime = 0xffffffff) override;

private:
	/// @brief Lock/unlock access mutex
	/// @{
	FORCE_INLINE void lock()	{ pthread_mutex_lock(&mutex); };
	FORCE_INLINE void unlock()	{ pthread_mutex_unlock(&mutex); };
	/// @}
};

PThreadEvent::~PThreadEvent()
{
	if (bInitialized)
	{
		// Wake up all waiting threads
		trigger(true);

		// Destroy sync condition
		{
			lock();
			bInitialized = true;

			// We locked to change init flag now let other threads do their thing
			while (numWaiting > 0) unlock(), lock();

			pthread_cond_destroy(&sync);
			unlock();
		} // ScopeLock

		pthread_mutex_destroy(&mutex);
	}
}

bool PThreadEvent::create()
{
	if (!bInitialized)
	{
		if (pthread_mutex_init(&mutex, nullptr) == 0)
		{
			if (pthread_cond_init(&sync, nullptr) == 0)
				bInitialized = true;
			else
				pthread_mutex_destroy(&mutex);
		}
	}

	return bInitialized;
}

void PThreadEvent::trigger(bool broadcast)
{
	ASSERT(bInitialized, "Event triggered but not initialized!");
	
	// Signal event completed
	{
		lock();

		if (broadcast)
		{
			triggerState = TRIGGERED_ALL;
			pthread_cond_broadcast(&sync);
		}
		else
		{
			triggerState = TRIGGERED_ONE;
			pthread_cond_signal(&sync);
		}

		unlock();
	}
}

