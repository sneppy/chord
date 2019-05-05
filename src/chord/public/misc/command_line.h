#pragma once

#include "coremin.h"

/**
 * @class CommandLine utils/command_line.h
 * 
 * A command line parser
 */
class CommandLine : public Singleton<CommandLine>
{
protected:
	/// Parameter values
	Map<String, String> valueMap;

public:
	/// Default constructor
	CommandLine(int32 argc, char ** argv);

	/**
	 * Retrieves parameter value
	 * 
	 * @param [in] name parameter name
	 * @param [out] value out value
	 * @param [in] lambda function to process the string
	 * @returns true if value exists, false otherwise
	 * @{
	 */
	template<typename T, typename Lambda>
	inline bool getValue(const String & name, T & value, Lambda && lambda)
	{
		auto it = valueMap.find(name);
		if (it != valueMap.nil())
		{
			lambda(it->second, value);
			return true;
		}

		return false;
	}

	template<typename T>
	inline bool getValue(const String & name, T & value)
	{
		auto it = valueMap.find(name);
		if (it != valueMap.nil())
		{
			value = T(it->second);
			return true;
		}

		return false;
	}

	FORCE_INLINE bool getValue(const String & name)
	{
		return valueMap.find(name) != valueMap.nil();
	}
	/// @}
};

/// @ref getValue() specializations
/// @{
template<>
inline bool CommandLine::getValue(const String & name, String & value)
{
	auto it = valueMap.find(name);
	if (it != valueMap.nil())
	{
		value = it->second;
		return true;
	}

	return false;
}

template<>
FORCE_INLINE bool CommandLine::getValue(const String & name, int16 & value)
{
	auto it = valueMap.find(name);
	if (it != valueMap.nil())
	{
		value = atoi(*it->second);
		return true;
	}

	return false;
}

template<>
inline bool CommandLine::getValue(const String & name, int32 & value)
{
	auto it = valueMap.find(name);
	if (it != valueMap.nil())
	{
		value = atoi(*it->second);
		return true;
	}

	return false;
}

template<>
FORCE_INLINE bool CommandLine::getValue(const String & name, uint32 & value)
{
	return getValue(name, (int32&)value);
}

template<>
FORCE_INLINE bool CommandLine::getValue(const String & name, uint16 & value)
{
	return getValue(name, (int16&)value);
}

template<>
inline bool CommandLine::getValue(const String & name, float32 & value)
{
	auto it = valueMap.find(name);
	if (it != valueMap.nil())
	{
		value = atof(*it->second);
		return true;
	}

	return false;
}
/// @}