#pragma once

#include "core_types.h"
#include "hal/platform_memory.h"
#include "hal/malloc_ansi.h"
#include "templates/const_ref.h"
#include "templates/is_trivially_copyable.h"

/// A single link of the list
template<typename T>
struct GCC_ALIGN(32) Link
{
	template<typename U, typename AllocU>
	friend class LinkedList;

public:
	/// Next link
	Link * next;

	/// Previous link
	Link * prev;

	/// Data held by the link
	T data;

public:
	/// Default constructor
	FORCE_INLINE Link(typename ConstRef<T>::Type _data, Link * _next = nullptr, Link * _prev = nullptr) :
		data(_data),
		prev(_prev),
		next(_next) {}

	/// Destructor
	FORCE_INLINE ~Link()
	{
		unlink();
	}

	/// Link next
	FORCE_INLINE void linkNext(Link * _next)
	{
		next = _next;
		if (LIKELY(next != nullptr))
		{
			// Your prev is my prev!
			prev = next->prev;

			// Link with next
			next->prev = this;

			if (prev != nullptr)
				// Link with prev
				prev->next = this;
		}
	}

	/// Link prev
	FORCE_INLINE void linkPrev(Link * _prev)
	{
		prev = _prev;
		if (LIKELY(prev != nullptr))
		{
			// Your next is my next!
			next = prev->next;

			// Link with prev
			prev->next = this;

			if (next != nullptr)
				// Link with next
				next->prev = this;
		}
	}

	/// Unlik this link
	FORCE_INLINE void unlink()
	{
		if (next != nullptr) next->prev = this->prev;
		if (prev != nullptr) prev->next = this->next;
		
		prev = next = nullptr;
	}
};

/// Link pointer
template<typename T> using LinkRef = Link<T>*;

/**
 * @class LinkedList containers/linked_list.h
 * Implementation of a traditional linked list container
 */
template<typename T, typename AllocT = MallocAnsi>
class GCC_ALIGN(32) LinkedList
{
	template<typename U, typename AllocU>
	friend class LinkedList;

public:
	/// LinkedList iterator
	template<typename U>
	class LinkedListIterator
	{
		friend LinkedList;

	private:
		/// Current link
		LinkRef<U> curr;

	public:
		/// Iterator methods
		/// @{
		FORCE_INLINE LinkedListIterator<U> & operator++()
		{
			curr = curr->next;
		}
		FORCE_INLINE LinkedListIterator<U> & operator--()
		{
			curr = curr->prev;
		}

		FORCE_INLINE bool operator==(const LinkedListIterator & iter) const { return curr == iter.curr; }
		FORCE_INLINE bool operator!=(const LinkedListIterator & iter) const { return curr != iter.curr; }

		FORCE_INLINE U & operator* () const { return curr->data; }
		FORCE_INLINE U * operator->() const { return &(curr->data); }
		/// @}

		/// Returns the link itself, not the underlying data
		FORCE_INLINE LinkRef<U> getLink() { return curr; }

	private:
		/// Default-constructor, private usage
		LinkedListIterator(LinkRef<U> _link = nullptr) : curr(_link) {}
	};

	using Iterator		= LinkedListIterator<T>;
	using ConstIterator	= LinkedListIterator<const T>;

protected:
	/// Allocator in use
	/// @{
	AllocT * allocator;
	bool bHasOwnAllocator;
	/// @}

	/// Head of the list
	LinkRef<T> head;

	/// Tail of the list
	LinkRef<T> tail;

	/// List length
	uint64 count;

public:
	/// Default-constructor, empty list
	FORCE_INLINE LinkedList(AllocT * _allocator = reinterpret_cast<AllocT*>(gMalloc)) :
		allocator(_allocator),
		bHasOwnAllocator(_allocator == nullptr),
		head(nullptr),
		tail(nullptr),
		count(0)
	{
		// Create own allocator
		if (bHasOwnAllocator)
			allocator = new AllocT;
	}

protected:
	/// Creates a new link with the provided data
	FORCE_INLINE LinkRef<T> createLink(typename ConstRef<T>::Type data, LinkRef<T> next = nullptr, LinkRef<T> prev = nullptr)
	{
		return new (reinterpret_cast<LinkRef<T>>(allocator->malloc(sizeof(Link<T>)))) Link<T>(data, next, prev);
	}

public:
	/// Copy constructor
	FORCE_INLINE LinkedList(const LinkedList & other) : LinkedList(nullptr)
	{
		if (other.head)
		{
			// Set head
			head = tail = createLink(other.head->data);

			// Very costly, we have to reinsert all the clients
			LinkRef<T> it = other.head->next;
			while (it)
			{
				tail = tail->next = createLink(it->data);
				it = it->next;
			}
		}

		// Set num clients
		count = other.count;
	}

	/// Copy constructor with different allocator type
	template<typename AllocU>
	FORCE_INLINE LinkedList(const LinkedList<T, AllocU> & other) : LinkedList(nullptr)
	{
		if (other.head)
		{
			// Set head
			head = tail = createLink(other.head->data);

			// Very costly, we have to reinsert all the clients
			typename LinkedList<T, AllocU>::LinkRefT it = other.head->next;
			while (it)
			{
				tail = tail->next = createLink(it->data);
				it = it->next;
			}
		}

		// Set num clients
		count = other.count;
	}

	/// Move constructor
	FORCE_INLINE LinkedList(LinkedList && other) :
		allocator(other.allocator),
		bHasOwnAllocator(other.bHasOwnAllocator),
		head(other.head),
		tail(other.tail),
		count(other.count)
	{
		other.bHasOwnAllocator = false;
		other.head = other.tail = nullptr;
	}

	/// Copy assignment
	FORCE_INLINE LinkedList & operator=(const LinkedList & other)
	{
		// empty self first
		empty();

		// @todo Instead of 'empty and allocate' new links
		// use existing ones, just copy the new data

		if (other.head)
		{
			// Set head
			head = tail = createLink(other.head->data);

			// Very costly, we have to reinsert all the clients
			LinkRef<T> it = other.head->next;
			while (it)
			{
				tail = tail->next = createLink(it->data);
				it = it->next;
			}
		}

		// Set num clients
		count = other.count;
	}

	/// Copy assignment with different allocator type
	template<typename AllocU>
	FORCE_INLINE LinkedList & operator=(const LinkedList<T, AllocU> & other)
	{
		// empty self first
		empty();

		// @todo Instead of 'empty and allocate' new links
		// use existing ones, just copy the new data

		if (other.head)
		{
			// Set head
			head = tail = createLink(other.head->data);

			// Very costly, we have to reinsert all the clients
			typename LinkedList<T, AllocU>::LinkRefT it = other.head->next;
			while (it)
			{
				tail = tail->next = createLink(it->data);
				it = it->next;
			}
		}

		// Set num clients
		count = other.count;
	}

	/// Move assignment
	FORCE_INLINE LinkedList & operator=(LinkedList && other)
	{
		// empty self first
		empty();

		allocator			= other.allocator;
		bHasOwnAllocator	= other.bHasOwnAllocator;
		head				= other.head;
		tail				= other.tail;
		count				= other.count;

		other.bHasOwnAllocator = false;
		other.head = other.tail = nullptr;
	}

	/// Destructor
	FORCE_INLINE ~LinkedList()
	{
		// Empty list
		empty();

		// Delete own allocator
		if (bHasOwnAllocator)
			delete allocator;
	}

	/// Return list length
	/// @{
	FORCE_INLINE uint64 getCount() const	{ return count; }
	FORCE_INLINE uint64 getLength() const	{ return count; }
	/// @}

	/// Random access operator, O(n) time complexity
	T & operator[](uint64 i)
	{		
		if (i < count / 2)
		{
			LinkRef<T> it = head;
			while (i > 0) it = it->next, --i;

			return it->data;
		}
		else
		{
			i = (count - 1) - i;
			LinkRef<T> it = tail;
			while (i > 0) it = it->prev, --i;

			return it->data;
		}
	}

	/// Iterators
	/// @{
	FORCE_INLINE Iterator		begin()			{ return Iterator(head); }
	FORCE_INLINE ConstIterator	begin() const	{ return ConstIterator(head); }

	FORCE_INLINE Iterator		end()		{ return Iterator(nullptr); }
	FORCE_INLINE ConstIterator	end() const	{ return ConstIterator(nullptr); }
	/// @}

	/**
	 * Insert a new element at the end of the end of the list
	 * 
	 * @param [in] elem element to insert
	 * @{
	 */
	/// @return self
	FORCE_INLINE LinkedList & operator+=(typename ConstRef<T>::Type elem)
	{
		// Push to end, replaces tail
		if (LIKELY(tail))
			tail = tail->next = createLink(elem, nullptr, tail);
		else
			head = tail = createLink(elem);

		++count;
		return *this;
	}
	/// @return ref to inserted element
	FORCE_INLINE T & push(typename ConstRef<T>::Type elem)
	{
		operator+=(elem);
		return tail->data;
	}
	/// @}

	/// Inserts in first position
	FORCE_INLINE T & pushFront(typename ConstRef<T>::Type elem)
	{
		if (LIKELY(head))
			head = head->prev = createLink(elem, head);
		else
			head = tail = createLink(elem);

		++count;
		return head->data;
	}

	/**
	 * Insert a new element at the provided position
	 * 
	 * @param [in]	elem	element to insert
	 * @param [in]	i		element position
	 * 
	 * @return ref to inserted elem
	 */
	T & insert(typename ConstRef<T>::Type elem, uint64 i = 0)
	{
		// Find i-th link
		LinkRef<T> it;
		if (i < count / 2)
		{
			it = head;
			while (i > 0) it = it->next, --i;
		}
		else
		{
			i = (count - 1) - i, it = tail;
			while (i > 0) it = it->prev, --i;
		}

		// Create link in i-th position
		LinkRef<T> link = createLink(elem);
		link->linkNext(it);

		++count;
		return link->data;
	}

	/// Removes last element of the list
	/// @{
	FORCE_INLINE bool pop()
	{
		if (tail)
		{
			LinkRef<T> removed = tail;
			tail = tail->prev;

			removed->~Link();
			allocator->free(removed);

			return true;
		}

		return false;
	}
	FORCE_INLINE bool pop(T & data)
	{
		if (tail)
		{
			moveOrCopy(data, head->data);
			return pop();
		}

		return false;
	}
	/// @}

	/// Removes first element of the list
	/// @{
	FORCE_INLINE bool popFront()
	{
		if (head)
		{
			// Unlink
			LinkRef<T> removed = head;
			head = head->next;

			removed->~Link();
			allocator->free(removed);

			return true;
		}

		return false;
	}
	FORCE_INLINE bool popFront(T & data)
	{
		if (head)
		{
			moveOrCopy(data, head->data);
			return popFront();
		}

		return false;
	}
	/// @}

	/// Empty the list
	FORCE_INLINE void empty()
	{
		LinkRef<T> it; while ((it = head))
		{
			// Move to next
			head = head->next;

			// Destroy link
			it->~Link();
			allocator->free(it);
		}

		// Make sure tail is null as well
		head = tail = nullptr;
	}
};

