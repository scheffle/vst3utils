//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/vst/ivstparameterchanges.h"

//------------------------------------------------------------------------
namespace vst3utils {

//------------------------------------------------------------------------
/** parameter changes iterator */
struct parameter_changes_iterator
{
	using IParameterChanges = Steinberg::Vst::IParameterChanges;
	using IParamValueQueue = Steinberg::Vst::IParamValueQueue;
	using int32 = Steinberg::int32;

	SMTG_ALWAYS_INLINE static parameter_changes_iterator begin (IParameterChanges* changes)
	{
		parameter_changes_iterator it;
		it.changes = changes;
		it.index = 0;
		it.num_changes = changes->getParameterCount ();
		it.update ();
		return it;
	}

	SMTG_ALWAYS_INLINE static parameter_changes_iterator end (IParameterChanges* changes)
	{
		parameter_changes_iterator it;
		it.changes = changes;
		it.queue = nullptr;
		it.num_changes = it.index = -1;
		return it;
	}

	SMTG_ALWAYS_INLINE bool operator== (const parameter_changes_iterator& it) const
	{
		return index == it.index;
	}

	SMTG_ALWAYS_INLINE bool operator!= (const parameter_changes_iterator& it) const
	{
		return index != it.index;
	}

	SMTG_ALWAYS_INLINE parameter_changes_iterator& operator++ ()
	{
		if (index >= 0)
			++index;
		update ();
		return *this;
	}
	SMTG_ALWAYS_INLINE parameter_changes_iterator operator+= (size_t adv)
	{
		assert (index >= 0);
		auto prev = *this;
		index += static_cast<int32> (adv);
		update ();
		return prev;
	}

	SMTG_ALWAYS_INLINE IParamValueQueue* operator* () { return queue; }
	SMTG_ALWAYS_INLINE IParamValueQueue* operator->() { return queue; }

private:
	SMTG_ALWAYS_INLINE void update ()
	{
		if (num_changes <= index || (queue = changes->getParameterData (index)) == nullptr)
			index = -1;
	}
	IParameterChanges* changes;
	IParamValueQueue* queue;
	int32 index;
	int32 num_changes;
};

//------------------------------------------------------------------------
/** parameter value queue iterator */
struct parameter_value_queue_iterator
{
	using IParamValueQueue = Steinberg::Vst::IParamValueQueue;
	using ParamValue = Steinberg::Vst::ParamValue;
	using ParamID = Steinberg::Vst::ParamID;
	using int32 = Steinberg::int32;

	struct point
	{
		ParamValue value {};
		ParamID pid {};
		int32 sample_offset {};
	};

	SMTG_ALWAYS_INLINE static parameter_value_queue_iterator begin (IParamValueQueue* queue)
	{
		parameter_value_queue_iterator it;
		it.queue = queue;
		it.index = 0;
		it.p.pid = queue->getParameterId ();
		it.num_points = queue->getPointCount ();
		it.update ();
		return it;
	}

	SMTG_ALWAYS_INLINE static parameter_value_queue_iterator end (IParamValueQueue* queue)
	{
		parameter_value_queue_iterator it;
		it.queue = queue;
		it.index = -1;
		return it;
	}

	SMTG_ALWAYS_INLINE bool operator== (const parameter_value_queue_iterator& it) const
	{
		return index == it.index;
	}

	SMTG_ALWAYS_INLINE bool operator!= (const parameter_value_queue_iterator& it) const
	{
		return index != it.index;
	}

	SMTG_ALWAYS_INLINE parameter_value_queue_iterator& operator++ ()
	{
		if (index >= 0)
			++index;
		update ();
		return *this;
	}
	SMTG_ALWAYS_INLINE parameter_value_queue_iterator operator+= (size_t adv)
	{
		assert (index >= 0);
		auto prev = *this;
		index += static_cast<int32> (adv);
		update ();
		return prev;
	}

	SMTG_ALWAYS_INLINE point& operator* () { return p; }
	SMTG_ALWAYS_INLINE point& operator->() { return p; }

private:
	SMTG_ALWAYS_INLINE void update ()
	{
		if (index >= num_points ||
			queue->getPoint (index, p.sample_offset, p.value) != Steinberg::kResultTrue)
			index = -1;
	}

	IParamValueQueue* queue;
	int32 index;
	int32 num_points;
	point p {};
};

//------------------------------------------------------------------------
SMTG_ALWAYS_INLINE parameter_changes_iterator begin (Steinberg::Vst::IParameterChanges* changes)
{
	assert (changes != nullptr);
	return parameter_changes_iterator::begin (changes);
}

//------------------------------------------------------------------------
SMTG_ALWAYS_INLINE parameter_changes_iterator end (Steinberg::Vst::IParameterChanges* changes)
{
	assert (changes != nullptr);
	return parameter_changes_iterator::end (changes);
}

//------------------------------------------------------------------------
SMTG_ALWAYS_INLINE parameter_value_queue_iterator begin (Steinberg::Vst::IParamValueQueue* queue)
{
	assert (queue != nullptr);
	return parameter_value_queue_iterator::begin (queue);
}

//------------------------------------------------------------------------
SMTG_ALWAYS_INLINE parameter_value_queue_iterator end (Steinberg::Vst::IParamValueQueue* queue)
{
	assert (queue != nullptr);
	return parameter_value_queue_iterator::end (queue);
}

//------------------------------------------------------------------------
} // vst3utils

//------------------------------------------------------------------------
namespace Steinberg::Vst {

using vst3utils::begin;
using vst3utils::end;

//------------------------------------------------------------------------
}
