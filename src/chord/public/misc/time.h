#pragma once

#include "core_types.h"

//////////////////////////////////////////////////
// Time global variables
//////////////////////////////////////////////////

extern uint64 currTick;
extern uint64 prevTick;

/**
 * @struct Timer misc/timer.h
 */
struct Timer
{
protected:
	/// Time interval (seconds)
	float32 interval;

	/// Total elapsed time
	float32 elapsed;

	/// Delta elapsed time
	float32 delta;

public:
	/// Default constructor
	FORCE_INLINE Timer(float32 _interval, float32 _elapsed = 0.f)
		: interval(_interval)
		, elapsed(_elapsed)
		, delta(_elapsed)
	{
		// Fix delta time
		while (delta > interval) delta -= interval;
	}

	/// Returns current delta time
	FORCE_INLINE float32 getDelta() const
	{
		return delta;
	}

	/// Returns total elapsed time
	FORCE_INLINE float32 getElapsed() const
	{
		return elapsed;
	}

	/**
	 * Tick timer
	 * 
	 * @param [in] dt delta time
	 * @return true if expired
	 */
	FORCE_INLINE bool tick(float32 dt)
	{
		elapsed += dt, delta += dt;
		if (delta > interval)
		{
			// Reset delta
			delta -= interval;
			return true;
		}

		return false;
	}
};