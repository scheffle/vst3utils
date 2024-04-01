//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#include "vst3utils/observable.h"
#include <gtest/gtest.h>
#include <string>

//------------------------------------------------------------------------
namespace vst3utils {

using namespace std::string_literals;
using namespace std::string_view_literals;

using obstring = observable<std::string>;
using obstring_token = observable_token_ptr<std::string>;

//------------------------------------------------------------------------
TEST (observable_test, listener_callback)
{
	obstring str;

	bool listener1_called = false;
	bool listener2_called = false;

	auto token1 = str.add_listener ([&] (const auto& str) {
		EXPECT_EQ (str, "test"s);
		listener1_called = true;
	});
	auto token2 = str.add_listener ([&] (const auto& str) {
		EXPECT_EQ (str, "test"s);
		listener2_called = true;
	});

	str.edit ([] (auto& str) {
		str = "test"s;
		return true;
	});

	EXPECT_TRUE (listener1_called);
	EXPECT_TRUE (listener2_called);
}

//------------------------------------------------------------------------
TEST (observable_test, obj_destroyed_callback)
{
	obstring_token token;
	bool object_destroyed_called = false;

	{
		obstring str;
		token = str.add_listener ([] (const auto&) {});
		token->set_object_destroyed_callback ([&] () { object_destroyed_called = true; });
	}

	EXPECT_NE (token.get (), nullptr);
	EXPECT_TRUE (object_destroyed_called);
}

//------------------------------------------------------------------------
TEST (observable_test, listener_removed_while_notifying_listeners)
{
	obstring str;
	obstring_token token2;

	bool listener1_called = false;
	bool listener2_not_called = true;

	auto token1 = str.add_listener ([&] (const auto& str) {
		listener1_called = true;
		token2.reset ();
	});
	token2 = str.add_listener ([&] (const auto& str) { listener2_not_called = false; });

	str.edit ([] (auto& str) { return true; });

	EXPECT_TRUE (listener1_called);
	EXPECT_TRUE (listener2_not_called);
}

//------------------------------------------------------------------------
} // vst3utils
