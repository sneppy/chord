#pragma once

#include "async/async.h"

#include "chord_fwd.h"
#include "types.h"
#include "request.h"
#include "math/uuid_generator.h"

namespace Chord
{
	/**
	 * @class LocalNode chord/local_node.h
	 * 
	 * A chord node running on the local machine
	 */
	class LocalNode
	{
		friend ReceiveTask;
		friend UpdateTask;

	protected:
		union
		{
			/// Local node info
			NodeInfo self;

			/// Local node id
			uint32 id;
		};

		union
		{
			/// Finger table
			/// Fingers with the same id of the local
			/// node are not valid
			NodeInfo fingers[32];

			/// Successor, a.k.a. first finger
			NodeInfo successor;
		};

		/// Predecessor node
		NodeInfo predecessor;

		/// Node UDP socket
		SocketDgram socket;

		/// Request id generator
		UUIdGenerator<uint16> requestIdGenerator;

		/// Request map
		Map<uint16, RequestCallback> callbacks;

		/// The index of the finger we'll update
		uint32 nextFinger;
	
	public:
		/// Default constructor
		LocalNode();
		
		/// Get node public address
		FORCE_INLINE const Ipv4 & getPublicAddress() const
		{
			return self.addr;
		}

	protected:
		/// Node initialization
		bool init();
	
	protected:
		/**
		 * Find successor for key in finger table
		 * 
		 * @param [in] key key to lookup
		 * @return successor node
		 */
		const NodeInfo & findSuccessor(uint32 key) const;

		/**
		 * Forge a request spawning from this node
		 * 
		 * @param [in] type request type
		 * @param [in] recipient request target
		 * @return forged request
		 */
		FORCE_INLINE Request makeRequest(Request::Type type, Ipv4 recipient, RequestCallback::CallbackT onSuccess = nullptr, uint32 ttl = (uint32)-1)
		{
			Request out{type};
			out.sender = self.addr;
			out.recipient = recipient; // !
			out.flags = 0;
			out.ttl = ttl;
			out.hopCount = 0;

			// Assign unique id
			out.id = requestIdGenerator.getNext();

			// Insert callback
			if (onSuccess) callbacks.insert(out.id, RequestCallback(onSuccess));

			return out;
		}

	public:
		//////////////////////////////////////////////////
		// Chord API
		//////////////////////////////////////////////////

		/**
		 * Join chord ring (blocking operation)
		 * 
		 * @param [in] peer address of a known peer
		 * @return join status
		 */
		bool join(const Ipv4 & peer);

		/**
		 * Look up key in chord ring
		 * 
		 * @param [in] key key to lookup
		 * @return future successor info
		 */
		Promise<NodeInfo> lookup(uint32 key);

	protected:
		/**
		 * Process incoming requests
		 * 
		 * @param [in] req incoming request
		 */
		void handleRequest(const Request & req);

		/**
		 * Process reply request
		 * 
		 * @param [in] req incoming reply request
		 */
		void handleReply(const Request & req);
		
		/**
		 * Process lookup request
		 * 
		 * @param [in] req incoming lookup request
		 */
		void handleLookup(const Request & req);

		/**
		 * Update node fingers
		 */
		void update();

	public:
		/// Returns a string representation of the node
		String getInfoString() const
		{
			char info[1024] = {};
			char *buffer = info;
			uint32 s = sizeof(info), n = 0;

			s -= (n = snprintf(buffer += n, s, "// -----------Node info------------\n"));
			s -= (n = snprintf(buffer += n, s, "// self | %s\n", *self.getInfoString()));
			s -= (n = snprintf(buffer += n, s, "// pred | %s\n", predecessor.id == id ? "NIL" : *predecessor.getInfoString()));

			for (uint32 i = 0; i < 32; ++i)
					s -= (n = snprintf(buffer += n, s, "// f#%02u | %s\n", i, fingers[i].id == id ? "NIL" : *fingers[i].getInfoString()));
			
			s -= (n = snprintf(buffer += n, s, "// --------------------------------\n"));

			return String(info);
		}

	protected:
		/**
		 * Returns whether number falls in range
		 * 
		 * @param [in] n test variable
		 * @param [in] a,b range delimiters
		 * @return true if test variables falls in range
		 * @{
		 */
		template<typename T>
		static FORCE_INLINE bool rangeOpen(T n, T a, T b)
		{
			return	(a < b && (n > a && n < b)) ||
					(a > b && (n > a || n < b));
		}
		
		template<typename T>
		static FORCE_INLINE bool rangeClosed(T n, T a, T b)
		{
			return	(a < b && (n >= a && n <= b)) ||
					(a > b && (n >= a || n <= b));
		}
		
		template<typename T>
		static FORCE_INLINE bool rangeOpenClosed(T n, T a, T b)
		{
			return	(a < b && (n > a && n <= b)) ||
					(a > b && (n > a || n <= b));
		}
		
		template<typename T>
		static FORCE_INLINE bool rangeClosedOpen(T n, T a, T b)
		{
			return	(a < b && (n >= a && n < b)) ||
					(a > b && (n >= a || n < b));
		}
		/// @}
	};
} // namespace Chord
