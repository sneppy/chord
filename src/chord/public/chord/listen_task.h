#pragma once

#include "hal/runnable.h"

#include "chord_fwd.h"

namespace Chord
{
	/**
	 * @class ListenTask chord/listen_task.h
	 * 
	 * Listen for incoming client connections
	 */
	class ListenTask : public Runnable
	{
	protected:
		/// Ref to local node
		LocalNode * node;

		/// Client socket
		SocketStream socket;

	public:
		/// Default constructor
		ListenTask(LocalNode * _node);

		//////////////////////////////////////////////////
		// Runnable interface
		//////////////////////////////////////////////////
		
		/// @copydoc Runnable::init
		virtual bool init() override;

		/// @copydoc Runnable::run
		virtual int32 run() override;
	};
} // namespace Chord
