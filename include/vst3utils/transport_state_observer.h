//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#pragma once

#include <algorithm>

//------------------------------------------------------------------------
namespace vst3utils {

//------------------------------------------------------------------------
/** a transport state observer

observes the playing, recording and cycle-active state of the host transport

usage:

struct MyProcessor
{
	transport_state_observer transport_observer {};

	void doProcessing (Steinberg::Vst::ProcessData& data)
	{
		transport_observer.update (data, [&] (auto new_flags, auto old_flags, auto time_jump) {
			// recalculate project time dependent data, like LFO phase or similar
		});
	}
};

 */
struct transport_state_observer
{
	static constexpr uint32_t playing = 1 << 1;
	static constexpr uint32_t cycle_active = 1 << 2;
	static constexpr uint32_t recording = 1 << 3;

	template<typename Process_Data, typename Proc>
	constexpr void update (const Process_Data& data, Proc change_callback) noexcept
	{
		const auto& context = data.processContext;
		if (!context)
			return;
		uint32_t new_flags = context->state & (playing | cycle_active | recording);
		auto is_playing = new_flags & playing;
		auto num_samples = is_playing ? data.numSamples : 0u;
		auto project_time_jump = std::abs (context->projectTimeSamples - project_time) >
								 (is_playing ? max_time_drift : 0u);
		if (new_flags != flags || project_time_jump)
		{
			change_callback (new_flags, flags, project_time_jump);
			flags = new_flags;
		}
		project_time = context->projectTimeSamples + num_samples;
	}

	/** set the amount of samples the project time is allowed to drift while in playback.

		some hosts report a few samples off while in playback (maybe caused by rounding),
		so this sets the amount the project time is allowed to be off the calculated value.
	*/
	constexpr void set_max_time_drift_allowed (uint32_t samples) noexcept
	{
		max_time_drift = samples;
	}

	/** resets all internal values to their defaults */
	constexpr void reset () noexcept
	{
		flags = {};
		project_time = {};
	}

private:
	uint32_t max_time_drift {2};
	uint32_t flags {};
	int64_t project_time {};
};

//------------------------------------------------------------------------
} // vst3utils
