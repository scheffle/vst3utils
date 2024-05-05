/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#include "vst3utils/string_conversion.h"
#include <gtest/gtest.h>
#include <string>

//------------------------------------------------------------------------
namespace vst3utils {

//------------------------------------------------------------------------
TEST (string_conversion_test, copy_utf16_to_ascii)
{
	auto utfStr = u"This is a string with á few unícode chäractérs";
	auto result = copy_utf16_to_ascii (utfStr);

	EXPECT_EQ (result, "This is a string with  few uncode chractrs");

	utfStr = u"This is a string with á few unícode chäractérs";
	result = copy_utf16_to_ascii<'*'> (utfStr);

	EXPECT_EQ (result, "This is a string with * few un*code ch*ract*rs");
}

//------------------------------------------------------------------------
TEST (string_conversion_test, create_utf16_from_ascii)
{
	auto asciiStr = "This is a ASCII string";
	auto result = create_utf16_from_ascii (asciiStr);

	EXPECT_EQ (result, u"This is a ASCII string");
}

//------------------------------------------------------------------------
TEST (string_conversion_test, copy_ascii_to_utf16)
{
	auto asciiStr = "This is a ASCII string";
	{
		char16_t result[20];
		copy_ascii_to_utf16 (asciiStr, std::begin (result), std::end (result));
		std::u16string str (result);
		EXPECT_EQ (str, u"This is a ASCII str");
	}
	{
		char16_t result[40];
		copy_ascii_to_utf16 (asciiStr, std::begin (result), std::end (result));
		std::u16string str (result);
		EXPECT_EQ (str, u"This is a ASCII string");
	}
	{
		char16_t result[23];
		copy_ascii_to_utf16 (asciiStr, std::begin (result), std::end (result));
		std::u16string str (result);
		EXPECT_EQ (str, u"This is a ASCII string");
	}
	{
		char16_t result[22];
		copy_ascii_to_utf16 (asciiStr, std::begin (result), std::end (result));
		std::u16string str (result);
		EXPECT_EQ (str, u"This is a ASCII strin");
	}
	{
		auto emptyAsciiString = "";
		char16_t result[1];
		copy_ascii_to_utf16 (emptyAsciiString, std::begin (result), std::end (result));
		std::u16string str (result);
		EXPECT_EQ (str, u"");
	}
}

//------------------------------------------------------------------------
} // vst3utils
