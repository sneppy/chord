#include "chord/client.h"
#include "crypto/sha1.h"

namespace Chord
{
	bool Client::doLookup(uint32 key, NodeInfo & target)
	{
		// Send lookup request
		uint32 request[] = {0, key}; // 0 = lookup
		if (serverSocket.write(request, sizeof(request)) == sizeof(request))
			// Sync wait for reply
			return serverSocket.read(target);
		
		return false;
	}

	void Client::doUpdate()
	{
		char line[256] = {};

		// Get filename
		fprintf(stdout, "filename: ");
		fgets(line, 256, stdin);
		strtok(line, "\n");
		String filename(line);

		Array<ubyte> content;
		if (filename == "-")
		{
			// Read stdin
			char c; while ((c = getc(stdin)) != EOF) content.add(c);
		}
		else
		{
			// Read file
			FILE * fp = fopen(*filename, "r");
			if (!fp)
			{
				fprintf(stderr, "ERROR: file %s not found\n", *filename);
				return;
			}

			// Get file length
			sizet len;
			fseek(fp, 0, SEEK_END);
			len = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			// Read data
			content.emplace(len);
			fread(*content, 1, len, fp);

			// Close file
			fclose(fp);
		}

		// Get data key
		fprintf(stdout, "name [%s]: ", *filename);
		fgets(line, 256, stdin);
		strtok(line, "\n");
		String name(line);

		// Generate hashed key
		uint32 key;
		{
			uint32 hash[5];
			Crypto::sha1(name, hash);
			key = hash[0];
		}
		
		// Lookup key in ring
		NodeInfo target;
		if (doLookup(key, target))
		{
			SocketStream * targetSocket = &serverSocket;
			bool bNeedClose = false;

			if (target.addr != serverEndpoint)
			{
				// Target is elsewhere in the network

				targetSocket = new SocketStream;

				// Open connection
				if (targetSocket->init())
					bNeedClose = targetSocket->connect(target.addr);
				else
				{
					delete targetSocket;
					targetSocket = nullptr;
				}
			}

			if (targetSocket)
			{
				// Begin upload command
				targetSocket->write<uint32>(1); // 1 = upload
				targetSocket->write<String>(name);

				// Upload data
				targetSocket->write(content);

				// TODO: Wait for response?

				// Dispose temporary socket resource
				if (bNeedClose) delete targetSocket;
			}
		}
		else
			fprintf(stderr, "ERROR: could not find target for name %s with key %u\n", *name, key);
	}

	void Client::doRetrieve()
	{
		char line[256] = {};

		// Get name
		fprintf(stdout, "name: ");
		fgets(line, 256, stdin);
		strtok(line, "\n");
		String name(line);

		// Get destination filename
		fprintf(stdout, "dest [%s]: ", *name);
		fgets(line, 256, stdin);
		strtok(line, "\n");
		String filename(line);
		
		// Get hashed key
		uint32 key;
		{
			uint32 hash[5];
			Crypto::sha1(name, hash);
			key = hash[0];
		}

		// Lookup key in ring
		NodeInfo target;
		if (doLookup(key, target))
		{
			SocketStream * targetSocket = &serverSocket;
			bool bNeedClose = false;

			if (target.addr != serverEndpoint)
			{
				// Target is elsewhere in the network

				targetSocket = new SocketStream;

				// Open connection
				if (targetSocket->init())
					bNeedClose = targetSocket->connect(target.addr);
				else
				{
					delete targetSocket;
					targetSocket = nullptr;
				}
			}

			if (targetSocket)
			{
				// Begin retrieve command
				targetSocket->write<uint32>(2); // 2 = retrieve
				targetSocket->write<String>(name);

				// Download data
				{
					Array<char> data;
					targetSocket->read(data);

					if (data.getCount() > 0)
					{
						if (filename == "-")
						{
							// Write to stdout
							fwrite(*data, 1, data.getCount(), stdout);
						}
						else
						{
							// Write to file (overwrite)
							FILE * fp = fopen(*filename, "w");
							if (!fp)
							{
								fprintf(stderr, "ERROR: could not create or open file %s\n", *filename);
								return;
							}

							// Write and finalize
							fwrite(*data, 1, data.getCount(), fp);
							fclose(fp);
						}
					}
				}

				// TODO: Wait for response?

				// Dispose temporary socket resource
				if (bNeedClose) delete targetSocket;
			}
		}
		else
			fprintf(stderr, "ERROR: could not find target for name %s with key %u\n", *name, key);
	}

	bool Client::init()
	{
		// Init socket resource and connect to server
		return serverSocket.init() && serverSocket.connect(serverEndpoint);
	}

	int32 Client::run()
	{
		bool bLocalRunning = true;
		ubyte cmd;

		while (bLocalRunning)
		{
			char line[256] = {};

			fprintf(stdout, "> (h for help) ");
			fgets(line, 256, stdin);
			sscanf(line, " %c", &cmd);

			switch (cmd)
			{
			case 'h':
				// TODO: Print help dialog
				break;

			case 'q':
				// Quit client
				fprintf(stdout, "LOG: bye bye!\n");
				bLocalRunning = false;
				break;
			
			case 'u':
				// Do insert/update
				doUpdate();
				break;
			
			case 'd':
				// Do retrieve
				doRetrieve();
				break;
			
			case '\t':
			case '\n':
			case '\r':
			case ' ':
				// Ignore whitespaces
				break;
			
			default:
				fprintf(stderr, "ERROR: command '%c' not recognized\n", cmd);
				break;
			}
		}

		return 0;
	}
} // namespace Chord
