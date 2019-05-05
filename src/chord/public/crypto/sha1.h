#pragma once

#include "coremin.h"
#include "math/math.h"

namespace Crypto
{
	/**
	 * Calculate sha-1 of string
	 * 
	 * @param [in] str input string
	 * @param [in] hash hashed value (buffer of 5 consecutives uint32)
	 */
	void sha1(const String & str, uint32 hash[5]);
} // Crypto
