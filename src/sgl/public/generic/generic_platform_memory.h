#pragma once

#include "core_types.h"
#include <string.h>
#include "templates/enable_if.h"
#include "templates/is_pointer.h"

/**
 * @struct GenericPlatformMemory generic/generic_platform_memory.h
 */
struct GenericPlatformMemory
{
	/**
	 * @brief Align memory pointer
	 * 
	 * @param [in]	ptr			memory pointer
	 * @param [in]	alignment	required alignment
	 */
	template<typename T = void*>
	static FORCE_INLINE T align(T ptr, sizet alignment)
	{
		// T must be a pointer type
		static_assert(IsPointerV(T), "Cannot align non-pointer type");

		// Cast to integer to ease arithmetic operations
		sizet original = reinterpret_cast<sizet>(ptr);

		// Align up
		const sizet a = alignment - 1;
		return original & a ? reinterpret_cast<T>((original | a) + 1) : ptr;
	}

	/// @brief Memory utilities wrappers
	/// @{
	static FORCE_INLINE void *	memmove(void * dest, const void * src, sizet size)	{ return ::memmove(dest, src, size); }
	static FORCE_INLINE int32	memcmp(const void * a, const void * b, sizet size)	{ return ::memcmp(a, b, size); }
	static FORCE_INLINE void *	memcpy(void * dest, const void * src, sizet size)	{ return ::memcpy(dest, src, size); }
	static FORCE_INLINE void *	memset(void * dest, int32 val, sizet size)			{ return ::memset(dest, val, size); }
	static FORCE_INLINE void *	memzero(void * dest, void * src, sizet size)		{ return ::memmove(dest, src, size); }
	/// @}

private:
	/// @brief Swap two generic values
	template<typename T>
	static FORCE_INLINE void swapValues(T & a, T & b)
	{
		T t = a;
		a = b, b = t;
	}

	/// @brief Swap two big memory chunks
	static FORCE_INLINE void swapMemoryChunks(void * RESTRICT mem1, void * RESTRICT mem2, sizet size)
	{
		// We don't want to allocate an entire new memory chunk.
		// We rather iterate over the memory bytes, and swap them
		// one by one

		union
		{
			void * _void;
			ubyte * _byte;
			uint16 * _dbyte;
			uint32 * _word;
			uint64 * _dword;
		} a = {mem1}, b = {mem2};

		// Swap 8 at once
		for (; size >= 8; size -= 8)
			swapValues(*a._dword++, *b._dword++);
		
		// Swap 4 at once
		for (; size >= 4; size -= 4)
			swapValues(*a._word++, *b._word++);
		
		// Swap 2 at once
		for (; size >= 2; size -= 2)
			swapValues(*a._dbyte++, *b._dbyte++);
		
		// Swap remaining byte
		if (size > 0)
			swapValues(*a._byte++, *b._byte++);
	}

public:
	/// @brief Swap two memory regions of fixed size
	static FORCE_INLINE void memswap(void * mem1, void * mem2, sizet size)
	{
		switch (size)
		{
			case 0:
				/// Nothing to swap
				break;
			
			case 1:
				swapValues(*(ubyte*)mem1, *(ubyte*)mem2);
				break;
			
			case 2:
				swapValues(*(uint16*)mem1, *(uint16*)mem2);
				break;
			
			case 4:
				swapValues(*(uint32*)mem1, *(uint32*)mem2);
				break;
			
			case 8:
				swapValues(*(uint64*)mem1, *(uint64*)mem2);
				break;
			
			case 16:
				swapValues(*((uint64*&)mem1)++, *((uint64*&)mem2)++);
				swapValues(*(uint64*)mem1, *(uint64*)mem2);
				break;
			
			default:
				swapMemoryChunks(mem1, mem2, size);
				break;
		}
	}

	/// @brief Return the default allocator
	static class Malloc * baseMalloc();
};

