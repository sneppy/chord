#pragma once

#include "chord_fwd.h"

namespace Chord
{
	/**
	 * @struct NodeInfo chord/types.h
	 * 
	 * Holds information about a chord node
	 */
	struct NodeInfo
	{
	public:
		/// Node id
		uint32 id;

		/// Node address
		Ipv4 addr;

	public:
		/// Get string with info
		FORCE_INLINE String getInfoString() const
		{
			char info[256]; snprintf(info, 256, "#%08x @ %s", id, *getIpString(addr));
			return String(info);
		}
	};
} // namespace Chord
