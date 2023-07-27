//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#pragma once

#include <array>

//------------------------------------------------------------------------
namespace vst3utils {

//------------------------------------------------------------------------
/** a std::array using an enum class for accessing the elements

Example:

	enum class color_component {
		red,
		green,
		blue,
		alpha,

		enum_end
	};

	using rgba_color = enum_array<uint8_t, color_component>;

	rgba_color color;
	rgba_color[color_component::red] = 255;

It is also possible to specify an offset where the enum values start

	enum class fruits {
		orange = 1000,
		apple,
		banana,

		enum_end
	};

	using fruits_array = enum_array<double, fruits, static_cast<size_t> (fruits::orange)>
	fruits_array f;
	f[fruits::apple] = 2.6;
 */
template<typename T, typename enum_t, size_t index_offset = 0u,
		 size_t array_size = static_cast<size_t> (enum_t::enum_end) - index_offset>
struct enum_array : std::array<T, array_size>
{
	using base_t = std::array<T, array_size>;

	static constexpr size_t count () noexcept { return array_size; }

	T& operator[] (enum_t index)
	{
		return base_t::operator[] (static_cast<size_t> (index) - index_offset);
	}
	const T& operator[] (enum_t index) const noexcept
	{
		return base_t::operator[] (static_cast<size_t> (index) - index_offset);
	}
	const T& operator[] (size_t index) const noexcept
	{
		return base_t::operator[] (index - index_offset);
	}
	T& operator[] (size_t index) noexcept { return base_t::operator[] (index - index_offset); }

	void set (size_t index, const T& value) noexcept
	{
		base_t::operator[] (index - index_offset) = value;
	}
};

//------------------------------------------------------------------------
} // vst3utils
