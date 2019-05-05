#include "hal/runnable_pthread.h"
#include "hal/runnable.h"

void RunnablePThread::kill(bool bShouldWait)
{
	if (LIKELY(bStarted))
	{
		// Gently stop runnable
		if (runnable) runnable->stop();

		if (bShouldWait)
			pthread_join(thread, nullptr);
		else if (bStarted)
			pthread_cancel(thread);

		bStarted = false;
	}
}

int32 RunnablePThread::run()
{
	// No runnable, no thread
	if (!runnable) return 1;

	if (runnable->init())
	{
		int32 status = runnable->run();

		// Clean up
		runnable->exit();
		return status;
	}

	// Initialization failed
	return 2;
}