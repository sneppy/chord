#pragma once

#include "generic/generic_platform_atomics.h"
#include "templates/enable_if.h"
#include "templates/is_integral.h"

/**
 * @struct UnixPlatformAtomics unix/unix_platform_atomics
 * @brief Unix implementation of atomic operations
 * 
 * We use GCC atomic builtins on unix, easy
 */
using PlatformAtomics = struct UnixPlatformAtomics
{
	template<typename Int>
	static FORCE_INLINE typename EnableIf<IsIntegral<Int>::value, Int>::Type increment(volatile Int * val)
	{
		return __sync_fetch_and_add(val, 1);
	}

	template<typename Int>
	static FORCE_INLINE typename EnableIf<IsIntegral<Int>::value, Int>::Type decrement(volatile Int * val)
	{
		return __sync_fetch_and_sub(val, 1);
	}
	
	template<typename Int, typename T>
	static FORCE_INLINE typename EnableIf<IsIntegral<Int>::value & IsIntegral<T>::value, Int>::Type add(volatile Int * val, T offset)
	{
		return __sync_fetch_and_add(val, offset);
	}

	template<typename Int, typename T>
	static FORCE_INLINE typename EnableIf<IsIntegral<Int>::value & IsIntegral<T>::value, Int>::Type exchange(volatile Int * val, T exchange)
	{
		return __sync_lock_test_and_set(val, exchange);
	}
	
	template<typename Int>
	static FORCE_INLINE typename EnableIf<IsIntegral<Int>::value, Int>::Type read(volatile const Int * src)
	{
		Int out;
		__atomic_load((volatile Int*)(src), &out, __ATOMIC_SEQ_CST);
		return out;
	}

	template<typename Int>
	static FORCE_INLINE typename EnableIf<IsIntegral<Int>::value, Int>::Type readRelaxed(volatile const Int * src)
	{
		Int out;
		__atomic_load((volatile Int*)(src), &out, __ATOMIC_RELAXED);
		return out;
	}

	template<typename Int, typename T = Int>
	static FORCE_INLINE void store(volatile typename EnableIf<IsIntegral<Int>::value & IsIntegral<T>::value, Int>::Type * src, Int val)
	{
		__atomic_store((volatile Int*)src, &val, __ATOMIC_SEQ_CST);
	}

	template<typename Int, typename T = Int>
	static FORCE_INLINE void storeRelaxed(volatile typename EnableIf<IsIntegral<Int>::value & IsIntegral<T>::value, Int>::Type * src, Int val)
	{
		__atomic_store((volatile Int*)src, &val, __ATOMIC_RELAXED);
	}
};

