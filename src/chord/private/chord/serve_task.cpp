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
		uint32 key; client.read(key);
		auto result = node->lookup(key).get();
		client.write<NodeInfo>(result);

		printf("LOG: found key #%u @ %s\n", key, *result.getInfoString());
	}

	void ServeTask::retrieve()
	{
		// Retrieve filename from client
		String filename;
		client.read(filename);

		printf("LOG: client @ %s requested file '%s'\n", *getIpString(client.getAddress()), *filename);

		// TODO: file manager
		{
			const String path = String("data/") + filename;
			FILE * fp = fopen(*path, "rb");

			if (fp)
			{
				fseek(fp, 0, SEEK_END);
				uint64 len = ftell(fp);
				fseek(fp, 0, SEEK_SET);

				Array<ubyte> payload(len, len);
				fread(*payload, 1, len, fp);

				fclose(fp);

				client.write(payload);
			}
		}
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
					retrieve();
					break;
				
				default:
					printf("LOG: unkown command received from client @ %s\n", *getIpString(client.getAddress()));
					break;
				}
			}
		}
	}
} // namespace Chord
