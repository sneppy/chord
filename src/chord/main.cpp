#include "coremin.h"
#include "math/math.h"
#include "hal/threading.h"
#include "misc/command_line.h"
#include "chord/chord.h"

/// The global allocator used by default
Malloc * gMalloc = nullptr;

/// Thread manager
ThreadManager * gThreadManager = nullptr;

/// Global argument parser
CommandLine * gCommandLine = nullptr;

//////////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////////

/**
 * Create server
 */
static FORCE_INLINE int32 createServer()
{
	using namespace Chord;
	using namespace Net;

	LocalNode * localNode = new LocalNode;

	Ipv4 peer;
	if (CommandLine::get().getValue("input", peer, [](const String & str, Net::Ipv4 & peer) {

		Net::parseIpString(peer, *str);
	}))
		localNode->join(peer); // Join existing ring
	else
		; // Create new ring

	// Bootstrap tasks
	auto receiver = RunnableThread::create(new ReceiveTask(localNode), "Listener");
	auto updater = RunnableThread::create(new UpdateTask(localNode), "Listener");
	auto listener = RunnableThread::create(new ListenTask(localNode), "Listener");

#if BUILD_DEBUG
	char line[256] = {};
	char c; do
	{
		switch (c = getc(stdin))
		{
		case 'q':
			// Leave gracefully
			localNode->leave();
			break;

		case 'p':
			// Print info about the local node
			localNode->printInfo();
			break;

		case 'l':
		{
			// Retrieve key owner
			uint32 key;

			fprintf(stdout, "> ");
			fscanf(stdin, "%u", &key);

			auto peer = localNode->lookup(key);
			printf("RESULT: found key 0x%08x @ [%s]\n", key, *peer.get().getInfoString());

			break;
		}
		
		default:
			break;
		}
	} while (c != 'q');
#endif

	// Disconnect clients
	delete listener;

	// Disconnect from network
	localNode->leave();

	// Shutdown tasks
	delete updater;
	delete receiver;

	delete localNode;

	return 0;
}

/**
 * Create client
 */
static FORCE_INLINE int32 createClient()
{
	using namespace Chord;
	using namespace Net;

	Ipv4 peer;
	if (CommandLine::get().getValue("input", peer, [](const String & str, Net::Ipv4 & peer) {

		Net::parseIpString(peer, *str);
	}))
	{
		auto client = RunnableThread::create(new Client(peer), "Client");
		if (client) delete client;
	}
	else
		fprintf(stderr, "usage: chord server_endpoint --client\n");
}

int32 main(int32 argc, char ** argv)
{
	//////////////////////////////////////////////////
	// Application setup
	//////////////////////////////////////////////////

	// Create globals	
	Memory::createGMalloc();
	gThreadManager = new ThreadManager();
	gCommandLine = new CommandLine(argc, argv);
	
	if (CommandLine::get().getValue("client"))
		createClient();
	else
		createServer();

	return 0;
}