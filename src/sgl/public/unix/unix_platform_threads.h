#pragma once

#include "unix_system_includes.h"
#include "generic/generic_platform_threads.h"
#include "templates/enable_if.h"

/**
 * @struct UnixPlatformTLS unix/unix_platform_threads.h
 * @brief Unix implementation of TLS
 */
struct UnixPlatformTLS : public GenericPlatformTLS
{
	/// @brief Returns current thread id
	static FORCE_INLINE uint64 getCurrentThreadId()
	{
		static_assert(sizeof(uint64) == sizeof(pthread_t), "pthread_t cannot be cast to uint64");
		return static_cast<uint64>(pthread_self());
	}

	/// @brief Allocate a TLS slot
	static FORCE_INLINE uint32 allocateSlot()
	{
		// Create tls key
		pthread_key_t key = 0;
		if (pthread_key_create(&key, nullptr) != 0) return static_cast<uint32>(-1);

		// -1 is invalid
		if (UNLIKELY(static_cast<uint32>(key) == static_cast<uint32>(-1)))
		{
			// Create a new key
			pthread_key_t _key = 0;
			if (pthread_key_create(&_key, nullptr) != 0)
			{
				pthread_key_delete(key);
				return static_cast<uint32>(-1);
			}
			
			key = _key;
		}

		return key;
	}

	/**
	 * @brief Store a value
	 * 
	 * @param [in]	slot	slot index
	 * @param [in]	val		pointer to the value
	 */
	static FORCE_INLINE void setValue(uint32 slot, void * val) { pthread_setspecific(static_cast<pthread_key_t>(slot), val); }

	/**
	 * @brief Get a value
	 * 
	 * @param [in]	slot	slot index
	 * 
	 * @return retrieved value
	 */
	static FORCE_INLINE void * getValue(uint32 slot)
	{
		return pthread_getspecific(static_cast<pthread_key_t>(slot));
	}

	/// @brief Clears slot
	static FORCE_INLINE void clear(uint32 slot)
	{
		pthread_key_delete(static_cast<pthread_key_t>(slot));
	}
};

typedef UnixPlatformTLS PlatformTLS;

