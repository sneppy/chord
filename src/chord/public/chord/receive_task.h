#pragma once

#include "hal/runnable.h"

#include "chord_fwd.h"

namespace Chord
{
	/**
	 * @class ReceiveTask chord/receive_task.h
	 * 
	 * Receives and process incoming messages in a separate thread
	 */
	class ReceiveTask : public Runnable
	{
	protected:
		/// Local node that owns this task
		LocalNode * node;

	public:
		/// Default constructor
		ReceiveTask(LocalNode * _node);

		//////////////////////////////////////////////////
		// Runnable interface
		//////////////////////////////////////////////////
		
		/// @copydoc Runnable::init
		virtual bool init() override;

		/// @copydoc Runnable::run
		virtual int32 run() override;
	};
} // namespace Chord
