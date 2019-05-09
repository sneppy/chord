#include "chord/local_node.h"
#include "crypto/sha1.h"

namespace Chord
{
	LocalNode::LocalNode()
		: self{}
		, fingers{}
		, predecessor{}
		, socket{}
		, requestIdGenerator{}
		, callbacks{}
		, nextFinger{1U}
	{
		// Initialize node
		init();
	}

	bool LocalNode::init()
	{
		// Initialize socket
		if (socket.init() && socket.bind())
		{
			// Get node public address
			// TODO: depending on are visibility
			// Fallback to socket binding address
			self.addr = socket.getAddress();
			getInterfaceAddr(self.addr);

			{
				// Compute sha-1 of address
				uint32 hash[5]; Crypto::sha1(getIpString(self.addr), hash);

				// We use the first 32-bit
				// Uniform distribution
				id = hash[0];
			}

			// Init predecessor, successor and finger table
			predecessor = self;
			for (uint32 i = 0; i < 32; ++i)
				fingers[i] = self;

			printf("INFO: created node %s\n", *self.getInfoString());
			return true;
		}

		return false;
	}

	const NodeInfo & LocalNode::findSuccessor(uint32 key) const
	{
		const uint32 offset = key - id;

		for (uint32 i = Math::getP2Index(offset, 32); i > 0; --i)
			if (rangeOpen(fingers[i].id, id, key)) return fingers[i];
		
		// Return successor if all other fingers failed
		return successor;
	}

	bool LocalNode::join(const Ipv4 & peer)
	{
		// Join starts with a lookup request
		Request req = makeRequest(
			Request::LOOKUP,
			peer
		);
		req.setSrc<NodeInfo>(self);
		req.setDst<uint32>(id);

		// Send lookup request
		socket.write<Request>(req, req.recipient);

		// Wait for response
		// TODO: timeout
		Request res;
		do socket.read(res, res.sender); while (res.id != req.id);

		// Set successor
		successor = res.getDst<NodeInfo>();

		printf("INFO: connected with successor %s\n", *successor.getInfoString());
	}

	Promise<NodeInfo> LocalNode::lookup(uint32 key)
	{
		Promise<NodeInfo> out;

		if (rangeOpenClosed(key, id, successor.id))
			out.set(successor);
		else
		{
			// Find closest preceding node
			const NodeInfo & next = findSuccessor(key);

			Request req = makeRequest(
				Request::LOOKUP,
				next.addr
			);
			req.setSrc<NodeInfo>(self);
			req.setDst<uint32>(key);

			// Send lookup request
			socket.write<Request>(req, req.recipient);
		}
		
		return out;
	}

	void LocalNode::leave()
	{
		Request req{Request::LEAVE};
		req.sender = self.addr;
		req.setSrc<NodeInfo>(self);

		// Send to successor
		{
			req.recipient = successor.addr;
			socket.write<Request>(req, req.recipient);
		}

		// Send to predecessor
		{
			req.recipient = predecessor.addr;
			socket.write<Request>(req, req.recipient);
		}
	}

	void LocalNode::stabilize()
	{
		// This op is slightly different from
		// the one described in the original
		// paper, for we first notify our
		// current successor than maybe update
		// it
		Request req = makeRequest(
			Request::NOTIFY,
			successor.addr,
			[this](const Request & req) {
				
				const NodeInfo & target = req.getDst<NodeInfo>();
				if (successor.id == id || rangeOpen(target.id, id, successor.id))
				{
					// Update successor
					setSuccessor(target);

					printf("LOG: new successor is %s\n", *successor.getInfoString());
				}
			}
		);
		req.setSrc<NodeInfo>(self);

		// Send notify
		socket.write<Request>(req, req.recipient);
	}

	void LocalNode::fixFingers()
	{
		const uint32 key = id + (1U << nextFinger);
		const uint32 i = nextFinger;

		/**
		 * Here there is a piece of code
		 * copied from @ref lookup
		 */
		if (rangeOpenClosed(key, id, successor.id))
		{
			// Update finger
			setFinger(successor, i);
			
			printf("LOG: updating finger #%u with %s\n", i, *fingers[i].getInfoString());
		}
		else
		{
			// Update next finger
			const NodeInfo & next = findSuccessor(key);

			Request req = makeRequest(
				Request::LOOKUP,
				next.addr,
				[this, i](const Request & req) {

					// Update finger
					setFinger(req.getDst<NodeInfo>(), i);

					printf("LOG: updating finger #%u with %s\n", i, *fingers[i].getInfoString());
				}
			);
			req.setSrc<NodeInfo>(self);
			req.setDst<uint32>(key);

			// Send lookup request
			socket.write<Request>(req, req.recipient);
		}

		// Next finger
		nextFinger = ++nextFinger == 32U ? 1U : nextFinger;
	}

	void LocalNode::handleRequest(const Request & req)
	{
		switch (req.type)
		{
	#if BUILD_DEBUG
		case Request::PING:
			printf("LOG: received PING from %s with id 0x%08x\n", *getIpString(req.sender), req.id);
			break;
	#endif

		case Request::REPLY:
			printf("LOG: received REPLY from %s with id 0x%08x\n", *getIpString(req.sender), req.id);
			handleReply(req);
			break;

		case Request::LOOKUP:
			printf("LOG: received LOOKUP from %s with id 0x%08x and hop count = %u\n", *getIpString(req.sender), req.id, req.hopCount);
			handleLookup(req);
			break;

		case Request::NOTIFY:
			printf("LOG: received NOTIFY from %s with id 0x%08x\n", *getIpString(req.sender), req.id);
			handleNotify(req);
			break;

		case Request::LEAVE:
			printf("LOG: received LEAVE from %s with id 0x%08x\n", *getIpString(req.sender), req.id);
			handleLeave(req);
			break;
		
		default:
			break;
		}
	}

	void LocalNode::handleReply(const Request & req)
	{
		auto callbackIt = callbacks.find(req.id);
		if (callbackIt != callbacks.nil())
		{
			// Execute callback
			auto callback = callbackIt->second;
			callback.onSuccess(req);
		}
	}

	void LocalNode::handleLookup(const Request & req)
	{
		const NodeInfo & src = req.getSrc<NodeInfo>();
		const uint32 key = req.getDst<uint32>();

		// If successor is succ(key) or successor is null
		if (rangeOpenClosed(key, id, successor.id))
		{
			// Reply to source node
			Request res{req};
			res.type = Request::REPLY;
			res.sender = self.addr;
			res.recipient = src.addr;
			res.setDst<NodeInfo>(successor);
			res.reset();

			socket.write<Request>(res, res.recipient);
		}
		else
		{
			// Find closest preceding node
			const NodeInfo & next = findSuccessor(key);

			// Break infinite loop
			if (next.id == id)
			{
				// Reply to source node
				Request res{req};
				res.type = Request::REPLY;
				res.sender = self.addr;
				res.recipient = src.addr;
				res.setDst<NodeInfo>(self);
				res.reset();

				socket.write<Request>(res, res.recipient);
			}
			else
			{
				// Forward request
				Request fwd{req};
				fwd.sender	= self.addr;
				fwd.recipient = next.addr;
				
				socket.write<Request>(fwd, fwd.recipient);
			}
		}
	}

	void LocalNode::handleNotify(const Request & req)
	{
		const NodeInfo & src = req.getSrc<NodeInfo>();

		// Reply with current predecessor
		Request res{req};
		res.type = Request::REPLY;
		res.sender = self.addr;
		res.recipient = src.addr;
		res.setDst<NodeInfo>(predecessor);

		socket.write<Request>(res, res.recipient);
		
		// if predecessor is nil or n -> (predecessor, self)
		if (predecessor.id == id || rangeOpen(src.id, predecessor.id, id))
		{
			// Update predecessor
			setPredecessor(src);

			printf("LOG: new predecessor is %s\n", *predecessor.getInfoString());
		}
	}

	void LocalNode::handleLeave(const Request & req)
	{
		const NodeInfo & src = req.getSrc<NodeInfo>();

		if (src.id == predecessor.id)
			// Set predecessor to NIL
			setPredecessor(self);
		
		if (src.id == successor.id)
		{
			// Reset successor temporarily
			setSuccessor(self);

			// Do lookup on predecessor
			Request req = makeRequest(
				Request::LOOKUP,
				predecessor.addr,
				[this](const Request & req) {
					
					setSuccessor(req.getDst<NodeInfo>());

					printf("LOG: new successor is %s\n", *successor.getInfoString());
				}
			);
			req.setDst<uint32>(id + 1);

			socket.write<Request>(req, req.recipient);
		}

		for (uint32 i = 1; i < 32; ++i)
			if (src.id == fingers[i].id)
				// Unset finger
				setFinger(self, i);

		printf("LOG: node %s is leaving\n", *src.getInfoString());
	}
} // namespace Chord
