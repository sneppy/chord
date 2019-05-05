#include "hal/platform_memory.h"
#include "hal/platform_math.h"

void Memory::createGMalloc()
{
	// Init global malloc
	gMalloc = PlatformMemory::baseMalloc();
}