#pragma once

#include "core_types.h"
#include "hal/platform_atomics.h"
#include "signed.h"
#include "is_trivial.h"

/**
 * @class BaseAtomic templates/atomic.h
 * @brief Base class for all atomic types
 */
template<typename T>
class GCC_ALIGN(alignof(T)) BaseAtomic
{
protected:
	/// @brief Underlying object
	volatile T obj;

public:
	/// @brief Returns a copy of the current value
	FORCE_INLINE T load(AtomicOrder order = AtomicOrder::Sequential) const
	{
		switch (order)
		{
			case AtomicOrder::Sequential:
				return PlatformAtomics::read(&obj);
			
			case AtomicOrder::Relaxed:
				return PlatformAtomics::readRelaxed(&obj);

			default:
				// ! Invalid order type
				return 0;
		}
	}

	/// @brief Stores value in underling object
	FORCE_INLINE void store(T val, AtomicOrder order = AtomicOrder::Sequential)
	{
		switch (order)
		{
			case AtomicOrder::Sequential:
				return PlatformAtomics::store(&obj, val);
			
			case AtomicOrder::Relaxed:
				return PlatformAtomics::storeRelaxed(&obj, val);
		}
	}

	/// @brief Like @ref store() but returns a copy of the previous value
	FORCE_INLINE T exchange(T val) { return PlatformAtomics::exchange(&obj, val); }

protected:
	/// @brief Default-constructor, default
	BaseAtomic() = default;

	/// @brief Value-constructor
	BaseAtomic(T val) : obj(val) {}
};

/**
 * @class ArithmeticBaseAtomic templates/atomic.h
 * @brief Atomic with overloaded arithmetic operators
 */
template<typename T, typename DiffT>
class ArithmeticBaseAtomic : public BaseAtomic<T>
{
public:
	/// @brief Pre-increment-operator, for arithmetic types
	FORCE_INLINE T operator++() { return PlatformAtomics::increment(&this->obj) + 1; }

	/// @brief Post-increment-operator, for arithmetic types
	FORCE_INLINE T operator++(int) { return PlatformAtomics::increment(&this->obj); }

	/// @brief Pre-decrement operator, for arithmetic types
	FORCE_INLINE T operator--() { return PlatformAtomics::decrement(&this->obj) - 1; }

	/// @brief Post-decrement operator, for arithmetic types
	FORCE_INLINE T operator--(int) { return PlatformAtomics::decrement(&this->obj); }

	/**
	 * @brief Arithmetic binary operators
	 * 
	 * @param [in] val second operand
	 * 
	 * @return result of operation
	 * @{
	 */
	FORCE_INLINE T operator+=(DiffT val) { return PlatformAtomics::add(&this->obj, val); }
	FORCE_INLINE T operator-=(DiffT val) { return PlatformAtomics::add(&this->obj, - (SignedT(T))val); }

protected:
	/// @brief Default constructor, default
	ArithmeticBaseAtomic() = default;

	/// @brief Inherit constructors
	using BaseAtomic<T>::BaseAtomic;
};

/// @brief Generic classes for different types of atomics
/// @{
template<typename T> using PointerAtomic = ArithmeticBaseAtomic<T, intP>;
template<typename T> using IntegralAtomic = ArithmeticBaseAtomic<T, T>;
/// @}

/// @brief Automatically detect atomic type
/// @{
template<typename T, bool bIsIntegral> struct AtomicType { using Type = BaseAtomic<T>; };
template<typename T> struct AtomicType<T, true> { using Type = IntegralAtomic<T>; };
template<typename T> struct AtomicType<T*, false> { using Type = PointerAtomic<T>; };

template<typename T> using AtomicT = typename AtomicType<T, IsIntegralV(T)>::Type;
/// @}

/**
 * @class Atomic templates/atomic.h
 * @brief Wraps a variable in a container that atomizes operations
 */
template<typename T>
class Atomic : public AtomicT<T>
{
	// Only with trivial types
	static_assert(IsTrivialV(T), "Atomic only works for trivial types");

public:
	/// @brief Default constructor, default
	FORCE_INLINE Atomic() = default;

	/// @brief Value-constructor
	FORCE_INLINE Atomic(T val) : AtomicT<T>(val) {}

	/// @brief Returns a copy of the underlying object
	FORCE_INLINE operator T() const { return this->load(); }

	/// @brief Assignment-operator, assign underlying object
	FORCE_INLINE T operator=(T val)
	{
		this->store(val);
		return val;
	}

private:
	/// @brief Copy/Move-constructor, removed
	/// @{
	Atomic(const Atomic<T>&) = delete;
	Atomic(Atomic<T>&&) = delete;
	/// @}

	/// @brief Copy/Move-assignment, removed
	/// @{
	Atomic<T> & operator=(const Atomic<T>&) = delete;
	Atomic<T> & operator=(Atomic<T>&&) = delete;
	/// @}
};

