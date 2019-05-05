#pragma once

#include "core_types.h"
#include "platform_memory.h"
#include "platform_math.h"

/**
 * @class MallocPool hal/malloc_pool.h
 * 
 * A fixed size pool with a fixed block size.
 * Blocks are managed by a singly linked list
 * embedded in the blocks themselves. Blocks
 * are introduced by a descriptor which points
 * to the next block in the free list.
 * 
 * When a block is freed it is reinserted at
 * the beginning of the list.
 * 
 * It is possible to specify the alignment of
 * the blocks, it should be a power of two.
 */
class GCC_ALIGN(32) MallocPool : public Malloc
{
	friend class MallocBinned;

protected:
	/// Memory pool
	void * pool;
	bool bHasOwnBuffer;

	/// Pool end address
	void * end;

	/// Head of free list
	void * head;

	/// Size of a single block in Bytes
	sizet blockSize;

	/// Total number of blocks
	uint64 numBlocks;

	/// Number of free blocks
	uint64 numFreeBlocks;

protected:
	/// Set descriptor value
	FORCE_INLINE void * setNext(void * descriptor, void ** next)
	{
		return *reinterpret_cast<void**>(descriptor) = next;
	}

	/// Get discriptor value
	FORCE_INLINE void * getNext(void * descriptor)
	{
		return *reinterpret_cast<void**>(descriptor);
	}

	/// Get block address from descriptor
	FORCE_INLINE void * getBlock(void * descriptor)
	{
		return reinterpret_cast<void*>(reinterpret_cast<uint64>(descriptor) + sizeof(void*));
	}

	/// Get descriptor address from block
	FORCE_INLINE void * getDescriptor(void * block)
	{
		return reinterpret_cast<void*>(reinterpret_cast<uint64>(block) - sizeof(void*));
	}

	/// Set descriptor value to offset
	FORCE_INLINE void * setOffset(void * descriptor, sizet offset)
	{
		return *reinterpret_cast<void**>(descriptor) = reinterpret_cast<void**>(reinterpret_cast<uint64>(descriptor) + offset);
	}

public:
	/// Default constructor
	MallocPool(uint64 _numBlocks = 65536, sizet _blockSize = 64/* Bytes */, sizet blockAlignment = 0x20, void * buffer = nullptr);

	/// TODO: Make it non-copyable

	/// Destructor
	FORCE_INLINE ~MallocPool()
	{
		if (bHasOwnBuffer)
			free(pool);
	}

	/// Returns num of free blocks
	FORCE_INLINE uint64 getNumFreeBlock() const { return numFreeBlocks; }

	/// Returns true if block was allocated by this allocator
	FORCE_INLINE bool hasBlock(void * original) const { return original > pool && original < end; }

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