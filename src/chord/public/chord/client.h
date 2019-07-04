#pragma once

#include "core_types.h"
#include "chord_fwd.h"
#include "hal/runnable.h"
#include "async/async.h"

namespace Chord
{
	/**
	 * Client
	 */
	class Client : public Runnable
	{
	protected:
		/// Server endpoint
		Ipv4 serverEndpoint;

		/// Server socket
		mutable SocketStream serverSocket;

	public:
		/**
		 * Default constructor
		 */
		FORCE_INLINE Client(const Ipv4 & inServerEndpoint)
			: serverEndpoint{inServerEndpoint}
		{
			//
		}

		//////////////////////////////////////////////////
		// Client interface
		//////////////////////////////////////////////////

		/**
		 * Lookup key in the chord ring
		 * 
		 * @param [in] key key to lookup
		 * @param [out] target target node
		 */
		bool doLookup(uint32 key, NodeInfo & target);
		
		/**
		 * Upload file to the network
		 */
		void doUpdate();

		/**
		 * Retrieve file from the network
		 */
		void doRetrieve();

		//////////////////////////////////////////////////
		// Runnable interface
		//////////////////////////////////////////////////
		
		virtual bool init() override;
		virtual int32 run() override;
	};
} // namespace Chord
