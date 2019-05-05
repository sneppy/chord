#pragma once

/// Build macros coming from CMake
/// Set to zero undefined

#ifndef BUILD_DEBUG
	#define BUILD_DEBUG 0
#endif
#ifndef BUILD_DEVELOPMENT
	#define SGL_BUILD_DEVELOPMENT 0
#endif
#ifndef BUILD_RELEASE
	#define BUILD_RELEASE 0
#endif