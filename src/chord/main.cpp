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

class Receiver : public Runnable
{
	Net::SocketStream client;

public:
	Receiver(Net::SocketStream && socket)
		: client{move(socket)} {}

	virtual bool init() override { return true; }
	virtual int32 run() override
	{
		char line[1024] = {};
		while (client.read(line, 1024) > 0)
			printf("%s -> %s\n", *Net::getIpString(client.getAddress()), line);
	}
};

int32 main(int32 argc, char ** argv)
{
	//////////////////////////////////////////////////
	// Application setup
	//////////////////////////////////////////////////

	// Create globals	
	Memory::createGMalloc();
	gThreadManager = new ThreadManager();
	gCommandLine = new CommandLine(argc, argv);
	
	Chord::LocalNode localNode;

	Net::Ipv4 peer;
	if (CommandLine::get().getValue("input", peer, [](const String & str, Net::Ipv4 & peer){

		Net::parseIpString(peer, *str);
	})) localNode.join(peer);

	auto receiver = RunnableThread::create(new Chord::ReceiveTask(&localNode), "Receiver");
	auto updater = RunnableThread::create(new Chord::UpdateTask(&localNode), "Updater");
	auto listener = RunnableThread::create(new Chord::ListenTask(&localNode), "Listener");

	char line[256] = {};
	char c; do
	{
		switch (c = getc(stdin))
		{
		case 'p':
		{
			localNode.printInfo();
			break;
		}

		case 'l':
		{
			uint32 key;
			scanf("%x", &key);

			auto peer = localNode.lookup(key);
			printf("RESULT: found key 0x%08x @ [%s]\n", key, *peer.get().getInfoString());
			break;
		}
		
		case 'q':
		{
			localNode.leave();
			break;
		}

		default:
			break;
		}
	} while(c != 'q');

	delete[] line;

	return 0;
}