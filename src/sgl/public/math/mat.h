#pragma once

#include "core_types.h"
#include "hal/platform_math.h"

/**
 * A fixed-size mathematical matrix
 */
template<typename T, uint32 N, uint32 M>
struct Mat
{
public:
	union
	{
		/// Matrix data
		T matrix[N][M];

		/// Array data
		T array[N * M];
	}
};