#include "chord/local_node.h"
#include "crypto/sha1.h"

namespace Chord
{
	LocalNode::LocalNode()
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

			printf("I'm node %08x @ %s\n", id, *getIpString(self.addr));
			return true;
		}

		return false;
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

	const NodeInfo & LocalNode::findSuccessor(uint32 key) const
	{
		const int32 offset = key - id;

		for (uint32 i = Math::getP2Index(offset < 0 ? offset + (1ULL << 32U) : offset, 32); i > 0; ++i)
			if (fingers[i].id != id) return fingers[i];
		
		// Return successor if all other fingers failed
		return successor;
	}
} // namespace Chord
