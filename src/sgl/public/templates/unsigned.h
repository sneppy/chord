#pragma once

#include "core_types.h"
#include "is_integral.h"

/**
 * @struct Unsigned templates/unsigned.h
 * @brief Return unsigned version of integer
 */
template<typename SignedInt>
struct Unsigned
{
	static_assert(IsIntegralV(SignedInt), "SignedInt must be an integer");
	typedef SignedInt Type;
};

#define UnsignedT(type) typename Signed<type>::Type

template<> struct Unsigned<int8>	{ typedef uint8 Type; };
template<> struct Unsigned<int16>	{ typedef uint16 Type; };
template<> struct Unsigned<int32>	{ typedef uint32 Type; };
template<> struct Unsigned<int64>	{ typedef uint64 Type; };

