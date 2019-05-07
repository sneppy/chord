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
			// TODO: not from socket
			self.addr = socket.getAddress();

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
			if (fingers[i].id != id) return fingers[i];
		
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

	void LocalNode::handleRequest(const Request & req)
	{
		switch (req.type)
		{
	#if BUILD_DEBUG
		case Request::PING:
			printf("LOG: received PING from %s\n", *getIpString(req.sender));
			break;
	#endif

		case Request::REPLY:
			printf("LOG: received REPLY from %s\n", *getIpString(req.sender));
			break;

		case Request::LOOKUP:
			printf("LOG: received LOOKUP from %s\n", *getIpString(req.sender));
			handleLookup(req);
			break;

		case Request::LEAVE:
			printf("LOG: received LEAVE from %s\n", *getIpString(req.sender));
			break;
		
		default:
			break;
		}
	}

	void LocalNode::handleLookup(const Request & req)
	{
		const NodeInfo & src = req.getSrc<NodeInfo>();
		const uint32 key = req.getDst<uint32>();

		// If successor is succ(key) or successor is null
		if (rangeClosedOpen(key, id, successor.id) || successor.id == id)
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

			// Forward request
			Request res{req};
			res.sender	= self.addr;
			res.recipient = next.addr;
			
			socket.write<Request>(res, res.recipient);
		}
	}

	void LocalNode::update()
	{
		// Update next finger
		const NodeInfo & next = findSuccessor(nextFinger);

		Request req = makeRequest(
			Request::LOOKUP,
			next.addr
		);
		req.setSrc<NodeInfo>(self);
		req.setDst<uint32>(nextFinger);

		// Send lookup request
		socket.write<Request>(req, req.recipient);

		// Next finger
		nextFinger = ++nextFinger == 32 ? 1U : nextFinger;
	}
} // namespace Chord
