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

int32 main(int32 argc, char ** argv)
{
	const uint32 f = (uint32)0xc6000000 + (uint32)(1U << 31U);
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

	while(1) getc(stdin), localNode.printInfo();

	return 0;
}