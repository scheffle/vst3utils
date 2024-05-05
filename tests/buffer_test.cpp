//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#include "vst3utils/buffer.h"
#include <gtest/gtest.h>

//------------------------------------------------------------------------
namespace vst3utils {

//------------------------------------------------------------------------
TEST (buffer_test, size)
{
	buffer<double> b;
	b.allocate (100);
	EXPECT_EQ (b.size (), 100);
	buffer<double> b2 (100);
	EXPECT_EQ (b2.size (), 100);
}

//------------------------------------------------------------------------
TEST (buffer_test, resize)
{
	buffer<double> b;
	b.allocate (100);
	EXPECT_EQ (b.size (), 100);
	b.allocate (200);
	EXPECT_EQ (b.size (), 200);
}

//------------------------------------------------------------------------
TEST (buffer_test, fill)
{
	buffer<double> b (100);
	b.fill (1.5);
	for (auto i = 0u; i < b.size (); ++i)
	{
		EXPECT_DOUBLE_EQ (b[i], 1.5);
	}
}

//------------------------------------------------------------------------
TEST (buffer_test, iterator)
{
	buffer<double> b (100);
	b.fill (1.5);
	for (const auto& el : b)
	{
		EXPECT_DOUBLE_EQ (el, 1.5);
	}

	auto f = [] (const buffer<double>& buffer) {
		for (auto& el : buffer)
		{
			EXPECT_DOUBLE_EQ (el, 1.5);
		}
	};

	f (b);
}

//------------------------------------------------------------------------
} // vst3utils
