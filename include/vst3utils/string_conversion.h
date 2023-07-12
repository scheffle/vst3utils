//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#pragma once

#include <cassert>
#include <string>
#include <string_view>

//------------------------------------------------------------------------
namespace vst3utils {

//------------------------------------------------------------------------
/** copy UTF-16 string to an ASCII string
 *
 *  all non ASCII characters are removed in this conversion
 */
inline std::string copy_utf16_to_ascii (std::u16string_view str)
{
	std::string result;
	std::for_each (str.begin (), str.end (), [&result] (const auto& c) {
		if (c >= 0 && c <= 127)
			result.push_back (static_cast<char> (c));
	});
	return result;
}

//------------------------------------------------------------------------
/** create UTF-16 string from ASCII string */
inline std::u16string create_utf16_from_ascii (std::string_view ascii)
{
	std::u16string result;
	std::for_each (ascii.begin (), ascii.end (),
				   [&result] (const auto& c) { result.push_back (static_cast<char16_t> (c)); });
	return result;
}

//------------------------------------------------------------------------
/** copy ASCII string to UTF-16 buffer */
inline void copy_ascii_to_utf16 (std::string_view ascii, char16_t* start, char16_t* end)
{
	assert (start < end);
	if (start > end)
		return;

	auto src = ascii.data ();
	auto srcEnd = src + ascii.size ();
	while (start != end)
	{
		if (src == srcEnd)
			break;
		*start = *src;
		++start;
		++src;
	}
	while (start != end)
	{
		*start = 0;
		++start;
	}
}

//------------------------------------------------------------------------
} // vst3utils
