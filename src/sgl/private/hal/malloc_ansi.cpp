#include "hal/malloc_ansi.h"
#include "hal/platform_math.h"

#if PLATFORM_UNIX
	#include <malloc.h>
#endif

void * MallocAnsi::malloc(uintP n, uint32 alignment)
{
	// Compute min alignment
	alignment = PlatformMath::max(n >= 16 ? 16U : 8U, alignment);

#if PLATFORM_UNIX
	void * out;
	if (UNLIKELY(::posix_memalign(&out, alignment, n) != 0)) return nullptr;
#else
	// Implement some kind of aligned allocation
	void * out = nullptr;
#endif

	return out;
}

void * MallocAnsi::realloc(void * original, uintP n, uint32 alignment)
{
	alignment = PlatformMath::max(n >= 16 ? 16U : 8U, alignment);

#if PLATFORM_UNIX
	void * out;

	if (original && n)
	{
		// Do not even try to realloc in place
		// But why??
		uintP usable = malloc_usable_size(original);
		if (UNLIKELY(::posix_memalign(&out, alignment, n) != 0))
			return nullptr;
		else if (LIKELY(usable))
			Memory::memcpy(out, original, PlatformMath::min(n, usable));
		
		// Free original block
		::free(original);
	}
	else if (!original)
	{
		// Mallocate
		if (UNLIKELY(::posix_memalign(&out, alignment, n) != 0)) return nullptr;
	}
	else
	{
		::free(original);
		out = nullptr;
	}
#else
	// Implement aligned realloc using only ::realloc()
#endif

	return out;
}

void MallocAnsi::free(void * original)
{
#if PLATFORM_UNIX
	::free(original);
#else
	// Implement aligned free using only ::free()
#endif
}

bool MallocAnsi::getAllocSize(void * original, uintP & n)
{
	if (!original) return false;

#if PLATFORM_UNIX
	n = malloc_usable_size(original);
#else
	// Implement aligned usable size?
#endif

	return true;
}