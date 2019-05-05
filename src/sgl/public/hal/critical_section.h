#pragma once

#include "core_types.h"

#if PLATFORM_WINDOWS
	#include "windows/windows_critical_section.h"
#elif PLATFORM_APPLE
	#include "apple/apple_critical_section.h"
#elif PLATFORM_UNIX
	#include "unix/unix_critical_section.h"
#else
	#error "Unkown platform"
#endif

/**
 * @class ScopeLock hal/critical_section.h
 * @brief Automatically locks and unlock a critical section inside a scope
 */
class ScopeLock
{
protected:
	/// @brief Lock object
	CriticalSection * criticalSection;

public:
	/// @brief Default-constructor, removed
	ScopeLock() = delete;

	/// @brief Copy-constructor, removed
	ScopeLock(const ScopeLock&) = delete;

	/// @brief Default-constructor, acquires lock
	FORCE_INLINE ScopeLock(CriticalSection * _criticalSection) : criticalSection(_criticalSection) { criticalSection->lock(); }

	/// @brief Destructor, releases lock
	FORCE_INLINE ~ScopeLock() { criticalSection->unlock(); }

	/// @brief Assignment operator, removed
	ScopeLock & operator=(const ScopeLock&) = delete;
};

