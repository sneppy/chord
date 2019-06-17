#pragma once

#include "containers_fwd.h"
#include "array.h"
#include "hal/platform_string.h"

/**
 * @class String containers/string.h
 * @brief Base string class
 * 
 * A dynamic size string
 */
class String
{
protected:
	/// Underlying data
	Array<ansichar> data;

public:
	/// Default constructor
	FORCE_INLINE String() :
		data(16) {}
	
protected:
	/// A protected constructor that initializes an empty buffer
	explicit FORCE_INLINE String(uint64 n) :
		data(n) {}

public:
	/// String constructor
	FORCE_INLINE String(const ansichar * string, uint32 len = 0)
		: data((len ? len : PlatformString::strlen(string)) + 1)
	{
		if (string)
		{
			data.count = data.size - 1;
			moveOrCopy(data.buffer, string, data.count);

			// Teminate string
			data[data.count] = '\0';
		}
	}

	/// Copy constructor
	FORCE_INLINE String(const String & other)
		: data(other.data.count + 1)
	{
		// Copy content
		PlatformMemory::memcpy(data.buffer, other.data.buffer, (data.count = other.data.count) + 1);
	}

	/// Provides access to underying data
	/// @{
	FORCE_INLINE ansichar *			operator*()			{ return data.buffer; }
	FORCE_INLINE const ansichar *	operator*() const	{ return data.buffer; }
	/// @}

	/// Random access operator
	/// @{
	FORCE_INLINE ansichar &			operator[](uint64 i)		{ return data.buffer[i]; }
	FORCE_INLINE const ansichar &	operator[](uint64 i) const	{ return data.buffer[i]; }
	/// @}

	/// Returns string length (without null terminating character)
	FORCE_INLINE uint64 getLength() const { return data.count; }

	/// Returns underlying array
	/// @{
	FORCE_INLINE const Array<ansichar> &	getArray() const	{ return data; }
	FORCE_INLINE Array<ansichar> &			getArray()			{ return data; }
	/// @}

	/**
	 * Compare with another string
	 * 
	 * @param [in] s,s1,s2 string operands
	 * @return distance of first different characters (zero if equals)
	 * @{
	 */
	FORCE_INLINE int32 compare(const String & s) const		{ return PlatformString::strcmp(**this, *s); }
	FORCE_INLINE int32 compare(const ansichar * s) const	{ return PlatformString::strcmp(**this, s); }

	friend FORCE_INLINE int32 compare(const ansichar * s1, const String & s2) { return s2.compare(s1); }
	/// @}

	/// Like @copydoc compare() but case insensitive
	/// @{
	FORCE_INLINE int32 comparei(const String & s) const		{ return PlatformString::strcmp(**this, *s); }
	FORCE_INLINE int32 comparei(const ansichar * s) const	{ return PlatformString::strcmp(**this, s); }

	friend FORCE_INLINE int32 comparei(const ansichar * s1, const String & s2) { return s2.comparei(s1); }
	/// @}

	/**
	 * String-string comparison operators
	 * 
	 * @param [in] s string operand
	 * @return comparison result
	 * @{
	 */
	FORCE_INLINE bool operator==(const String & s) const	{ return compare(s) == 0; }
	FORCE_INLINE bool operator!=(const String & s) const	{ return compare(s) != 0; }
	FORCE_INLINE bool operator< (const String & s) const	{ return compare(s) < 0; }
	FORCE_INLINE bool operator> (const String & s) const	{ return compare(s) > 0; }
	FORCE_INLINE bool operator<=(const String & s) const	{ return compare(s) <= 0; }
	FORCE_INLINE bool operator>=(const String & s) const	{ return compare(s) >= 0; }

	friend FORCE_INLINE bool operator==(const ansichar * s1, const String & s2)	{ return s2.compare(s1) == 0; }
	friend FORCE_INLINE bool operator!=(const ansichar * s1, const String & s2)	{ return s2.compare(s1) != 0; }
	friend FORCE_INLINE bool operator< (const ansichar * s1, const String & s2) { return s2.compare(s1) > 0; }
	friend FORCE_INLINE bool operator> (const ansichar * s1, const String & s2) { return s2.compare(s1) < 0; }
	friend FORCE_INLINE bool operator<=(const ansichar * s1, const String & s2)	{ return s2.compare(s1) >= 0; }
	friend FORCE_INLINE bool operator>=(const ansichar * s1, const String & s2)	{ return s2.compare(s1) <= 0; }
	/// @}

	/**
	 * Append a single character
	 * 
	 * @param [in] c character
	 * @return self
	 */
	FORCE_INLINE String & operator+=(ansichar c)
	{
		data.resizeIfNecessary(data.count + 2);
		data.buffer[data.count++] = c;
		data.buffer[data.count] = '\0';

		return *this;
	}

	/**
	 * Append another string
	 * 
	 * @param [in] s string operand
	 * @param [in] n string length
	 * @return self
	 * @{
	 */
	FORCE_INLINE String & append(const ansichar * s, sizet n)
	{
		data.resizeIfNecessary(data.count + n);
		PlatformMemory::memcpy(data.buffer + data.count, s, n);
		data.buffer[data.count += n] = '\0';

		return *this;
	}
	FORCE_INLINE String & operator+=(const ansichar * s)	{ return append(s, PlatformString::strlen(s)); }
	FORCE_INLINE String & operator+=(const String & s)		{ return append(*s, s.data.count); }
	/// @}

	/**
	 * Concatenates two strings
	 * 
	 * @param [in] s string operand
	 * @return new string
	 * @{
	 */
	FORCE_INLINE String concat(const ansichar * s, uint64 n) const
	{
		// Create a capable buffer
		const uint64 len = data.count + n;
		String out(len + 1);

		// Copy content
		PlatformMemory::memcpy(out.data.buffer, data.buffer, data.count),
		PlatformMemory::memcpy(out.data.buffer + data.count, s, n);

		out.data.buffer[len] = '\0';
		out.data.count = len;

		return out;
	}
	FORCE_INLINE String operator+(const ansichar * s) const	{ return concat(s, PlatformString::strlen(s)); }
	FORCE_INLINE String operator+(const String & s) const	{ return concat(*s, s.data.count); }
	/// @}

	/**
	 * Create an empty string with length len
	 * 
	 * @param [in] len string length
	 * @return created string
	 */
	static FORCE_INLINE String createEmpty(uint32 len)
	{
		String out(len + 1);

		// Null-terminate
		out.data.count = len;
		out.data[len] = '\0';

		return out;
	}

	/**
	 * Extract substring
	 * 
	 * @param [in] begin begin (inclusive) of substring
	 * @param [in] end end (exclusive) of substring
	 * @return new string
	 * @{
	 */
	FORCE_INLINE String substring(uint32 begin, uint32 end)
	{
		return String(data.buffer + begin, end - begin);
	}
	FORCE_INLINE String substring(uint32 begin)
	{
		return substring(begin, data.count);
	}
	/// @}
};

