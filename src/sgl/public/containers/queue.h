#pragma once

#include "core_types.h"
#include "hal/platform_memory.h"
#include "templates/const_ref.h"
#include "templates/is_trivially_copyable.h"

/**
 * @class Queue containers/queue.h
 * 
 * A Queue class implemented as a
 * singly-linked list
 */
template <typename T, typename AllocT = MallocAnsi>
class Queue
{
	// Sometimes C++ doesn't really make sense ...
	template<typename, typename> friend class Queue;

protected:
	/// A client node
	struct GCC_ALIGN(16) Client
	{
		/// Next client in queue
		Client * next;

		/// Data carried by the client
		T data;

		/// Default constructor
		FORCE_INLINE Client(typename ConstRef<T>::Type & _data, Client * _next = nullptr) :
			data(_data),
			next(nullptr) {}
	};

	using ClientRef = Client*;

protected:
	/// Allocator in use
	/// @{
	AllocT * allocator;
	bool bHasOwnAllocator;
	/// @}

	/// First client
	ClientRef first;

	/// Last client
	ClientRef last;

	/// Num of clients
	uint64 numClients;

public:
	/// Default constructor
	FORCE_INLINE Queue(AllocT * _allocator = reinterpret_cast<AllocT*>(gMalloc)) :
		allocator(_allocator),
		bHasOwnAllocator(_allocator == nullptr),
		first(nullptr),
		last(nullptr),
		numClients(0)
	{
		// Create a new allocator for the queue
		if (bHasOwnAllocator)
			allocator = new AllocT;
	}

protected:
	/// Construct client node
	FORCE_INLINE ClientRef createClient(typename ConstRef<T>::Type data)
	{
		return new (reinterpret_cast<ClientRef>(allocator->malloc(sizeof(Client)))) Client(data);
	}

public:
	/// Copy constructor
	FORCE_INLINE Queue(const Queue & other) : Queue(nullptr)
	{
		if (other.first)
		{
			// Set first
			first = last = createClient(other.first->data);

			// Very costly, we have to reinsert all the clients
			ClientRef it = other.first->next;
			while (it)
			{
				last = last->next = createClient(it->data);
				it = it->next;
			}
		}

		// Set num clients
		numClients = other.numClients;
	}

	/// Copy constructor with different allocator type
	template<typename AllocU>
	FORCE_INLINE Queue(const Queue<T, AllocU> & other) : Queue(nullptr)
	{
		if (other.first)
		{
			// Set first
			first = last = createClient(other.first->data);

			// Very costly, we have to reinsert all the clients
			typename Queue<T, AllocU>::ClientRef it = other.first->next;
			while (it)
			{
				last = last->next = createClient(it->data);
				it = it->next;
			}
		}

		// Set num clients
		numClients = other.numClients;
	}

	/// Move constructor
	FORCE_INLINE Queue(Queue && other) :
		allocator(other.allocator),
		bHasOwnAllocator(other.bHasOwnAllocator),
		first(other.first),
		last(other.last),
		numClients(other.numClients)
	{
		other.bHasOwnAllocator = false;
		other.first = other.last = nullptr;
	}

	/// Copy assignment
	FORCE_INLINE Queue & operator=(const Queue & other)
	{
		// Empty self first
		empty();

		// @todo Instead of 'empty and allocate' new links
		// use existing ones, just copy the new data

		if (other.first)
		{
			// Set first
			first = last = createClient(other.first->data);

			// Very costly, we have to reinsert all the clients
			ClientRef it = other.first->next;
			while (it)
			{
				last = last->next = createClient(it->data);
				it = it->next;
			}
		}

		// Set num clients
		numClients = other.numClients;
	}

	/// Copy assignment with different allocator type
	template<typename AllocU>
	FORCE_INLINE Queue & operator=(const Queue<T, AllocU> & other)
	{
		// Empty self first
		empty();

		// @todo Instead of 'empty and allocate' new links
		// use existing ones, just copy the new data
		
		if (other.first)
		{
			// Set first
			first = last = createClient(other.first->data);

			// Very costly, we have to reinsert all the clients
			typename Queue<T, AllocU>::ClientRef it = other.first->next;
			while (it)
			{
				last = last->next = createClient(it->data);
				it = it->next;
			}
		}

		// Set num clients
		numClients = other.numClients;
	}

	/// Move assignment
	FORCE_INLINE Queue & operator=(Queue && other)
	{
		// empty self first
		empty();

		allocator			= other.allocator;
		bHasOwnAllocator	= other.bHasOwnAllocator;
		first				= other.first;
		last				= other.last;
		numClients			= other.numClients;

		other.bHasOwnAllocator = false;
		other.first = other.last = nullptr;
	}

	/// Destructor
	FORCE_INLINE ~Queue()
	{
		// Empty queue
		empty();

		// Delete own allocator
		if (bHasOwnAllocator)
			delete allocator;
	}
	
	/// Returns number of clients in queue
	FORCE_INLINE uint64 getLength() const { return numClients; }

	/**
	 * Insert a new client in queue
	 * 
	 * @param [in] data client data
	 * @return ref to inserted data
	 */
	FORCE_INLINE T & push(typename ConstRef<T>::Type data)
	{
		// Insert at the end of the queue
		if (last == nullptr)
			first = last = createClient(data);
		else
			last = last->next = createClient(data);
		
		++numClients;

		return last->data;
	}

protected:
	/**
	 * Remove and deallocate first client in queue
	 * 
	 * ! Doesn't check for empty queue
	 * @return next client
	 */
	Client * removeFirst()
	{
		ClientRef next = first->next;
		allocator->free(first);
		--numClients;

		// Link next
		first = next;
		if (first == nullptr) last = first;

		return first;
	}

public:
	/**
	 * Pop first client in queue
	 * 
	 * @param [out] data value carried by client
	 * @return true if queue was not empty
	 * @{
	 */
	FORCE_INLINE bool pop()
	{
		if (LIKELY(first))
		{
			removeFirst();
			return true;
		}

		return false;
	}
	FORCE_INLINE bool pop(T & data)
	{
		if (LIKELY(first))
		{
			// Get data out
			moveOrCopy(data, first->data);
			return pop();
		}

		return false;
	}
	/// @}

	/// Empty the queue
	FORCE_INLINE void empty()
	{
		ClientRef it; while ((it = first))
		{
			// Move to next
			first = first->next;

			// Destroy link
			allocator->free(it);
		}

		// Make sure last is null as well
		first = last = nullptr;
	}
};

