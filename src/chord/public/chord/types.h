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
	};
} // namespace Chord
