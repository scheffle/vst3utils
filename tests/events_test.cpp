//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#include "vst3utils/events.h"
#include <gtest/gtest.h>

//------------------------------------------------------------------------
namespace vst3utils {

using event_type = Steinberg::Vst::Event::EventTypes;

//------------------------------------------------------------------------
TEST (events_test, dispatch_default_return)
{
	default_event_handler handler;

	Steinberg::Vst::Event e {};
	for (uint32_t type = event_type::kNoteOnEvent; type <= event_type::kLegacyMIDICCOutEvent;
		 ++type)
	{
		e.type = type;
		EXPECT_FALSE (dispatch_event (e, handler));
	}
}

//------------------------------------------------------------------------
TEST (events_test, note_on_event)
{
	Steinberg::Vst::Event e {};
	e.type = event_type::kNoteOnEvent;

	struct eh final : default_event_handler
	{
		bool on_note_on (const event& event, const note_on_event& note_on) final override
		{
			return true;
		}
	};

	eh handler;
	EXPECT_TRUE (dispatch_event (e, handler));
}

//------------------------------------------------------------------------
TEST (events_test, note_off_event)
{
	Steinberg::Vst::Event e {};
	e.type = event_type::kNoteOffEvent;

	struct eh final : default_event_handler
	{
		bool on_note_off (const event& event, const note_off_event& note_off) final override
		{
			return true;
		}
	};

	eh handler;
	EXPECT_TRUE (dispatch_event (e, handler));
}

//------------------------------------------------------------------------
TEST (events_test, data_event)
{
	Steinberg::Vst::Event e {};
	e.type = event_type::kDataEvent;

	struct eh final : default_event_handler
	{
		bool on_data (const event& event, const data_event& data) final override { return true; }
	};

	eh handler;
	EXPECT_TRUE (dispatch_event (e, handler));
}

//------------------------------------------------------------------------
TEST (events_test, poly_pressure_event)
{
	Steinberg::Vst::Event e {};
	e.type = event_type::kPolyPressureEvent;

	struct eh final : default_event_handler
	{
		bool on_poly_pressure (const event& event, const poly_pressure_event& ppe) final override
		{
			return true;
		}
	};

	eh handler;
	EXPECT_TRUE (dispatch_event (e, handler));
}

//------------------------------------------------------------------------
TEST (events_test, note_expression_value_event)
{
	Steinberg::Vst::Event e {};
	e.type = event_type::kNoteExpressionValueEvent;

	struct eh final : default_event_handler
	{
		bool on_note_expression_value (const event& event,
									   const note_expression_value_event& neve) final override
		{
			return true;
		}
	};

	eh handler;
	EXPECT_TRUE (dispatch_event (e, handler));
}

//------------------------------------------------------------------------
TEST (events_test, note_expression_text_event)
{
	Steinberg::Vst::Event e {};
	e.type = event_type::kNoteExpressionTextEvent;

	struct eh final : default_event_handler
	{
		bool on_note_expression_text (const event& event,
									  const note_expression_text_event& nete) final override
		{
			return true;
		}
	};

	eh handler;
	EXPECT_TRUE (dispatch_event (e, handler));
}

//------------------------------------------------------------------------
TEST (events_test, chord_event)
{
	Steinberg::Vst::Event e {};
	e.type = event_type::kChordEvent;

	struct eh final : default_event_handler
	{
		bool on_chord (const event& event, const chord_event& ce) final override { return true; }
	};

	eh handler;
	EXPECT_TRUE (dispatch_event (e, handler));
}

//------------------------------------------------------------------------
TEST (events_test, scale_event)
{
	Steinberg::Vst::Event e {};
	e.type = event_type::kScaleEvent;

	struct eh final : default_event_handler
	{
		bool on_scale (const event& event, const scale_event& se) final override { return true; }
	};

	eh handler;
	EXPECT_TRUE (dispatch_event (e, handler));
}

//------------------------------------------------------------------------
TEST (events_test, unknown_event)
{
	Steinberg::Vst::Event e {};
	e.type = event_type::kScaleEvent + 1;

	struct eh final : default_event_handler
	{
		bool on_unknown_event (const event& event) final override { return true; }
	};

	eh handler;
	EXPECT_TRUE (dispatch_event (e, handler));
}

//------------------------------------------------------------------------
} // vst3utils
