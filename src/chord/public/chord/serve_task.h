#pragma once

#include "hal/runnable.h"

#include "chord_fwd.h"

namespace Chord
{
	/**
	 * @class ServeTask chord/serve_task.h
	 * 
	 * A simple task that receives and processes
	 * client requests
	 */
	class ServeTask : public Runnable
	{
	protected:
		/// Ref to local node
		LocalNode * node;

		/// Client socket
		SocketStream client;

	public:
		/// Default constructor
		ServeTask(LocalNode * _node, SocketStream && _client);

		//////////////////////////////////////////////////
		// Service endpoints
		//////////////////////////////////////////////////
		
		/**
		 * Lookup a key in the chord ring
		 */
		void lookup();

		/**
		 * Upload a new file on this node
		 */
		void upload();

		/**
		 * Retrieve a file from this service
		 */
		void retrieve();

		//////////////////////////////////////////////////
		// Runnable interface
		//////////////////////////////////////////////////
		
		virtual bool init() override;
		virtual int32 run() override;
	};
} // namespace Chord
