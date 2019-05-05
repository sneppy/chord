#pragma once

#include "core_types.h"
#include "templates/const_ref.h"
#include "templates/functional.h"
#include "templates/same_type.h"

/**
 * @class Pair containers/pair.h
 * 
 * A pair of values
 */
template<typename A, typename B, typename CompareT = Compare<A>>
class Pair
{
public:
	/// First element
	A first;

	/// Second element
	B second;

public:
	/// Pair constructor
	FORCE_INLINE Pair(typename ConstRef<A>::Type _first = A(), typename ConstRef<B>::Type _second = B()) :
		first(_first),
		second(_second) {}

	/// Equality operators
	/// @{
	FORCE_INLINE bool operator==(const Pair & other) const { return first == other.first & second == other.second; }
	FORCE_INLINE bool operator!=(const Pair & other) const { return first != other.first | second != other.second; }
	/// @}

	/// Key comparison operators
	/// @{
	FORCE_INLINE bool operator< (const Pair & other) const { return CompareT()(first, other.first) < 0; }
	FORCE_INLINE bool operator> (const Pair & other) const { return CompareT()(first, other.first) > 0; }
	FORCE_INLINE bool operator<=(const Pair & other) const { return CompareT()(first, other.first) <= 0; }
	FORCE_INLINE bool operator>=(const Pair & other) const { return CompareT()(first, other.first) >= 0; }
	/// @}

	/// Key comparison, pairs with different compare methods
	/// @{
	template<typename CompareU>
	FORCE_INLINE typename EnableIf<!SameType<CompareT, CompareU>::value, bool>::Type operator< (const Pair<A, B, CompareU> & other) { return CompareT()(first, other.first) < 0; }
	template<typename CompareU>
	FORCE_INLINE typename EnableIf<!SameType<CompareT, CompareU>::value, bool>::Type operator> (const Pair<A, B, CompareU> & other) { return CompareT()(first, other.first) > 0; }
	template<typename CompareU>
	FORCE_INLINE typename EnableIf<!SameType<CompareT, CompareU>::value, bool>::Type operator<=(const Pair<A, B, CompareU> & other) { return CompareT()(first, other.first) <= 0; }
	template<typename CompareU>
	FORCE_INLINE typename EnableIf<!SameType<CompareT, CompareU>::value, bool>::Type operator>=(const Pair<A, B, CompareU> & other) { return CompareT()(first, other.first) >= 0; }
	/// @}
};

