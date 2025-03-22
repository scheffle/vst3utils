//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#include "vst3utils/message.h"
#include "public.sdk/source/vst/hosting/hostclasses.h"
#include <gtest/gtest.h>

//------------------------------------------------------------------------
namespace vst3utils {

//------------------------------------------------------------------------
TEST (message_test, valid_test)
{
	message msg (Steinberg::owned (new Steinberg::Vst::HostMessage ()));
	EXPECT_TRUE (msg.is_valid ());

	message msg2 (nullptr);
	EXPECT_FALSE (msg2.is_valid ());
}

//------------------------------------------------------------------------
TEST (message_test, id_test)
{
	message msg (Steinberg::owned (new Steinberg::Vst::HostMessage ()));
	EXPECT_TRUE (msg.get_id ().empty ());

	msg.set_id ("my message");
	EXPECT_STREQ ("my message", (*msg).getMessageID ());
	EXPECT_STREQ ("my message", msg.get_id ().data ());
}

//------------------------------------------------------------------------
TEST (message_test, attribute_list_valid_test)
{
	message msg (Steinberg::owned (new Steinberg::Vst::HostMessage ()));
	EXPECT_TRUE (msg.get_attributes ().is_valid ());

	message msg2 (nullptr);
	EXPECT_FALSE (msg2.get_attributes ().is_valid ());
}

//------------------------------------------------------------------------
} // vst3utils
