#pragma once

#include "core_types.h"

/**
 * @struct GenericPlatformString generic/generic_platform_string.h
 * 
 * Utility functions to work with C-like strings
 */
struct GenericPlatformString
{
	/// Convert a character to lowercase
	template<typename C>
	static FORCE_INLINE C toLower(C c)
	{
		// Default for ansi character
		return c < 96 ? c + 32 : c;
	}

	/// Get length of a null terminated string
	template<typename C>
	static FORCE_INLINE uint64 strlen(const C * string)
	{
		return string ? ::strlen(string) : 0;
	}

	/**
	 * Compare two C-strings
	 * 
	 * @param s1,s2 C-strigs (assumed to be null-terminated)
	 * 
	 * @return	zero if they are equal,
	 * 			greater than zero if s1 > s2
	 * 			otherwise less than zero
	 */
	template<typename S1, typename S2>
	static FORCE_INLINE int32 strcmp(const S1 * s1, const S2 * s2)
	{
		for (; *s1 || *s2; ++s1, ++s2)
			if (*s1 != *s2) return *s1 - *s2;
		
		// Same string
		return 0;
	}

	/**
	 * Same as @ref strcmp but case insensitive
	 */
	template<typename S1, typename S2>
	static FORCE_INLINE int32 strcmpi(const S1 * s1, const S2 * s2)
	{
		for (; *s1 || *s2; ++s1, ++s2)
		{
			if (*s1 != *s2)
			{
				// Compare case insensitive
				const auto c1 = toLower(*s1);
				const auto c2 = toLower(*s2);

				if (c1 != c2) return c1 - c2;
			}
		}

		// Same string
		return 0;
	}

	/**
	 * Same as @ref strcmp but with limit
	 */
	template<typename S1, typename S2>
	static FORCE_INLINE int32 strncmp(const S1 * s1, const S2 * s2, uint64 n)
	{
		for (uint64 i = 0; i < n; ++i, ++s1, ++s2)
		{
			if (*s1 != *s2) return *s1 - *s2;
		}

		// Same string
		return 0;
	}

	/**
	 * Same as @ref strncmp but case insensitive
	 */
	template<typename S1, typename S2>
	static FORCE_INLINE int32 strncmpi(const S1 * s1, const S2 * s2, uint64 n)
	{
		for (uint64 i = 0; i < n; ++i, ++s1, ++s2)
		{
			if (*s1 != *s2)
			{
				// Compare case insensitive
				const auto c1 = toLower(*s1);
				const auto c2 = toLower(*s2);

				if (c1 != c2) return c1 - c2;
			}
		}

		// Same string
		return 0;
	}
};

