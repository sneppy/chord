#pragma once

#include "coremin.h"
#include "chord.h"

namespace Chord
{
	/**
	 * A node request
	 */
	struct alignas(16) Request
	{
	public:
		/// Request type enum
		enum Type
		{
			PING = 0,
			REPLY,
			LOOKUP,
			LEAVE
		};
		
		/// Request type
		Type type : 8;

		/// Flags
		uint32 flags : 8;

		/// Request id
		uint32 id : 16;

		/// Destination operand
		ubyte dest[Math::max(20UL, sizeof(Ipv4))];

		/// Source operand
		ubyte src[Math::max(20UL, sizeof(Ipv4))];

		/// Sender address
		Ipv4 sender;

		/// Recipient address
		Ipv4 recipient;

		/// Request time to live (ttl)
		uint32 ttl : 16;

		/// Request hop count
		uint32 hopCount : 16;

	public:
		/// Returns whether request is expired
		FORCE_INLINE bool isExpired() const
		{
			return ttl > 0 && hopCount >= ttl;
		}

		/// Returns source operand
		/// @{
		template<typename T>
		FORCE_INLINE typename EnableIf<IsPointer<T>::value, T>::Type			getSrc()		{ return reinterpret_cast<T>(src); }
		template<typename T>
		FORCE_INLINE typename EnableIf<IsPointer<T>::value, const T>::Type		getSrc() const	{ return reinterpret_cast<const T>(src); }

		template<typename T>
		FORCE_INLINE typename EnableIf<!IsPointer<T>::value, T&>::Type			getSrc()		{ return *reinterpret_cast<T*>(src); }
		template<typename T>
		FORCE_INLINE typename EnableIf<!IsPointer<T>::value, const T&>::Type	getSrc() const	{ return *reinterpret_cast<const T*>(src); }
		/// @}

		/// Returns destination operand
		/// @{
		template<typename T>
		FORCE_INLINE typename EnableIf<IsPointer<T>::value, T>::Type			getDest()		{ return reinterpret_cast<T>(dest); }
		template<typename T>
		FORCE_INLINE typename EnableIf<IsPointer<T>::value, const T>::Type		getDest() const	{ return reinterpret_cast<const T>(dest); }

		template<typename T>
		FORCE_INLINE typename EnableIf<!IsPointer<T>::value, T&>::Type			getDest()		{ return *reinterpret_cast<T*>(dest); }
		template<typename T>
		FORCE_INLINE typename EnableIf<!IsPointer<T>::value, const T&>::Type	getDest() const	{ return *reinterpret_cast<const T*>(dest); }
		/// @}

		/**
		 * Sets value of source operand
		 * 
		 * @param [in] val source value
		 */
		template<typename T>
		FORCE_INLINE typename EnableIf<!IsPointer<T>::value, void>::Type setSrc(typename ConstRef<T>::Type val)
		{
			moveOrCopy(*reinterpret_cast<T*>(src), val);
		}

		/**
		 * Sets value of destination operand
		 * 
		 * @param [in] val destination value
		 */
		template<typename T>
		FORCE_INLINE typename EnableIf<!IsPointer<T>::value, void>::Type setDest(typename ConstRef<T>::Type val)
		{
			moveOrCopy(*reinterpret_cast<T*>(dest), val);
		}

		/// Do hop, increment count
		FORCE_INLINE Request & hop()
		{
			++hopCount;
			return *this;
		}

		/// Resets hop count
		FORCE_INLINE Request & reset()
		{
			hopCount = 0;
			return *this;
		}
	};
} // Chord
