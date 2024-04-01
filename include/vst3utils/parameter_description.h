//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#pragma once

#include "vst3utils/norm_plain_conversion.h"
#include <variant>
#include <array>

//------------------------------------------------------------------------
namespace vst3utils {
namespace param {

using convert_func = double (*) (double norm_value);

//------------------------------------------------------------------------
/** Parameter Range description

describes a parameter range between a minimum and maximum value

 */
struct range
{
	double min {0.};
	double max {1.};
	uint32_t precision {1};
	const char16_t* unit {nullptr};
};

//------------------------------------------------------------------------
/** Parameter step count description

describes a parameter with a concret number of steps. if a stringList is provided the number of
strings must be equal to the number of steps.

 */
struct step_count
{
	uint32_t num_steps {0};
	int32_t start_value {0};
	const char16_t* unit {nullptr};
	const char16_t* const* string_list {nullptr};
};

//------------------------------------------------------------------------
/** Parameter convert functions
 */
struct convert_functions
{
	const convert_func to_plain {nullptr};
	const convert_func to_normalized {nullptr};
};

//------------------------------------------------------------------------
/** parameter description

describes a parameter with a name, the default normalized value, if it is a range or step count
parameter and it's optional convert functions from/to plain/normalized.

 */
struct description
{
	const char16_t* name {nullptr};
	const double default_normalized {0.};
	const std::variant<step_count, range> range_or_step_count {range {}};
	const convert_functions convert {};
};

//------------------------------------------------------------------------
static const constexpr std::array strings_on_off = {u"off", u"on"};

//------------------------------------------------------------------------
template<typename array_t>
inline constexpr step_count make_step_count (array_t&& a, int32_t start_value = 0,
											 const char16_t* unit = nullptr)
{
	return {static_cast<uint32_t> (a.size () - 1), start_value, unit, a.data ()};
}

//------------------------------------------------------------------------
template<int32_t min, int32_t max>
inline constexpr convert_func make_normalized_to_plain_func ()
{
	return [] (double v) {
		return normalized_to_plain (static_cast<double> (min), static_cast<double> (max), v);
	};
}

//------------------------------------------------------------------------
template<int32_t num_steps, int32_t start_value = 0>
inline constexpr convert_func make_normalized_to_steps_func ()
{
	return [] (double v) -> double {
		return normalized_to_steps (num_steps, start_value, v);
	};
}

//------------------------------------------------------------------------
template<int32_t min, int32_t max>
inline constexpr convert_func make_normalized_to_exp_func ()
{
	return [] (double v) {
		return normalized_to_exp (static_cast<double> (min), static_cast<double> (max), v);
	};
}

//------------------------------------------------------------------------
template<int32_t min, int32_t max>
inline constexpr convert_func make_plain_to_normalized_func ()
{
	return [] (double v) {
		return plain_to_normalized (static_cast<double> (min), static_cast<double> (max), v);
	};
}

//------------------------------------------------------------------------
template<int32_t num_steps, int32_t start_value = 0>
inline constexpr convert_func make_steps_to_normalized_func ()
{
	return [] (double v) -> double {
		return steps_to_normalized (num_steps, start_value, v);
	};
}

//------------------------------------------------------------------------
template<int32_t min, int32_t max>
inline constexpr convert_func make_exp_to_normalized_func ()
{
	return [] (double v) {
		return exp_to_normalized (static_cast<double> (min), static_cast<double> (max), v);
	};
}

//------------------------------------------------------------------------
inline constexpr convert_func make_normalized_to_db_func ()
{
	return [] (double v) {
		return gain_to_db (v);
	};
}

//------------------------------------------------------------------------
inline constexpr convert_func make_db_to_normalized_func ()
{
	return [] (double v) {
		return db_to_gain (v);
	};
}

//------------------------------------------------------------------------
template<int32_t min, int32_t max>
inline constexpr convert_functions linear_functions ()
{
	return {make_normalized_to_plain_func<min, max> (), make_plain_to_normalized_func<min, max> ()};
}

//------------------------------------------------------------------------
template<int32_t min, int32_t max>
inline constexpr convert_functions exponent_functions ()
{
	return {make_normalized_to_exp_func<min, max> (), make_exp_to_normalized_func<min, max> ()};
}

//------------------------------------------------------------------------
template<int32_t num_steps, int32_t start_value = 0>
inline constexpr convert_functions steps_functions ()
{
	return {make_normalized_to_steps_func<num_steps, start_value> (),
			make_steps_to_normalized_func<num_steps, start_value> ()};
}

//------------------------------------------------------------------------
inline constexpr convert_functions db_functions ()
{
	return {make_normalized_to_db_func (), make_db_to_normalized_func ()};
}

//------------------------------------------------------------------------
template<typename array_t>
inline constexpr description list_description (const char16_t* name, uint32_t default_index,
											   const array_t& a)
{
	return {name,
			static_cast<double> (default_index) /
				(sizeof (a) / sizeof (typename array_t::pointer) - 1),
			{make_step_count (a)},
			steps_functions<sizeof (a) / sizeof (typename array_t::pointer) - 1> ()};
}

//------------------------------------------------------------------------
inline constexpr description range_description (const char16_t* name, double default_plain,
												convert_functions f, uint32_t precision = 1,
												const char16_t* unit = nullptr)
{
	return {name, f.to_normalized (default_plain),
			range {f.to_plain (0.), f.to_plain (1.), precision, unit}, f};
}

//------------------------------------------------------------------------
inline constexpr description steps_description (const char16_t* name, double default_plain,
												convert_functions f, const char16_t* unit = nullptr)
{
	return {name, f.to_normalized (default_plain),
			step_count {static_cast<uint32_t> (f.to_plain (1.) - f.to_plain (0.)),
						static_cast<int32_t> (f.to_plain (0.)), unit},
			f};
}

//------------------------------------------------------------------------
} // param
} // vst3utils
