#include "chord/serve_task.h"
#include "chord/local_node.h"
#include "crypto/sha1.h"

namespace Chord
{
	ServeTask::ServeTask(LocalNode * _node, SocketStream && _client)
		: node{_node}
		, client{move(_client)} {}
	
	void ServeTask::lookup()
	{
		String key("sneppy");
		client.read(key);

		// Compute key hash
		uint32 k;
		{
			uint32 hash[5];
			Crypto::sha1(key, hash);
			k = *hash;
		}

		auto result = node->lookup(k).get();
		client.write<NodeInfo>(result);

		printf("LOG: found key '%s' (with hash %u) @ %s\n", *key, k, *result.getInfoString());
	}
	
	bool ServeTask::init()
	{
		printf("INFO: client @ %s connected to service\n", *getIpString(client.getAddress()));

		return client.isInit();
	}
	
	int32 ServeTask::run()
	{
		bool bRunning = true;
		while (bRunning)
		{
			uint32 cmd; if (client.read(cmd))
			{
				switch (cmd)
				{
				case 0:
					lookup();
					break;
				
				case 1:
					printf("It's a me, Mario!\n");
					break;
				
				case 2:
					printf("kek\n");
					break;
				
				default:
					break;
				}
			}
		}
	}
} // namespace Chord
