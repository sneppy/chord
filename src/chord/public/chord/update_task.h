#pragma once

#include "hal/runnable.h"

#include "misc/time.h"
#include "chord_fwd.h"

namespace Chord
{
	/**
	 * @class ReceiveTask chord/receive_task.h
	 * 
	 * Receives and process incoming messages in a separate thread
	 */
	class UpdateTask : public Runnable
	{
	protected:
		/// Local node that owns this task
		LocalNode * node;

		/// Update timer
		Timer updateTimer;

		/// Check timer
		Timer checkTimer;

	public:
		/// Default constructor
		UpdateTask(LocalNode * _node);

		//////////////////////////////////////////////////
		// Runnable interface
		//////////////////////////////////////////////////
		
		/// @copydoc Runnable::init
		virtual bool init() override;

		/// @copydoc Runnable::run
		virtual int32 run() override;
	};
} // namespace Chord
