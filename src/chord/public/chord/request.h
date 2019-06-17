#pragma once

#include "chord_fwd.h"
#include "templates/reference.h"

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
			NOTIFY,
			LEAVE,
			CHECK
		};
		
		/// Request type
		Type type : 8;

		/// Flags
		uint32 flags : 8;

		/// Request id
		uint32 id : 16;

		/// Destination operand
		ubyte dst[Math::max(20UL, sizeof(Ipv4))];

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
		FORCE_INLINE typename EnableIf<IsPointer<T>::value, T>::Type			getDst()		{ return reinterpret_cast<T>(dst); }
		template<typename T>
		FORCE_INLINE typename EnableIf<IsPointer<T>::value, const T>::Type		getDst() const	{ return reinterpret_cast<const T>(dst); }

		template<typename T>
		FORCE_INLINE typename EnableIf<!IsPointer<T>::value, T&>::Type			getDst()		{ return *reinterpret_cast<T*>(dst); }
		template<typename T>
		FORCE_INLINE typename EnableIf<!IsPointer<T>::value, const T&>::Type	getDst() const	{ return *reinterpret_cast<const T*>(dst); }
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
		FORCE_INLINE typename EnableIf<!IsPointer<T>::value, void>::Type setDst(typename ConstRef<T>::Type val)
		{
			moveOrCopy(*reinterpret_cast<T*>(dst), val);
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

	/**
	 * @struct RequestCallback chord/request.h
	 */
	struct RequestCallback
	{
	public:
		/// Callback types
		/// @{
		using CallbackT	= Function<void(const Request&)>;
		using ErrorT	= Function<void()>;
		/// @}

	public:
		/// Success callback
		const CallbackT onSuccess;

		/// Error callback
		const ErrorT onError;

	protected:
		/// Time to live
		float32 ttl;

		/// Request current age
		float32 age;

	public:
		/// Default constructor
		FORCE_INLINE RequestCallback()
			: onSuccess{nullptr}
			, onError{nullptr}
			, ttl{0.f}
			, age{0.f} {}
		
		/// Callback constructor
		explicit FORCE_INLINE RequestCallback(CallbackT && _onSuccess, ErrorT && _onError = nullptr, float32 _ttl = 2.f)
			: onSuccess{::move(_onSuccess)}
			, onError{::move(_onError)}
			, ttl{_ttl}
			, age{0.f} {}

		/**
		 * Tick request
		 * 
		 * @param [in] dt time elapsed from last tick
		 * @return true if request has expired
		 */
		FORCE_INLINE bool tick(float32 dt)
		{
			age += dt;
			if (ttl > 0.f && age > ttl)
			{
				age -= ttl;
				return true;
			}

			return false;
		}
	};
} // Chord
