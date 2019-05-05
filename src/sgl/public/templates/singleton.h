#pragma once

#include "core_types.h"

/**
 * @class Singleton templates/singleton.h
 * 
 * A singleton is an object that exists only
 * once in the lifetime of an application
 */
template<class T>
class Singleton
{
public:
	static Singleton * instance;

public:
	/// Default constructor
	FORCE_INLINE Singleton()
	{
		if (instance == nullptr)
			instance = this;
		else
			/* Fail */;
	}

private:
	/// Copy constructor, removed
	Singleton(const Singleton&) = delete;

	/// Copy assignment, removed
	Singleton & operator=(const Singleton&) = delete;

public:
	/// Returns pointer to global instance
	static FORCE_INLINE T * getPtr()
	{
		return reinterpret_cast<T*>(instance);
	}

	/// Returns ref to global instance
	static FORCE_INLINE T & get()
	{
		return *getPtr();
	}
};

template<typename T>
Singleton<T> * Singleton<T>::instance = nullptr;