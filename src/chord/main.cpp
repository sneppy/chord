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

int32 main(int32 argc, char ** argv)
{
	//////////////////////////////////////////////////
	// Application setup
	//////////////////////////////////////////////////

	// Create globals	
	Memory::createGMalloc();
	gThreadManager = new ThreadManager();
	gCommandLine = new CommandLine(argc, argv);

	return 0;
}