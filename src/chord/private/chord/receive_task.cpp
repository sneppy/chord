#include "chord/receive_task.h"
#include "chord/local_node.h"

namespace Chord
{
	ReceiveTask::ReceiveTask(LocalNode * _node)
		: node(_node) {}
	
	bool ReceiveTask::init()
	{
		return node && node->socket.isInit();
	}

	int32 ReceiveTask::run()
	{
		const bool bRunning = true;
		Request req;

		while (bRunning)
		{
			if (node->socket.read(req, req.sender) && !req.hop().isExpired())
				// Single threaded handler
				node->handleRequest(req);
		}
	}
} // namespace Chord
