#pragma once

#include "core_types.h"
#include "hal/platform_memory.h"
#include "hal/runnable_thread.h"
#include "hal/thread_manager.h"

#if PLATFORM_USE_PTHREADS

/**
 * @class RunnablePThread hal/runnable_pthread.h
 * @brief Base implementation that uses pthreads
 */
class RunnablePThread : public RunnableThread
{
protected:
	/// @brief Entry point function type
	/// 
	/// The pthread_create interface allows passing
	/// a single parameter via reference, and returns
	/// its exit value as a void pointer
	typedef void * (*PThreadEntryPoint)(void * arg);

protected:
	/// @brief Underlying pthread resource
	pthread_t thread;

	/// @brief If true the thread has started and not yet finished
	bool bStarted;

public:
	/// @brief Default-constructor
	FORCE_INLINE RunnablePThread() {}

	/// @brief Virtual-destructor
	FORCE_INLINE ~RunnablePThread() { destroy(); }

	/// @copydoc RunnableThread::kill
	virtual void kill(bool bShouldWait = true) override;

	/// @copydoc RunnableThread::join
	virtual FORCE_INLINE void join() override
	{
		if (bStarted)
		{
			pthread_join(id, nullptr);
			bStarted = false;
		}
	}

protected:
	/// @brief Platform dependant default stack size
	virtual FORCE_INLINE int32 getDefaultStackSize()
	{
		// Some information on default stack sizes, selected when given 0:
		// - On Windows, all threads get 1MB: http://msdn.microsoft.com/en-us/library/windows/desktop/ms686774(v=vs.85).aspx
		// - On Mac, main thread gets 8MB,
		// - all other threads get 512 kB when created through pthread or NSThread, and only 4kB when through MPTask()
		//	( http://developer.apple.com/library/mac/#qa/qa1419/_index.html )

		// I'm not gonna mess with this
		return 0;
	}

	/// @brief Fixed thread entry points
	/// 
	/// Forwards to appropriate methods
	static FORCE_INLINE void * entryPoint(void * _self)
	{
		assert(_self);

		// Add to thread manager
		RunnablePThread * self = reinterpret_cast<RunnablePThread*>(_self);
		ThreadManager::getPtr()->add(self->getThreadId(), self);

		// Thread life-cycle
		self->preRun(), self->run(), self->postRun();

		// Exit silently
		pthread_exit(nullptr);
		return nullptr;
	}

	/// @brief Called before running the thread (virtual, can be modified)
	virtual FORCE_INLINE void preRun() {};

	/// @brief Called before thread exits (virtual, can be modified)
	virtual FORCE_INLINE void postRun() {};

	/**
	 * @brief Thread run routine, the real entry point of the thread
	 */
	int32 run();

	/// @brief Performs object destruction
	/// 
	/// Why here? Some weird stuff about virtualization,
	/// go ask those madman at Epic
	void FORCE_INLINE destroy()
	{
		// Kill running thread
		if (bStarted) kill();
	}

protected:
	/// @copydoc RunnableThread::create_internal()
	virtual bool create_internal(Runnable * _runnable, const ansichar * _name, uint32 stackSize) override
	{
		// No runnable? No thread!
		if (_runnable == nullptr) return false;
		runnable = _runnable;

		// Set thread name
		name = _name ? _name : "unnamed";

		// Create the thread
		const int32 status = pthread_create(&thread, nullptr, entryPoint, this);
		bStarted = status == 0;

		return bStarted;
	}
};

#endif // PLATFORM_USE_PTHREADS

