#pragma once

#include "core_types.h"

/**
 * @struct UnixPlatformMemory unix/unix_platform_memory.h
 */
struct UnixPlatformMemory : public GenericPlatformMemory
{
	// Empty
};
typedef UnixPlatformMemory PlatformMemory;

