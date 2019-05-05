#pragma once

#include "core_types.h"
#include "pair.h"
#include "binary_tree.h"
#include "hal/malloc_binned.h"

/**
 * @class Map containers/tree_map.h
 * 
 * A map built on top of a binary tree
 * 
 * A tree map has an optimal space-efficiency. Keys
 * are sorted and all basic operations (insertion,
 * deletion, search) are O(log(n))
 */
template<typename KeyT, typename ValT, typename CompareT = Compare<KeyT>, typename AllocT = MallocBinned>
class Map
{
public:
	/// Pair type
	using PairT = Pair<KeyT, ValT, CompareT>;

	/// Tree type
	using TreeT = BinaryTree<PairT, Compare<PairT>, AllocT>;

	/// Node type
	using Node		= typename TreeT::Node;
	using NodeRef	= Node*;

	/// Iterators
	/// @{
	using Iterator		= typename TreeT::Iterator;
	using ConstIterator	= typename TreeT::ConstIterator;

	using MapIterator		= typename TreeT::TreeIterator;
	using ConstMapIterator	= typename TreeT::ConstTreeIterator;
	/// @}

protected:
	/// Binary tree used for node storage
	TreeT tree;

public:
	/// Default constructor
	FORCE_INLINE Map(AllocT * allocator = reinterpret_cast<AllocT*>(gMalloc)) :
		tree(allocator) {}
	
	/// Returns number of nodes
	/// @{
	FORCE_INLINE uint64 getCount() const { return tree.numNodes; }
	FORCE_INLINE uint64 getSize() const { return tree.numNodes; }
	/// @}

	/**
	 * Find value using key
	 * 
	 * @param [in] key search key
	 * @return map iterator
	 * @{
	 */
	FORCE_INLINE Iterator find(typename ConstRef<KeyT>::Type key)
	{
		return tree.find(PairT(key));
	}
	FORCE_INLINE ConstIterator find(typename ConstRef<KeyT>::Type key) const
	{
		return tree.find(PairT(key));
	}
	/// @}

	/// Returns empty search iterator
	/// @{
	FORCE_INLINE Iterator		nil()		{ return tree.nil(); }
	FORCE_INLINE ConstIterator	nil() const	{ return tree.nil(); }
	/// @}

	/// Returns linear iterators
	/// @{
	FORCE_INLINE MapIterator		begin()			{ return tree.begin(); }
	FORCE_INLINE ConstMapIterator	begin() const	{ return tree.begin(); }

	FORCE_INLINE MapIterator		end()		{ return tree.end(); }
	FORCE_INLINE ConstMapIterator	end() const	{ return tree.end(); }

	FORCE_INLINE MapIterator		last()			{ return tree.last(); }
	FORCE_INLINE ConstMapIterator	last() const	{ return tree.last(); }
	/// @}

	/**
	 * Returns ref to value associated with key
	 * 
	 * If key doesn't exist, create a new one
	 * 
	 * @param [in] key search key
	 * @return ref to associated value
	 */
	FORCE_INLINE ValT & operator[](typename ConstRef<KeyT>::Type key)
	{
		PairT pair	= PairT(key);
		Iterator it	= tree.find(pair);

		if (it != tree.nil())
			return it->second;
		else
			// Create if not found
			return tree.insert(pair).second;
	}

	/**
	 * Insert a new unique pair
	 * 
	 * @param [in] pair <key, value> pair to insert
	 * @param [in] key key value
	 * @param [in] val pair value
	 * @return inserted pair or pair that prevented insertion
	 * @{
	 */
	FORCE_INLINE PairT & insert(const PairT & pair)
	{
		return tree.insertUnique(pair);
	}
	FORCE_INLINE PairT & insert(typename ConstRef<KeyT>::Type key, typename ConstRef<ValT>::Type val)
	{
		return insert(PairT(key, val));
	}
	/// @}

	/**
	 * Remove an element from the map
	 * 
	 * @param [in] key search key
	 * @param [in] it iterator
	 * @{
	 */
	FORCE_INLINE void remove(typename ConstRef<KeyT>::Type key)
	{
		tree.remove(tree.find(PairT(key)));
	}
	FORCE_INLINE void remove(MapIterator it)
	{
		tree.remove(it);
	}
	FORCE_INLINE void remove(ConstMapIterator it)
	{
		tree.remove(it);
	}
	FORCE_INLINE void remove(Iterator it)
	{
		tree.remove(it);
	}
	FORCE_INLINE void remove(ConstIterator it)
	{
		tree.remove(it);
	}

	FORCE_INLINE void erase(Iterator it)
	{
		remove(it);
	}
	FORCE_INLINE void erase(ConstIterator it)
	{
		remove(it);
	}
	/// @}

public:

};

