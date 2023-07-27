//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/vst/ivstevents.h"
#include <cstdint>

namespace vst3utils {

//------------------------------------------------------------------------
/** event iterator

c++ compatible Steinberg::Vst::Event forward iterator

the iterator always works on a copy of the event from the event list

Example:

	for (auto it = vst3utils::begin (eventList); it != vst3utils::end (eventList); ++it)
	{
		if (it->type == Steinberg::Vst::Event::kNoteOnEvent)
		{
			// ...
		}
	}

 */
struct event_iterator
{
	using IEventList = Steinberg::Vst::IEventList;
	using Event = Steinberg::Vst::Event;

	SMTG_ALWAYS_INLINE event_iterator () : event_list (nullptr), index (-1) {}
	SMTG_ALWAYS_INLINE event_iterator (IEventList& event_list)
	: event_list (&event_list), index (-1)
	{
	}
	SMTG_ALWAYS_INLINE event_iterator (IEventList& event_list, int32_t index)
	: event_list (&event_list), index (index)
	{
		update_event ();
	}

	SMTG_ALWAYS_INLINE bool operator== (const event_iterator& it) const
	{
		return index == it.index && it.event_list == event_list;
	}

	SMTG_ALWAYS_INLINE bool operator!= (const event_iterator& it) const
	{
		return index != it.index || it.event_list != event_list;
	}

	SMTG_ALWAYS_INLINE event_iterator& operator++ () noexcept
	{
		if (index >= 0)
			++index;
		update_event ();
		return *this;
	}

	SMTG_ALWAYS_INLINE event_iterator operator+= (size_t adv) noexcept
	{
		auto prev = *this;
		index += static_cast<int32_t> (adv);
		update_event ();
		return prev;
	}

	SMTG_ALWAYS_INLINE Event& operator* () { return e; }
	SMTG_ALWAYS_INLINE Event* operator->() { return &e; }

private:
	SMTG_ALWAYS_INLINE void update_event ()
	{
		if (!event_list || event_list->getEvent (index, e) != Steinberg::kResultTrue)
			index = -1;
	}

	Event e;
	int32_t index;
	IEventList* event_list;
};

//------------------------------------------------------------------------
SMTG_ALWAYS_INLINE event_iterator begin (Steinberg::Vst::IEventList* event_list)
{
	return event_list ? event_iterator (*event_list, 0) : event_iterator ();
}

//------------------------------------------------------------------------
SMTG_ALWAYS_INLINE event_iterator end (Steinberg::Vst::IEventList* event_list)
{
	return event_list ? event_iterator (*event_list, -1) : event_iterator ();
}

//------------------------------------------------------------------------
} // vst3utils
