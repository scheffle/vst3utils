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
	return min * gcem::exp (normalized_value * gcem::log (max / min));
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
	return gcem::log (plain_value / min) / gcem::log (max / min);
}

//------------------------------------------------------------------------
} // vst3utils
