#pragma once

#include "core_types.h"
#include "unix_system_includes.h"

/**
 * @struct UnixPlatformMath unix/unix_platform_math.h
 * @brief Unix specific math implementation
 */
using PlatformMath = struct UnixPlatformMath : public GenericPlatformMath
{
	// Empty
};