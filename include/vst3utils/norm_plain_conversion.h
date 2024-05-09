//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#pragma once

#include "../../gcem/include/gcem.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>

//------------------------------------------------------------------------
namespace vst3utils {

//------------------------------------------------------------------------
template<typename T>
inline constexpr T normalized_to_plain (T min, T max, T normalized_value) noexcept
{
	return normalized_value * (max - min) + min;
}

//------------------------------------------------------------------------
template<typename return_t = int32_t, typename value_t = double>
inline constexpr return_t normalized_to_steps (int32_t num_steps, int32_t start_value,
											   value_t normalized_value) noexcept
{
	return static_cast<return_t> (
		std::min (num_steps,
				  static_cast<int32_t> (normalized_value * static_cast<value_t> (num_steps + 1))) +
		start_value);
}

//------------------------------------------------------------------------
template<typename T>
inline constexpr T normalized_to_exp (T min, T max, T normalized_value) noexcept
{
	auto min_is_zero = min == static_cast<T> (0);
	if (min_is_zero)
		min += std::numeric_limits<T>::epsilon ();
	auto result = min * gcem::exp (normalized_value * gcem::log (max / min));
	if (min_is_zero)
	{
		result -= std::numeric_limits<T>::epsilon ();
		min -= std::numeric_limits<T>::epsilon ();
	}
	result = std::clamp (result, min, max);
	return result;
}

//------------------------------------------------------------------------
template<typename return_t = double, typename value_t>
inline constexpr return_t plain_to_normalized (return_t min, return_t max,
											   value_t plain_value) noexcept
{
	return (plain_value - min) / (max - min);
}

//------------------------------------------------------------------------
template<typename return_t = double, typename value_t = double>
inline constexpr return_t steps_to_normalized (int32_t num_steps, int32_t start_value,
											   value_t plain_value) noexcept
{
	return static_cast<return_t> (plain_value - start_value) / static_cast<return_t> (num_steps);
}

//------------------------------------------------------------------------
template<typename T>
inline constexpr T exp_to_normalized (T min, T max, T plain_value) noexcept
{
	auto min_is_zero = min == static_cast<T> (0);
	if (min_is_zero)
		min += std::numeric_limits<T>::epsilon ();
	auto result = gcem::log (plain_value / min) / gcem::log (max / min);
	result = std::clamp (result, 0., 1.);
	return result;
}

//------------------------------------------------------------------------
template<typename T>
inline constexpr T gain_to_db (T gain_value) noexcept
{
	return static_cast<T> (20. * gcem::log10 (gain_value));
}

//------------------------------------------------------------------------
template<typename T>
inline constexpr T db_to_gain (T db_value) noexcept
{
	constexpr const auto inv20 = static_cast<T> (1. / 20.);
	return static_cast<T> (gcem::pow (10, db_value * inv20));
}

//------------------------------------------------------------------------
} // vst3utils
