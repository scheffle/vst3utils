//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

//------------------------------------------------------------------------
namespace vst3utils {

//------------------------------------------------------------------------
/** a throttling realtime parameter updater

This class manages sending throttled parameter updates from the realtime process to the controller.

Example:

Create a throttled_parameter_updater variable in your AudioEffect

```
throttled_parameter_updater peakParamUpdate {ParameterID::Peak};
```

Initialize it with the sample rate and the update rate

```
peakParamUpdate.init (processSetup.sampleRate, 60.};
```

Call it in your realtime process function

a) either with the value to set :

```
peakParamUpdate.process (peakValue, data);
```

b) or by calling a function which calculates the new value :

```
peakParamUpdate.process (peakValue, data,
						 [] (auto lastValue, auto currentValue, auto updateInterval) {
							 if (lastValue <= currentValue)
								 return currentValue;
							 lastValue -= 0.01;
							 if (lastValue < currentValue)
								 return currentValue;
							 return lastValue;
						 });

```
the updater will only send the value when the time reached the next update time and the value is
different to the last sent value.

 */
struct throttled_parameter_updater
{
	using ParamID = Steinberg::Vst::ParamID;
	using ParamValue = Steinberg::Vst::ParamValue;
	using SampleRate = Steinberg::Vst::SampleRate;
	using TSamples = Steinberg::Vst::TSamples;
	using ProcessData = Steinberg::Vst::ProcessData;
	using IParameterChanges = Steinberg::Vst::IParameterChanges;

	throttled_parameter_updater () noexcept = default;
	throttled_parameter_updater (ParamID parameter_id) : param_id (parameter_id) {}

	void set_parameter_id (ParamID pID) noexcept { param_id = pID; }

	void init (SampleRate sampleRate, double hertz = 60.) noexcept
	{
		update_interval = static_cast<TSamples> (sampleRate / hertz);
		update_countdown = 0;
	}

	template<typename Proc>
	inline void process (ParamValue current_value, int32_t num_samples,
						 IParameterChanges* output_parameter_changes, Proc func,
						 int32_t current_sample_offset = 0) noexcept
	{
		assert (update_interval > 0 && "update interval not set");
		if (reached (num_samples))
		{
			check_and_send_parameter_update (func (last_value, current_value, update_interval),
											 output_parameter_changes, current_sample_offset);
		}
	}

	template<typename Proc>
	inline void process (ParamValue current_value, ProcessData& data, Proc func) noexcept
	{
		process (current_value, data.numSamples, data.outputParameterChanges, func);
	}

	inline void process (ParamValue current_value, ProcessData& data) noexcept
	{
		process (current_value, data, [] (auto, auto currentValue, auto) { return currentValue; });
	}

private:
	inline bool reached (int32_t samples) noexcept
	{
		update_countdown -= samples;
		if (update_countdown <= 0)
		{
			while (update_countdown <= 0)
				update_countdown += update_interval;
			return true;
		}
		return false;
	}

	inline void check_and_send_parameter_update (ParamValue new_value,
												 IParameterChanges* output_parameter_changes,
												 int32_t sample_offset = 0) noexcept
	{
		if (last_value == new_value || output_parameter_changes == nullptr)
			return;
		Steinberg::int32 index;
		if (auto queue = output_parameter_changes->addParameterData (param_id, index))
			queue->addPoint (sample_offset, new_value, index);
		last_value = new_value;
	}

	ParamID param_id {0};
	ParamValue last_value {0};
	TSamples update_countdown {0};
	TSamples update_interval {0};
};

//------------------------------------------------------------------------
} // vst3utils
