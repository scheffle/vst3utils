//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#pragma once

#include <cassert>
#include <type_traits>

//------------------------------------------------------------------------
namespace vst3utils {

//------------------------------------------------------------------------
/** smooth value

a value class where the value converges from one value to another

 */
template<typename T>
struct smooth_value
{
	static_assert (std::is_floating_point_v<T>, "Must be a floating point type");
	static const constexpr T alpha_min = static_cast<T> (0.);
	static const constexpr T alpha_max = static_cast<T> (1.);

	smooth_value (T initalValue = {}, T alpha = 0.1) : value (initalValue), alpha (alpha) { flush (); }
	smooth_value (const smooth_value&) = default;
	smooth_value (smooth_value&&) = default;
	smooth_value& operator= (const smooth_value&) = default;
	smooth_value& operator= (smooth_value&&) = default;

	/** smooth the parameter further and return the smoothed value */
	inline T process () noexcept
	{
		smoothed_value = alpha * value + (alpha_max - alpha) * smoothed_value;
		return smoothed_value;
	}

	/** set the value to be reached */
	inline void set (T v) noexcept { value = v; }
	/** get the value to be reached */
	inline T get () const noexcept { return value; }

	/** get the smoothed value */
	inline T operator* () noexcept { return smoothed_value; }
	/** get the smoothed value */
	inline operator T () noexcept { return smoothed_value; }

	/** set the value to be reached */
	inline smooth_value& operator= (T v) noexcept
	{
		set (v);
		return *this;
	}

	/** set alpha
	 *
	 *	sets the factor of how quickly the value approaches the actual value.
	 *	the higher the factor the faster the actual value will be reached.
	 *
	 *	@param v smoothing factor [0..1]
	 */
	inline void set_alpha (T v) noexcept
	{
		assert (v >= alpha_min && v <= alpha_max);
		alpha = v;
	}

	/** set the smoothed value to the actual value */
	inline void flush () noexcept { smoothed_value = value; }

	/** set the smoothed value and the actual value */
	inline void set_flushed (T v) noexcept { smoothed_value = value = v; }

private:
	T alpha {0.1};
	T value {0.};
	T smoothed_value {0.};
};

//------------------------------------------------------------------------
} // vst3utils
