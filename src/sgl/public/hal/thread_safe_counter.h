#pragma once

#include "core_types.h"
#include "templates/is_integral.h"
#include "templates/signed.h"
#include "hal/platform_atomics.h"

template<typename Int = int32>
class ThreadSafeCounter
{
	// Allow only for integer types
	static_assert(IsIntegralV(Int), "Safe count type must be integer type");

protected:
	/// @brief Underlying counter variable
	volatile Int counter;

public:
	/// @brief Default-constructor, init to zero
	FORCE_INLINE ThreadSafeCounter() : counter(0) {}

	/// @brief Copy-constructor, no sync guarantees
	FORCE_INLINE ThreadSafeCounter(const ThreadSafeCounter<Int> & other) { counter = other.counter; }

	/// @brief Value-constructor
	FORCE_INLINE ThreadSafeCounter(Int val) : counter(val) {}

	/// @brief Assignment-operator, no sync guarantees
	FORCE_INLINE ThreadSafeCounter & operator=(const ThreadSafeCounter<Int> & other) { counter = other.counter; }

	/// @brief Returns current value
	FORCE_INLINE Int get() { return counter; }

	/// @brief Increment and return new value
	FORCE_INLINE Int increment() { return PlatformAtomics::increment(&counter) + 1; }

	/// @brief Decrement and return new value
	FORCE_INLINE Int decrement() { return PlatformAtomics::decrement(&counter) - 1; }

	/// @brief Increment and return previous value
	FORCE_INLINE Int postIncrement() { return PlatformAtomics::increment(&counter); }

	/// @brief Increment and return previous value
	FORCE_INLINE Int postDecrement() { return PlatformAtomics::decrement(&counter); }

	/// @brief Add to counter
	template<typename T = Int>
	FORCE_INLINE Int add(T val) { return PlatformAtomics::add(&counter, val); }

	/// @brief Subtract from counter
	template<typename T = SignedT(Int)>
	FORCE_INLINE Int subtract(T val) { return PlatformAtomics::add(&counter, -val); }
};

/// @brief Type definitions for common used counter types
/// @{
typedef ThreadSafeCounter<int32> ThreadSafeCounter32;
typedef ThreadSafeCounter<int64> ThreadSafeCounter64;
typedef ThreadSafeCounter<uint32> ThreadSafeCounterU32;
typedef ThreadSafeCounter<uint64> ThreadSafeCounterU64;
/// @}

