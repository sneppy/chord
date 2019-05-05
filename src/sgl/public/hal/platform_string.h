#pragma once

#include "core_types.h"
#include "generic/generic_platform_string.h"

#if PLATFORM_WINDOWS
	#include "windows/windows_platform_string.h"
#elif PLATFORM_APPLE
	#include "apple/apple_platform_string.h"
#elif PLATFORM_UNIX
	#include "unix/unix_platform_string.h"
#else
	#error "Unkown platform"
#endif

