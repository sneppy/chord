#pragma once

#include "core_types.h"

/// @brief Global allocator
extern class Malloc * gMalloc;

/// @brief Default alignment
enum
{
	DEFAULT_ALIGNMENT	= 0x8,
	MIN_ALIGNMENT		= 0x8
};

/**
 * @class Malloc hal/memory_base.h
 */
class Malloc
{
public:
	/**
	 * @brief Memory allocation
	 * 
	 * @param [in]	n			number of Bytes
	 * @param [in]	alignment	memory alignment
	 * 
	 * @return address of allocated memory
	 */
	virtual void * malloc(uintP n, uint32 alignment = DEFAULT_ALIGNMENT) = 0;

	/**
	 * @brief Memory re(al)location
	 * 
	 * @param [in]	original	originally allocated block
	 * @param [in]	n			number of Bytes
	 * @param [in]	alignment	memory alignment
	 * 
	 * @return address of reallocated memory
	 */
	virtual void * realloc(void * original, uintP n, uint32 alignment = DEFAULT_ALIGNMENT) = 0;

	/**
	 * @brief Memory deallocation+
	 * 
	 * @param [in] original originally deallcoated block
	 */
	virtual void free(void * original) = 0;

	/**
	 * @brief Return allocation size if possible
	 * 
	 * @param [in]	original	originally allocated block
	 * @param [out]	n			allocation size
	 * 
	 * @return @c true if possible, @c false otherwise
	 */
	virtual bool getAllocSize(void * original, uintP & n) { return false; }
};

