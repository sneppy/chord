#pragma once

#include "platform_memory.h"

/**
 * @class MallocAnsi hal/malloc_ansi.h
 * @brief Default allocation class
 */
class MallocAnsi : public Malloc
{
public:
	/// @copydoc Malloc::malloc()
	virtual void * malloc(sizet n, uint32 alignment = DEFAULT_ALIGNMENT) override;

	/// @copydoc Malloc::realloc()
	virtual void * realloc(void * original, sizet n, uint32 alignment = DEFAULT_ALIGNMENT) override;

	/// @copydoc Malloc::free()
	virtual void free(void * original) override;

	/// @copydoc Malloc::getAllocSize()
	virtual bool getAllocSize(void * original, sizet & n) override;
};

