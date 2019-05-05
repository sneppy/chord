#pragma once

#include "core_types.h"

/// Forward decl
/// @{
class Event;
/// @}

/**
 * @class GenericPlatformProcess generic/generic_platform_process.h
 * @brief Platform-agnostic process utility functions
 */
struct GenericPlatformProcess
{
private:
	/// @brief Create a new sync event
	static Event * createEvent();
	
public:
	/// @brief Returns an event from the pool or creates a new one
	static Event * getEvent();

	/// @brief Releases an event, making it available for late calls
	static void releaseEvent(Event * event);
};

