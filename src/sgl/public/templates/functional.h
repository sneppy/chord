#pragma once

#include "core_types.h"
#include "const_ref.h"

/**
 * @class Compare templates/functional.h
 * 
 * Three-way comparison operator
 */
template<typename A, typename B = A>
struct Compare
{
	/**
	 * Compares two operands.
	 * Requires operator< and operator> to be defined for both types
	 * 
	 * @param [in] a,b operands to compare
	 * @return 1 if a > b, -1 if a < b, 0 otherwise
	 */
	FORCE_INLINE int32 operator()(typename ConstRef<A>::Type a, typename ConstRef<B>::Type b) const
	{
		return int32(a > b) - int32(a < b);
	}
};