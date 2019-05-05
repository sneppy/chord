#include "hal/malloc_pool.h"
MallocPool::MallocPool(uint64 _numBlocks, sizet _blockSize, sizet blockAlignment, void * buffer) :
	pool(buffer),
	bHasOwnBuffer(buffer == nullptr),
	numBlocks(_numBlocks),
	blockSize(_blockSize),
	numFreeBlocks(numBlocks)
{
	const sizet
		descriptorSize	= sizeof(void*),
		chunkSize		= PlatformMath::alignUp(descriptorSize + blockSize, blockAlignment),
		poolSize		= blockAlignment + chunkSize * numBlocks;

	// Allocate pool
	if (bHasOwnBuffer)
	{
		if (posix_memalign(&pool, blockAlignment, poolSize)) return;
	}

	// Padding for aligned blocks
	head = setOffset(pool, blockAlignment - descriptorSize);
	// Set pool end address
	end = reinterpret_cast<void*>(reinterpret_cast<uint64>(pool) + poolSize);

	// Init linked list
	for (uint64 b = 0; b < numBlocks - 1; ++b)
		head = setOffset(head, chunkSize);
	
	// Set last as nullptr
	setNext(head, nullptr);
	
	// Reset head
	head = getNext(pool);
}

void * MallocPool::malloc(sizet n, uint32 alignment)
{
	// Pool empty
	if (head == nullptr | n > blockSize) return nullptr;

	void * out = getBlock(head);
	head = getNext(head);

	--numFreeBlocks;
	return out;
}

void * MallocPool::realloc(void * original, sizet n, uint32 alignment)
{
	// We cannot really realloc
	if (n > blockSize)
		return nullptr;
	else
		return original;
}

void MallocPool::free(void * original)
{
	// Relink in free list
	original = getDescriptor(original);
	setNext(original, (void**)head);
	
	++numFreeBlocks;
	head = original;
}

bool MallocPool::getAllocSize(void * original, sizet & n)
{
	if (hasBlock(original))
	{
		n = blockSize;
		return true;
	}
	
	return false;
}