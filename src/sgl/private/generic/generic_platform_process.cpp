#include "generic/generic_platform_process.h"
#include "hal/event.h"
#include "hal/event_pthread.h"

Event * GenericPlatformProcess::createEvent()
{
	Event * event = nullptr;

	/// Only pthread event is implemented
#if PLATFORM_USE_PTHREADS
	event = new PThreadEvent;
	if (!event->create()) // Init event
	{
		delete event;
		event = nullptr;
	}
#endif

	return event;
}

Event * GenericPlatformProcess::getEvent()
{
	/// @todo Create a thread pool to reuse existing events
	return createEvent();
}

void GenericPlatformProcess::releaseEvent(Event * event)
{
	/// @todo For now, we just delete it
	delete event;
}