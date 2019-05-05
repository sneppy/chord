#pragma once

#include "core_types.h"

template<typename T>
class UUIdGenerator
{
protected:
	/// Current state = last id
	T state;

public:
	/// Default constructor
	FORCE_INLINE UUIdGenerator() : state(0) {}

	/// Get next uuid
	FORCE_INLINE T getNext() { return ++state; }
};