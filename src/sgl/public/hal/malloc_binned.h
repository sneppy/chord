#pragma once

#include "core_types.h"
#include "platform_memory.h"
#include "platform_math.h"
#include "malloc_pool.h"
#include "containers/linked_list.h"
#include "containers/binary_tree.h"
#include "containers/pair.h"

#define MALLOC_BINNED_POOL_SIZE 8 * 1024 * 1024		// Fixed pool size, 4 MB
#define MALLOC_BINNED_BLOCK_MIN_SIZE 32				// Min block size (block size for first bucket), 32 B
#define MALLOC_BINNED_BLOCK_MAX_SIZE 1024 * 1024	// Max block size (block size for last bucket), 1 MB
#define MALLOC_BINNED_BLOCK_ALIGNMENT 0x20			// Block alignment inside pool
#define MALLOC_BINNED_NUM_BUCKETS 16				// Should change according to min and max bucket size
#define MALLOC_BINNED_POOL_ALIGNMENT 0x1000			// 4 KB, size of a page

/**
 * @class MallocBinned hal/malloc_binned.h
 * 
 * Allocator backed by memory pools
 * 
 * A binned allocator uses buckets of memory pools
 * organized by block size.
 * 
 * When an allocation request is made, the request
 * is forwarded to the appropriate bucket and the
 * first free pool is queried for a free block. If
 * all pools are exahusted, a new pool is created
 * and pushed to the front of the list (for future
 * allocations).
 * 
 * If the allocation request exceeds the maximum
 * block size, a backup allocator is used instead.
 * 
 * The allocator also mantains a red-black tree of
 * the pools using the pool initial address as key.
 * This allows us to quickly locate the pool that
 * alloc'd a block, for deallocating it or determine
 * is allocation size. If no such pool is found, we
 * assume that the blocks was allocated using the
 * backup allocator
 */
class MallocBinned : public Malloc
{
protected:
	/// Link and node types
	/// @{
	using PoolLink		= Link<MallocPool>;
	using PoolLinkRef	= PoolLink*;

	using PoolNode		= BinaryNode<Pair<void*, MallocPool*>>;
	using PoolNodeRef	= PoolNode*;
	/// @}

protected:
	/// Buckets of pools
	PoolLinkRef buckets[MALLOC_BINNED_NUM_BUCKETS];

	/// Tree of pool refs organized by memory address
	/// Allows for fast deallocation (O(log(N)) search)
	PoolNodeRef root;

	/// Some stats
	/// @{
	uint64 numPools;
	/// @}

public:
	/// Default constructor
	MallocBinned();

	/// Default destructor
	FORCE_INLINE ~MallocBinned()
	{
		// Destroy pools
		for (uint32 i = 0; i < MALLOC_BINNED_NUM_BUCKETS; ++i)
		{
			PoolLinkRef next = buckets[i], it;
			while (it = next)
			{
				next = it->next;
				// Free whole block
				::free(it);
			}
		}
	}

	/// Returns number of allocated pools
	FORCE_INLINE uint64 getNumPools() { return numPools; }

protected:
	/// Get bucket index from required size
	FORCE_INLINE uint32 getBucketIndex(sizet n) const
	{
		uint32 i = 0; while (n > MALLOC_BINNED_BLOCK_MIN_SIZE) ++i, n >>= 1;
		return i;
	}

	/// Create a new pool
	/// Forced inline, not used outside class
	FORCE_INLINE MallocPool * createPool(uint32 bucketIdx)
	{
		/// Update stats
		++numPools;

		/**
		 * Create a new pool.
		 *  PoolLink PoolNode Padding Pool buffer
		 * |--------|--------|-------|-----------|
		 */
		const sizet allocSize
			= sizeof(PoolLink)
			+ sizeof(PoolNode)
			+ MALLOC_BINNED_POOL_ALIGNMENT
			+ MALLOC_BINNED_POOL_SIZE;

		// Create buffer
		void * buffer;
		if (::posix_memalign(&buffer, PlatformMath::max(alignof(PoolLink), alignof(PoolNode)), allocSize) == 0)
		{
			// Compute offsets
			PoolLinkRef link	= reinterpret_cast<PoolLinkRef>(buffer);
			PoolNodeRef node	= reinterpret_cast<PoolNodeRef>(link + 1);
			void * poolBuffer	= reinterpret_cast<void*>(PlatformMath::alignUp(reinterpret_cast<uint64>(node + 1), (uint64)MALLOC_BINNED_POOL_ALIGNMENT));
			
			// Compute pool properties
			const sizet blockSize	= (1 << bucketIdx) * MALLOC_BINNED_BLOCK_MIN_SIZE;
			const sizet chunkSize	= PlatformMath::alignUp(sizeof(void*) + blockSize, MALLOC_BINNED_BLOCK_ALIGNMENT);
			const uint64 numBlocks	= (MALLOC_BINNED_POOL_SIZE - MALLOC_BINNED_BLOCK_ALIGNMENT) / chunkSize;

			// Construct link
			new (link) PoolLink((MallocPool&&)MallocPool(numBlocks, blockSize, MALLOC_BINNED_BLOCK_ALIGNMENT, poolBuffer));

			// Construct node
			new (node) PoolNode(Pair<void*, MallocPool*>(poolBuffer, &(link->data)));

			// Link pool
			link->linkNext(buckets[bucketIdx]);
			buckets[bucketIdx] = link;

			// Insert in tree
			if (UNLIKELY(root == nullptr))
			{
				root = node;
				root->color = PoolNode::NodeColor::BLACK;
			}
			else
			{
				root->insertUnique(node);

				if (UNLIKELY(root->parent))
					root = root->parent;
			}

			return node->data.second;
		}

		return nullptr;
	}

public:
	//////////////////////////////////////////////////
	// Malloc interface
	//////////////////////////////////////////////////

	/// @copydoc Malloc::malloc()
	virtual void * malloc(sizet n, uint32 alignment = DEFAULT_ALIGNMENT) override;

	/// @copydoc Malloc::realloc()
	virtual void * realloc(void * original, sizet n, uint32 alignment = DEFAULT_ALIGNMENT) override;

	/// @copydoc Malloc::free()
	virtual void free(void * original) override;

	/// @copydoc Malloc::getAllocSize()
	virtual bool getAllocSize(void * original, sizet & n) override;
};

