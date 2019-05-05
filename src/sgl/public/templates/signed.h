#pragma once

#include "core_types.h"
#include "is_integral.h"

/**
 * @struct Unsigned templates/unsigned.h
 * @brief Return unsigned version of integer
 */
template<typename UnsignedInt>
struct Signed
{
	static_assert(IsIntegralV(UnsignedInt), "UnsignedInt must be an integer type");
	using Type = void;
};

#define SignedT(T) typename Signed<T>::Type

template<> struct Signed<uint8>		{ using Type = int8; };
template<> struct Signed<uint16>	{ using Type = int16; };
template<> struct Signed<uint32>	{ using Type = int32; };
template<> struct Signed<uint64>	{ using Type = int64; };

