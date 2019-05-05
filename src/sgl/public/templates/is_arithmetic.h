#pragma once

#include "core_types.h"

/// Sets member 'value' to true if arithmetic type
/// false otherwise
template<typename T>	struct IsArithmetic				{ enum {value = false}; };

template				struct IsArithmetic<int8>		{ enum {value = true}; };
template				struct IsArithmetic<int16>		{ enum {value = true}; };
template				struct IsArithmetic<int32>		{ enum {value = true}; };
template				struct IsArithmetic<int64>		{ enum {value = true}; };

template				struct IsArithmetic<uint8>		{ enum {value = true}; };
template				struct IsArithmetic<uint16>		{ enum {value = true}; };
template				struct IsArithmetic<uint32>		{ enum {value = true}; };
template				struct IsArithmetic<uint64>		{ enum {value = true}; };

template				struct IsArithmetic<float32>	{ enum {value = true}; };
template				struct IsArithmetic<float64>	{ enum {value = true}; };
template				struct IsArithmetic<float128>	{ enum {value = true}; };

/// Quick access
#define IsArithmeticV(T) IsArithmetic<T>::value