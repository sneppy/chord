#include "generic/generic_platform_memory.h"
#include "hal/malloc_ansi.h"

Malloc * GenericPlatformMemory::baseMalloc()
{
	return new MallocAnsi;
}