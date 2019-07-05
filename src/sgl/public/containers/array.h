#pragma once

#include "core_types.h"
#include "containers_fwd.h"
#include "hal/platform_crt.h"
#include "hal/platform_math.h"
#include "hal/platform_memory.h"
#include "hal/malloc_ansi.h"
#include "templates/reference.h"
#include "templates/is_trivially_copyable.h"
#include "templates/is_trivially_constructible.h"
#include "templates/is_trivially_destructible.h"

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
	/// @{
	AllocT * allocator;
	bool bHasOwnAllocator;
	/// @}

	/// Element buffer
	T * buffer;

	/// Current size of buffer
	uint64 size;

	/// Actual number of elements
	uint64 count;

protected:
	/**
	 * Default construct range of elements
	 * 
	 * @param [in] begin,end elements range
	 */
	FORCE_INLINE void constructElements(Iterator begin, Iterator end)
	{
		for (; begin != end; ++begin)
			// Construct element
			new (begin) T();
	}

	/**
	 * Copy construct range of elements
	 * 
	 * @param [in] begin,end elements range
	 * @param [in] src source iterator
	 */
	FORCE_INLINE void constructElements(Iterator begin, Iterator end, ConstIterator src)
	{
		for (; begin != end; ++begin, ++src)
			// Copy construct element
			new (begin) T(*src);
	}

	/**
	 * Move construct range of elements
	 * 
	 * @param [in] begin,end elements range
	 * @param [in] src source iterator
	 */
	FORCE_INLINE void constructElements(Iterator begin, Iterator end, Iterator src)
	{
		for (; begin != end; ++begin, ++src)
			// Move construct element
			new (begin) T(move(*src));
	}

	/**
	 * Destroy range of elements
	 * (i.e. call destructor for
	 * non-trivially destructible
	 * types)
	 * 
	 * @param [in] begin,end range of elements
	 * @{
	 */
	template<typename _T = T>
	FORCE_INLINE typename EnableIf<IsTriviallyDestructible<_T>::value, void>::Type destroyElements(ConstIterator begin, ConstIterator end)
	{
		// Nothing to do
	}
	template<typename _T = T>
	FORCE_INLINE typename EnableIf<!IsTriviallyDestructible<_T>::value, void>::Type destroyElements(ConstIterator begin, ConstIterator end)
	{
		for (; begin != end; ++begin)
			// Destroy element
			begin->~T();
	}
	/// @}

public:
	/// Default constructor
	explicit Array(uint64 _size = 2, uint64 _count = 0, AllocT * _allocator = reinterpret_cast<AllocT*>(gMalloc))
		: allocator{_allocator}
		, bHasOwnAllocator{_allocator == nullptr}
		, buffer{nullptr}
		, size{_size}
		, count{_count}
	{
		// Create a default allocator
		if (bHasOwnAllocator) allocator = new AllocT;

		// Allocate initial buffer
		if (size) buffer = reinterpret_cast<T*>(allocator->malloc(size * sizeof(T)));

		// Default construct elements
		constructElements(begin(), end());
	}

	/// Copy constructor
	FORCE_INLINE Array(const Array & other)
		: Array(other.size, other.count)
	{
		// Copy content
		constructElements(begin(), end(), other.begin());
	}

	/// Copy constructor with different allocator type
	template<typename AllocU>
	FORCE_INLINE Array(const Array<T, AllocU> & other)
		: Array(other.size, other.count)
	{
		// Copy content
		constructElements(begin(), end(), other.begin());
	}

	/// Move constructor (only if same allocator type)
	FORCE_INLINE Array(Array<T, AllocT> && other)
		: allocator{other.allocator}
		, bHasOwnAllocator{other.bHasOwnAllocator}
		, buffer{other.buffer}
		, size{other.size}
		, count{other.count}
	{
		other.allocator = nullptr;
		other.bHasOwnAllocator = false;
		other.buffer = nullptr;
		other.count = other.size = 0;
	}

	/// Copy assignment
	FORCE_INLINE Array<T, AllocT> & operator=(const Array<T, AllocT> & other)
	{
		// Check allocator
		if (allocator == nullptr)
		{
			allocator = new AllocT;
			bHasOwnAllocator = true;
		}
		
		// Realloc or create new buffer
		if (buffer == nullptr)
			buffer = reinterpret_cast<T*>(allocator->malloc((size = count = other.count) * sizeof(T)));
		else if (other.count > size)
		{
			// Dealloc this buffer
			destroyElements(begin(), end());
			allocator->free(buffer);

			buffer = reinterpret_cast<T*>(allocator->malloc((size = count = other.count) * sizeof(T)));
		}

		// Copy content
		constructElements(begin(), end(), other.begin());
		return *this;
	}

	/// Copy assignment with different allocator type
	template<typename AllocU>
	FORCE_INLINE Array<T, AllocT> & operator=(const Array<T, AllocT> & other)
	{
		// Check allocator
		if (allocator == nullptr)
		{
			allocator = new AllocT;
			bHasOwnAllocator = true;
		}
		
		// Realloc or create new buffer
		if (buffer == nullptr)
			buffer = reinterpret_cast<T*>(allocator->malloc((size = count = other.count) * sizeof(T)));
		else if (other.count > size)
		{
			// Dealloc this buffer
			destroyElements(begin(), end());
			allocator->free(buffer);

			buffer = reinterpret_cast<T*>(allocator->malloc((size = count = other.count) * sizeof(T)));
		}

		// Copy content
		constructElements(begin(), end(), other.begin());
		return *this;
	}

	/// Move assignment
	FORCE_INLINE Array<T, AllocT> & operator=(Array<T, AllocT> && other)
	{
		// Free existing buffer
		if (LIKELY(buffer != nullptr))
		{
			destroyElements(begin(), end());
			allocator->free(buffer);
		}

		// Free own allocator
		if (bHasOwnAllocator) delete allocator;

		allocator			= other.allocator;
		bHasOwnAllocator	= other.bHasOwnAllocator;

		buffer	= other.buffer;
		size	= other.size;
		count	= other.count;

		other.allocator			= nullptr;
		other.bHasOwnAllocator	= false;

		other.buffer = nullptr; 
		other.count = other.size = 0;

		return *this;
	}

public:
	/// Destructor
	FORCE_INLINE ~Array()
	{
		// Destroy elements
		if (buffer)
		{
			destroyElements(begin(), end());

			allocator->free(buffer);
			buffer = nullptr;
		}

		// Destroy allocator
		if (bHasOwnAllocator && allocator)
		{
			delete allocator;
			allocator = nullptr;
		}

		count = size = 0;
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

	/**
	 * Returns element at index
	 * 
	 * @param [in] index element index
	 * @{
	 */
	FORCE_INLINE const T & getAt(uint64 index) const
	{
		return buffer[index];
	}
	FORCE_INLINE T & getAt(uint64 index)
	{
		return buffer[index];
	}
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
			if (_size < count)
				// Destroy removed elements
				destroyElements(buffer + _size, buffer + count);

			// Realloc buffer
			buffer	= reinterpret_cast<T*>(allocator->realloc(buffer, _size * sizeof(T)));

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
		resizeIfNecessary(count = _count);

		// Construct elements
		constructElements(begin(), end());
	}

	/// Resize only if new count is bigger than current size
	FORCE_INLINE bool resizeIfNecessary(uint64 _count)
	{
		if (_count > size)
		{
			uint64 _size = size ? size * 2 : 2;
			while (_count > _size) _size *= 2;

			return resize(_size);
		}

		return false;
	}

	/// Like @ref operator[] but extends the array if out of bounds
	FORCE_INLINE T & operator()(uint64 i)
	{
		resizeIfNecessary(i + 1);
		count = PlatformMath::max(count, i + 1);

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
		const uint64 n = end - begin;
		Array out(n, n);

		// Copy buffer
		copy(out.buffer, buffer + begin, n);

		return out;
	}

	FORCE_INLINE Array<T> slice(uint64 begin, uint64 end) const
	{
		return operator()(begin, end);
	}
	FORCE_INLINE Array<T> slice(uint64 begin) const
	{
		return operator()(begin, count);
	}

	FORCE_INLINE Array<T> slice(ConstIterator begin, ConstIterator end) const
	{
		const uint64 n = reinterpret_cast<uint64>(end) - reinterpret_cast<uint64>(begin);
		Array out(n, n);

		// Copy buffer
		copy(out.buffer, begin, n);

		return out;
	}
	FORCE_INLINE Array<T> slice(ConstIterator begin) const
	{
		return slice(begin, end());
	}
	/// @}

	/**
	 * Add item at the end of the array
	 * 
	 * @param [in] item added item
	 * @return inserted item
	 */
	template<typename _T = T>
	T & add(_T && item)
	{
		resizeIfNecessary(++count);

		// Construct object
		new (buffer + count - 1) T(::forward<_T>(item));
		return buffer[count - 1];
	}

	/**
	 * Add multiple items at the end of the array
	 * 
	 * @param [in] items buffer of items
	 * @param [in] n number of items
	 */
	FORCE_INLINE void add(const T * items, uint64 n)
	{
		resizeIfNecessary(count + n);

		// Construct objects
		constructElements(begin() + count, end() + n, items);
		count += n;
	}

	/**
	 * Add default constructed element
	 * 
	 * @return insered element
	 */
	FORCE_INLINE T & addDefault()
	{
		resizeIfNecessary(count + 1);
		constructElements(begin() + count, begin() + count + 1);
		return buffer[count++];
	}

	/**
	 * Add uninitialized elements at the end of the array
	 * 
	 * @param [in] n number of elements
	 * @return inserted element
	 * @{
	 */
	FORCE_INLINE T & addUninitialized()
	{
		resizeIfNecessary(count + 1);
		return buffer[count++];
	}

	FORCE_INLINE void addUninitialized(uint64 n)
	{
		resizeIfNecessary(count + n);
		count += n;
	}
	/// @}

	/**
	 * Remove items at position
	 * 
	 * @param [in] i item's position
	 * @param [in] n number of items to remove
	 */
	FORCE_INLINE void removeAt(uint64 i, uint64 n = 1)
	{
		// Destroy elements
		Iterator it = buffer + i;
		destroyElements(it, it + n);

		// Just move back memory
		if (i + n < count) constructElements(it, it + count - (i + n), it + n);
		count -= n;
	}

	/// Remove all elements, doesn't realloc array
	FORCE_INLINE void removeAll()
	{
		destroyElements(begin(), end());
		count = 0;
	}

	/**
	 * Remove items at the end of the array
	 * 
	 * @param [in] n number of items to remove
	 */
	FORCE_INLINE void pop(uint64 n = 1)
	{
		// Destroy element
		Iterator it = buffer + (count - n);
		destroyElements(it, it + n);

		count -= n;
	}
};