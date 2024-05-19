//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/vst/ivstevents.h"

//------------------------------------------------------------------------
namespace vst3utils {

//------------------------------------------------------------------------
template<typename return_t>
struct event_handler;

//------------------------------------------------------------------------
/** dispatch an event to an event handler
 *
 *	@param event the event
 *	@param handler the event handler to dispatch the event
 *	@return result from calling the event handler
 */
template<typename return_t>
inline return_t dispatch_event (Steinberg::Vst::Event& event, event_handler<return_t>& handler);

//------------------------------------------------------------------------
template<typename return_t>
struct event_handler
{
	using event = Steinberg::Vst::Event;
	using note_on_event = Steinberg::Vst::NoteOnEvent;
	using note_off_event = Steinberg::Vst::NoteOffEvent;
	using data_event = Steinberg::Vst::DataEvent;
	using poly_pressure_event = Steinberg::Vst::PolyPressureEvent;
	using note_expression_value_event = Steinberg::Vst::NoteExpressionValueEvent;
	using note_expression_text_event = Steinberg::Vst::NoteExpressionTextEvent;
	using chord_event = Steinberg::Vst::ChordEvent;
	using scale_event = Steinberg::Vst::ScaleEvent;

	virtual return_t on_note_on (const event& event, const note_on_event& note_on) = 0;
	virtual return_t on_note_off (const event& event, const note_off_event& note_off) = 0;
	virtual return_t on_data (const event& event, const data_event& data) = 0;
	virtual return_t on_poly_pressure (const event& event,
									   const poly_pressure_event& poly_pressure) = 0;
	virtual return_t on_note_expression_value (
		const event& event, const note_expression_value_event& note_expression_value) = 0;
	virtual return_t on_note_expression_text (
		const event& event, const note_expression_text_event& note_expression_text) = 0;
	virtual return_t on_chord (const event& event, const chord_event& chord) = 0;
	virtual return_t on_scale (const event& event, const scale_event& scale) = 0;
	virtual return_t on_unknown_event (const event& event) = 0;
};

//------------------------------------------------------------------------
template<typename return_t>
inline return_t dispatch_event (Steinberg::Vst::Event& event, event_handler<return_t>& handler)
{
	using Type = Steinberg::Vst::Event::EventTypes;

	switch (event.type)
	{
		case Type::kNoteOnEvent:
			return handler.on_note_on (event, event.noteOn);
		case Type::kNoteOffEvent:
			return handler.on_note_off (event, event.noteOff);
		case Type::kDataEvent:
			return handler.on_data (event, event.data);
		case Type::kPolyPressureEvent:
			return handler.on_poly_pressure (event, event.polyPressure);
		case Type::kNoteExpressionValueEvent:
			return handler.on_note_expression_value (event, event.noteExpressionValue);
		case Type::kNoteExpressionTextEvent:
			return handler.on_note_expression_text (event, event.noteExpressionText);
		case Type::kChordEvent:
			return handler.on_chord (event, event.chord);
		case Type::kScaleEvent:
			return handler.on_scale (event, event.scale);
	}
	return handler.on_unknown_event (event);
}

//------------------------------------------------------------------------
template<typename return_t, return_t default_return_value>
struct event_handler_adapter : event_handler<return_t>
{
	using event = Steinberg::Vst::Event;
	using note_on_event = Steinberg::Vst::NoteOnEvent;
	using note_off_event = Steinberg::Vst::NoteOffEvent;
	using data_event = Steinberg::Vst::DataEvent;
	using poly_pressure_event = Steinberg::Vst::PolyPressureEvent;
	using note_expression_value_event = Steinberg::Vst::NoteExpressionValueEvent;
	using note_expression_text_event = Steinberg::Vst::NoteExpressionTextEvent;
	using chord_event = Steinberg::Vst::ChordEvent;
	using scale_event = Steinberg::Vst::ScaleEvent;

	return_t on_note_on (const event& event, const note_on_event& note_on) override
	{
		return default_return_value;
	}
	return_t on_note_off (const event& event, const note_off_event& note_off) override
	{
		return default_return_value;
	}
	return_t on_data (const event& event, const data_event& data) override
	{
		return default_return_value;
	}
	return_t on_poly_pressure (const event& event,
							   const poly_pressure_event& poly_pressure) override
	{
		return default_return_value;
	}
	return_t on_note_expression_value (
		const event& event, const note_expression_value_event& note_expression_value) override
	{
		return default_return_value;
	}
	return_t on_note_expression_text (
		const event& event, const note_expression_text_event& note_expression_text) override
	{
		return default_return_value;
	}
	return_t on_chord (const event& event, const chord_event& chord) override
	{
		return default_return_value;
	}
	return_t on_scale (const event& event, const scale_event& scale) override
	{
		return default_return_value;
	}
	return_t on_unknown_event (const event& event) override { return default_return_value; }
};

//------------------------------------------------------------------------
using default_event_handler = event_handler_adapter<bool, false>;

//------------------------------------------------------------------------
} // vst3utils
