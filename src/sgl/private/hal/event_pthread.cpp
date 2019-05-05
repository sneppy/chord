#include "hal/event_pthread.h"

#if PLATFORM_USE_PTHREADS

#include <sys/time.h>

bool PThreadEvent::wait(uint32 waitTime)
{
#if BUILD_DEBUG
	ASSERT(bInitialized, "Event is not initialized");
#else
	if (!bInitialized) return false;
#endif

	// Save start time
	struct timeval startTime;
	gettimeofday(&startTime, nullptr);

	bool status = false;

	{
		lock();

		do
		{
			// I'm waiting guys
			++numWaiting;

			if (triggerState == TRIGGERED_ONE)
			{
				// Yo, it's mine
				triggerState = TRIGGERED_NONE;
				status = true;
			}
			else if (triggerState == TRIGGERED_ALL)
			{
				// This wakes everyone
				status = true;
			}
			else if (waitTime > 0)
			{
				if (waitTime == 0xffffffff) // Infinite wait
					// Block thread indefinetely
					pthread_cond_wait(&sync, &mutex);
				else
				{
					// Time structure required by pthread library
					// It is an exact timestamp, not a timespan
					struct timespec timeout;
					uint32 ms = (startTime.tv_usec / 1000) + waitTime;
					timeout.tv_sec	= startTime.tv_sec + (ms / 1000); // Seconds
					timeout.tv_nsec	= (ms % 1000) * 1000000; // Nanoseconds

					// And now we wait
					pthread_cond_timedwait(&sync, &mutex, &timeout);

					// Update time
					struct timeval now, diff;
					gettimeofday(&now, nullptr);
					if (now.tv_usec < startTime.tv_usec)
						diff.tv_sec		= now.tv_sec - startTime.tv_sec - 1,
						diff.tv_usec	= 1000000 + now.tv_usec - startTime.tv_usec;
					else
						diff.tv_sec		= now.tv_sec - startTime.tv_sec,
						diff.tv_usec	= now.tv_usec - startTime.tv_usec;
					
					ms = (diff.tv_sec * 1000) + (diff.tv_usec / 1000);
					waitTime	= ms > waitTime ? 0 : waitTime - ms;
					startTime	= now;
				}
			}

			// Hopefully I'm done waiting
			--numWaiting;
		} while (!status & waitTime > 0);

		unlock();
	}

	return status;
}

#endif