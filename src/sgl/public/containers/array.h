#pragma once

#include "core_types.h"
#include "containers_fwd.h"
#include "hal/platform_crt.h"
#include "hal/platform_math.h"
#include "hal/platform_memory.h"
#include "hal/malloc_ansi.h"
#include "templates/const_ref.h"
#include "templates/is_trivially_copyable.h"

/**
 * @class Array containers/array.h
 * A dynamic array
 */
template<typename T, typename AllocT = MallocAnsi>
class GCC_ALIGN(32) Array
{
	template<typename, typename>	friend class Array;
									friend String;

public:
	/// Iterators type definitions
	using Iterator		= T*;
	using ConstIterator	= const T*;

protected:
	/// Allocator in use
	AllocT * allocator;
	bool bHasOwnAllocator;

	/// Element buffer
	T * buffer;

	/// Current size of buffer
	uint64 size;

	/// Actual number of elements
	uint64 count;

public:
	/// Default constructor
	explicit FORCE_INLINE Array(uint64 _size = 2, AllocT * _allocator = reinterpret_cast<AllocT*>(gMalloc)) :
		allocator(_allocator),
		bHasOwnAllocator(_allocator == nullptr),
		buffer(nullptr),
		size(_size ? _size : 2),
		count(0U)
	{
		// Create a default allocator
		if (allocator == nullptr)
			allocator = new AllocT;

		// Allocate initial buffer
		if (size) buffer = reinterpret_cast<T*>(allocator->malloc(size * sizeof(T)));
	}

	/// Copy constructor
	FORCE_INLINE Array(const Array<T, AllocT> & other) : Array(other.size)
	{
		// Copy content
		count = other.count;
		moveOrCopy(buffer, other.buffer, count);
	}

	/// Copy constructor with different allocator type
	template<typename AllocU>
	FORCE_INLINE Array(const Array<T, AllocU> & other) : Array(other.size)
	{
		// Copy content
		count = other.count;
		moveOrCopy(buffer, other.buffer, count);
	}

	/// Move constructor (only if same allocator type)
	FORCE_INLINE Array(Array<T, AllocT> && other) :
		allocator(other.allocator),
		bHasOwnAllocator(other.bHasOwnAllocator),
		buffer(other.buffer),
		size(other.size),
		count(other.count)
	{
		other.bHasOwnAllocator	= false;
		other.buffer			= nullptr;
	}

	/// Copy assignment
	FORCE_INLINE Array<T, AllocT> & operator=(const Array<T, AllocT> & other)
	{
		// Realloc or create new buffer
		if (buffer == nullptr)
		{
			size = other.size;
			buffer = reinterpret_cast<T*>(allocator->malloc(size * sizeof(T)));
		}
		else if (other.count > size)
		{
			allocator->free(buffer);

			size = other.size;
			buffer = reinterpret_cast<T*>(allocator->malloc(size * sizeof(T)));
		}

		// Copy content
		count = other.count;
		moveOrCopy(buffer, other.buffer, count);

		return *this;
	}

	/// Copy assignment with different allocator type
	template<typename AllocU>
	FORCE_INLINE Array<T, AllocT> & operator=(const Array<T, AllocT> & other)
	{
		// Realloc or create new buffer
		if (buffer == nullptr)
		{
			size = other.size;
			buffer = reinterpret_cast<T*>(allocator->malloc(size * sizeof(T)));
		}
		else if (other.count > size)
		{
			allocator->free(buffer);

			size = other.size;
			buffer = reinterpret_cast<T*>(allocator->malloc(size * sizeof(T)));
		}

		// Copy content
		count = other.count;
		moveOrCopy(buffer, other.buffer, count);
	}

	/// Move assignment
	FORCE_INLINE Array<T, AllocT> & operator=(Array<T, AllocT> && other)
	{
		// Free existing buffer
		if (buffer)
			allocator->free(buffer);

		// Free own allocator
		if (bHasOwnAllocator)
			delete allocator;

		allocator			= other.allocator;
		bHasOwnAllocator	= other.bHasOwnAllocator;
		buffer				= other.buffer;
		size				= other.size;
		count				= other.count;

		other.bHasOwnAllocator	= false;
		other.buffer			= nullptr; 
	}

	/// Destructor
	FORCE_INLINE ~Array()
	{
		//if (buffer)
			//allocator->free(buffer);
		
		if (bHasOwnAllocator)
			delete allocator;
	}

	/// Returns raw content
	/// @{
	FORCE_INLINE T *		operator*()			{ return buffer; }
	FORCE_INLINE const T *	operator*() const	{ return buffer; }
	/// @}

	/// STL compliant iterators
	/// @{
	FORCE_INLINE Iterator		begin()			{ return buffer; }
	FORCE_INLINE ConstIterator	begin() const	{ return buffer; }

	FORCE_INLINE Iterator		end()		{ return buffer + count; }
	FORCE_INLINE ConstIterator	end() const	{ return buffer + count; }
	/// @}

	/// Random access operator
	/// @{
	FORCE_INLINE const T &	operator[](uint64 i) const	{ return buffer[i]; }
	FORCE_INLINE T &		operator[](uint64 i)		{ return buffer[i]; }
	/// @}

	/// Returns item count
	FORCE_INLINE uint64 getCount() const { return count; }

	/// Returns array size
	FORCE_INLINE uint64 getSize() const { return size; }

	/// Returns actual buffer load in Bytes
	FORCE_INLINE uint64 getBytes() const { return count * sizeof(T); }

	/// Returns true if array is empty
	FORCE_INLINE bool isEmpty() const { return count == 0; }

	/// Returns true if array is valid
	FORCE_INLINE bool isValid() const { return buffer && size; }

	/// Force resize array
	FORCE_INLINE bool resize(uint64 _size)
	{
		if (_size != size)
		{
			// Realloc buffer
			buffer = reinterpret_cast<T*>(allocator->realloc(buffer, _size * sizeof(T)));

			size	= _size;
			count	= PlatformMath::min(count, size);

			// Buffer was resized
			return true;
		}

		return false;
	}

	/// Insert uninitialized elements
	FORCE_INLINE void emplace(uint64 _count)
	{
		resize(count = _count);
	}

protected:
	/// Resize only if new count is bigger than current size
	FORCE_INLINE bool resizeIfNecessary(uint64 _count)
	{
		if (_count > size)
		{
			uint64 _size = size * 2;
			while (_count > _size) _size *= 2;

			return resize(_size);
		}

		return false;
	}

public:
	/// Like @ref operator[] but extends the array if out of bounds
	FORCE_INLINE T & operator()(uint64 i)
	{
		resizeIfNecessary(i);
		count = PlatformMath::max(count, i);

		return buffer[i];
	}

	/**
	 * Returns a subarray
	 * 
	 * @param [in] begin,end subarray range
	 * @param [in] n subarray num of elements
	 * @{
	 */
	FORCE_INLINE Array<T> operator()(uint64 begin, uint64 end) const
	{
		const uint32 n = end - begin;
		Array out(n);

		// Copy buffer and set count
		moveOrCopy(out.buffer, buffer + begin, n);
		out.count = n;

		return out;
	}

	FORCE_INLINE Array<T> slice(uint64 begin) const
	{
		return operator()(begin, count);
	}
	FORCE_INLINE Array<T> slice(uint64 begin, uint64 end) const
	{
		return operator()(begin, end);
	}
	/// @}

	/**
	 * Insert a new item in the array
	 * 
	 * @param [in] item T operand
	 * @param [in] i target position
	 */
	FORCE_INLINE void insert(typename ConstRef<T>::Type item, uint64 i)
	{
		resizeIfNecessary(count + 1);

		// Move content up
		if (LIKELY(i < count))
			PlatformMemory::memmove(buffer + i + 1, buffer + i, (count - i) * sizeof(T));
		
		// Construct object
		moveOrCopy(buffer[i], item);
		++count;
	}

	/**
	 * Inserts multiple items in the array
	 * 
	 * @param [in] items buffer of items
	 * @param [in] n number of items
	 * @param [in] i target position
	 */
	FORCE_INLINE void insert(const T * items, uint64 n, uint64 i)
	{
		resizeIfNecessary(count + n);

		// Move content up
		if (i < count)
			PlatformMemory::memmove(buffer + i + n, buffer + i, (count - i) * sizeof(T));

		// Construct objects
		moveOrCopy(buffer + i, items, n);
		count += n;
	}

	/**
	 * Add item at the end of the array
	 * 
	 * @param [in] item T operand
	 * @{
	 */
	FORCE_INLINE void add(typename ConstRef<T>::Type item)
	{
		resizeIfNecessary(count + 1);

		// Construct object
		moveOrCopy(buffer[count], item);
		++count;
	}
	FORCE_INLINE void push(typename ConstRef<T>::Type item) { add(item); }
	/// @}

	/**
	 * Add multiple items at the end of the array
	 * 
	 * @param [in] items buffer of items
	 * @param [in] n number of items
	 * @{
	 */
	FORCE_INLINE void add(const T * items, uint64 n)
	{
		resizeIfNecessary(count + n);

		// Construct objects
		moveOrCopy(buffer + count, items, n);
		count += n;
	}
	FORCE_INLINE void push(const T * items, uint64 n) { add(items, n); }
	/// @}

	/**
	 * Remove items at position
	 * 
	 * @param [in] i item's position
	 * @param [in] n number of items to remove
	 */
	FORCE_INLINE void removeAt(uint64 i, uint64 n = 1)
	{
		// Just move back memory
		if (i < count)
			PlatformMemory::memmove(buffer + i, buffer + i + n, (count - i) * sizeof(T));
		
		count -= n;
	}

	/// Remove item at the end of the array
	FORCE_INLINE void pop(uint64 i, uint64 n)
	{		
		--count;
	}
};