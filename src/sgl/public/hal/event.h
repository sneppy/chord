#pragma once

#include "core_types.h"
#include "templates/atomic.h"

/**
 * @class Event hal/event.h
 * @brief Interface for system events
 */
class Event
{
protected:
	/// @brief Auto incrementing unique id, used to assign a unique id to a new event
	static Atomic<uint32> uniqueId;

	/// @brief Event id
	uint32 id;

public:
	/// @brief Default-constructor
	Event() : id(0) {}

	/// @brief Destructor, virtual
	virtual ~Event() {};

	/// @brief Creates the event
	virtual bool create() = 0;

	/// @brief Triggers the event
	/// @param [in] broadcast if @c true wakes all threads, otherwise only one
	virtual void trigger(bool broadcast = false) = 0;

	/// @brief Reset a triggered event
	virtual void reset() = 0;

	/**
	 * @brief Waits for the event to be triggered
	 * 
	 * @param [in] waitTime max wait time (ms)
	 * 
	 * @return @c true if event was triggered, @c false if timed out
	 */
	virtual bool wait(uint32 waitTime = 0xffffffff) = 0;
};

