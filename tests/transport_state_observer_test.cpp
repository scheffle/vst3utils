//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#include "vst3utils/transport_state_observer.h"
#include <gtest/gtest.h>

//------------------------------------------------------------------------
namespace vst3utils {

//------------------------------------------------------------------------
struct process_context_mock
{
	uint32_t state {};
	int64_t projectTimeSamples {};
};

//------------------------------------------------------------------------
struct process_data_mock
{
	process_context_mock* processContext {};
	int32_t numSamples {};
};

//------------------------------------------------------------------------
TEST (transport_state_observer_test, state_changes)
{
	process_context_mock process_context;
	process_data_mock data {&process_context, 32};
	transport_state_observer observer;

	observer.update (data, [] (auto new_flags, auto old_flags, auto time_jump) { FAIL (); });

	process_context.state = transport_state_observer::playing;
	bool called = false;
	observer.update (data, [&] (auto new_flags, auto old_flags, auto time_jump) {
		EXPECT_EQ (new_flags, transport_state_observer::playing);
		EXPECT_EQ (old_flags, 0u);
		EXPECT_FALSE (time_jump);
		called = true;
	});
	EXPECT_TRUE (called);
	process_context.projectTimeSamples += data.numSamples;

	process_context.state |= transport_state_observer::cycle_active;
	called = false;
	observer.update (data, [&] (auto new_flags, auto old_flags, auto time_jump) {
		EXPECT_EQ (new_flags,
				   transport_state_observer::playing | transport_state_observer::cycle_active);
		EXPECT_EQ (old_flags, transport_state_observer::playing);
		EXPECT_FALSE (time_jump);
		called = true;
	});
	EXPECT_TRUE (called);
	process_context.projectTimeSamples += data.numSamples;

	process_context.state &= ~transport_state_observer::cycle_active;
	process_context.state |= transport_state_observer::recording;
	called = false;
	observer.update (data, [&] (auto new_flags, auto old_flags, auto time_jump) {
		EXPECT_EQ (new_flags,
				   transport_state_observer::playing | transport_state_observer::recording);
		EXPECT_EQ (old_flags,
				   transport_state_observer::playing | transport_state_observer::cycle_active);
		EXPECT_FALSE (time_jump);
		called = true;
	});
	EXPECT_TRUE (called);
	process_context.projectTimeSamples += data.numSamples;

	process_context.state = 0;
	called = false;
	observer.update (data, [&] (auto new_flags, auto old_flags, auto time_jump) {
		EXPECT_EQ (new_flags, 0u);
		EXPECT_EQ (old_flags,
				   transport_state_observer::playing | transport_state_observer::recording);
		EXPECT_FALSE (time_jump);
		called = true;
	});
	EXPECT_TRUE (called);

	process_context.state = transport_state_observer::playing;
	called = false;
	observer.update (data, [&] (auto new_flags, auto old_flags, auto time_jump) {
		EXPECT_EQ (new_flags, transport_state_observer::playing);
		EXPECT_EQ (old_flags, 0u);
		EXPECT_FALSE (time_jump);
		called = true;
	});
	EXPECT_TRUE (called);
	process_context.projectTimeSamples += data.numSamples;
}

//------------------------------------------------------------------------
TEST (transport_state_observer_test, time_jump)
{
	process_context_mock process_context {transport_state_observer::playing, 0};
	process_data_mock data {&process_context, 32};
	transport_state_observer observer;

	bool called = false;
	observer.update (data, [&] (auto new_flags, auto old_flags, auto time_jump) {
		EXPECT_EQ (new_flags, transport_state_observer::playing);
		EXPECT_EQ (old_flags, 0u);
		EXPECT_FALSE (time_jump);
		called = true;
	});
	EXPECT_TRUE (called);
	process_context.projectTimeSamples += data.numSamples;

	observer.update (data, [] (auto new_flags, auto old_flags, auto time_jump) { FAIL (); });
	process_context.projectTimeSamples += data.numSamples;

	process_context.projectTimeSamples += data.numSamples * 100; // time jump
	called = false;
	observer.update (data, [&] (auto new_flags, auto old_flags, auto time_jump) {
		EXPECT_EQ (new_flags, old_flags);
		EXPECT_TRUE (time_jump);
		called = true;
	});
	EXPECT_TRUE (called);
}

//------------------------------------------------------------------------
} // vst3utils
