#include "coremin.h"
#include "math/math.h"
#include "hal/threading.h"
#include "misc/command_line.h"

/// The global allocator used by default
Malloc * gMalloc = nullptr;

/// Thread manager
ThreadManager * gThreadManager = nullptr;

/// Global argument parser
CommandLine * gCommandLine = nullptr;

#include "chord/chord.h"

template<typename T>
typename EnableIf<IsIntegral<T>::value, T>::Type powi(T b, T e)
{
	if (e > 1)
	{
		T h = powi(b, (T)(e >> 1));
		return e & 0x1 ? h * h * b : h * h;
	}
	else
		return b;
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
	
	Chord::LocalNode localNode;

	Net::SocketDgram socket;
	socket.init();
	socket.bind();

	Net::Ipv4 peer;
	if (CommandLine::get().getValue("input", peer, [](const String & str, Net::Ipv4 & peer){

		Net::parseIpString(peer, *str);
	})) localNode.join(peer);

	auto receiver = RunnableThread::create(new Chord::ReceiveTask(&localNode), "Receiver");
	auto updater = RunnableThread::create(new Chord::UpdateTask(&localNode), "Updater");

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