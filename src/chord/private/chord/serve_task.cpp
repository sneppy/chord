#include "misc/log.h"
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

		LOG(LOG, "found key #%u @ %s\n", key, *result.getInfoString());
	}

	void ServeTask::upload()
	{
		// Read filename
		String filename;
		client.read(filename);

		// Check hash of file
		/* {
			uint32 hash[5];
			Crypto::sha1(filename, hash);
			// TODO
		} */

		// Read content
		// ! Note: if request exceeds a maximum
		// ! size, server could crash. Implement
		// ! a safety check (e.g. 512 MB max size)
		Array<ubyte> data;
		client.read(data);

		LOG(LOG, "client @ %s wants to upload '%s' (%.2f KB)\n", *getIpString(client.getAddress()), *filename, data.getBytes() / 1024.f);

		// TODO: file manager
		{
			const String path = String("data/") + filename;
			FILE * fp = fopen(*path, "wb");

			if (fp)
			{
				fwrite(*data, 1, data.getBytes(), fp);
				fclose(fp);
			}
		}
	}

	void ServeTask::retrieve()
	{
		// Retrieve filename from client
		String filename;
		client.read(filename);

		LOG(LOG, "client @ %s requested file '%s'\n", *getIpString(client.getAddress()), *filename);

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
			else
			{
				Array<ubyte> emptyPayload(0);
				client.write(emptyPayload);
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
		// Send own id to client
		client.write(node->id);

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
					upload();
					break;
				
				case 2:
					retrieve();
					break;

				case 0xffffffff:
					// Terminate task
					bRunning = false;
					break;
				
				default:
					ERROR(WARNING, "unkown command received from client @ %s\n", *getIpString(client.getAddress()));
					break;
				}
			}
		}

		printf("INFO: terminating connection with client @ %s\n", *getIpString(client.getAddress()));

		return 0;
	}
} // namespace Chord
