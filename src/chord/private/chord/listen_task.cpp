#include "chord/listen_task.h"
#include "chord/local_node.h"
#include "chord/serve_task.h"

namespace Chord
{
	ListenTask::ListenTask(LocalNode * _node)
		: node{_node}
		, socket{} {}
	
	bool ListenTask::init()
	{
		if (node)
		{
			// Get port from node
			const uint16 port = node->getPublicAddress().getPort();

			// Bind to wildcard
			socket.init();
			socket.bind(port);
		}

		return socket.isInit();
	}
	
	int32 ListenTask::run()
	{
		/// Local map
		class Ipv4Compare
		{
		public:
			/// Compare two ipv4 addresses
			FORCE_INLINE int32 operator()(const Ipv4 & a, const Ipv4 & b)
			{
				return a.compare(b);
			}
		};
		Map<Ipv4, RunnableThread*, Ipv4Compare> clients;
		
		// Start listening
		socket.listen();

		bool bRunning = true;
		while (bRunning)
		{
			SocketStream client;
			Ipv4 addr;

			if (socket.accept(client, addr))
				// TODO: remove from client map when connection is closed
				clients.insert(addr, RunnableThread::create(new ServeTask(node, move(client)), "ServeTask"));
		}

		return 0;
	}
} // namespace Chord
