#pragma once

#include "core_types.h"

/**
 * @class RefCountPtr templates/ref_count.h
 * @brief Smart pointer with reference counting
 */
template<class RefT>
class RefCountPtr
{
public:
	/// @brief Default-constructor
	FORCE_INLINE RefCountPtr() : ref(nullptr) {}

	/// @brief Copy-constructor, increments reference
	FORCE_INLINE RefCountPtr(const RefCountPtr & other) : ref(other.ref) { if(ref) ref->addRef(); }

	/// @brief Move-constructor, never increments reference
	FORCE_INLINE RefCountPtr(RefCountPtr && other) : ref(other.ref) { other.ref = nullptr; }

	/// @brief Initializer-constructor
	FORCE_INLINE RefCountPtr(RefT * _ref) : ref(_ref) { if (ref) ref->addRef(); }

	/// @brief Destructor, decrements count
	FORCE_INLINE ~RefCountPtr() { if (ref) ref->release(); }

	/// @brief Assignment operator, like copy-ctor
	/// @{
	inline RefCountPtr & operator=(RefT * _ref)
	{
		auto oldRef = ref;
		ref = _ref;

		// Update references
		if (ref) ref->addRef();
		if (oldRef) oldRef->release();

		return *this;
	}
	FORCE_INLINE RefCountPtr & operator=(const RefCountPtr & other) { return operator=(other.ref); }
	inline RefCountPtr & operator=(const RefCountPtr && other)
	{
		if (this != &other)
		{
			auto oldRef = ref;
			ref = other.ref;
			other.ref = nullptr;

			// Update references
			if (oldRef) oldRef->release();
		}
	}
	/// @}

	/// @brief Access reference
	/// @{
	FORCE_INLINE RefT * operator->() const { return ref; }
	FORCE_INLINE operator RefT() const { return ref; }
	FORCE_INLINE RefT * get() const { return ref; }
	/// @}

	/// @brief Returns @c true if reference is valid
	FORCE_INLINE bool isValid() const { return ref != nullptr; }

	/// @brief Returns reference count
	FORCE_INLINE uint32 getRefCount() const { return ref ? ref->getRefCount() : 0; }

	/// @brief Release reference
	FORCE_INLINE void release() { *this = nullptr; }

	/// @brief Swap two ref pointers, ref count does not change
	/// @{
	FORCE_INLINE void swap(RefCountPtr & other) { auto t = ref; ref = other.ref, other.ref = t; }
	FORCE_INLINE friend void swap(RefCountPtr & a, RefCountPtr & b) { a.swap(b); }
	/// @}

protected:
	/// @brief Underlying reference
	RefT * ref;
};

