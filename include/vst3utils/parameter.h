//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#pragma once

#include "vst3utils/parameter_description.h"
#include "vst3utils/string_conversion.h"
#include "public.sdk/source/vst/vstparameters.h"
#include <charconv>
#include <functional>
#include <string>
#include <vector>

#if !defined(__cpp_lib_to_chars) || !defined(__cpp_lib_to_chars)
#include "pluginterfaces/base/ustring.h"
#endif

//------------------------------------------------------------------------
namespace vst3utils {

//------------------------------------------------------------------------
/** a vst3 sdk compatible parameter class using a param::description structure to initialize

Example:

	enum class ParameterID {
		Bypass,
		Gain,
		Pan,

		Count,
	};

	static std::array<param::description, static_cast<size_t> (ParameterID::Count)>
	parameterDescriptions = {{
		{listDescription (u"bypass", 0, strings_on_off)},
		{rangeDescription (u"gain", -12, linear_functions<-60, 0> (), 1, u"dB")},
		{u"pan", 1., range {-100., 100., 0}, nullptr},
	}};

	struct MyEditController : public Vst::EditController
	{
		tresult initialize (FUnknown* context) override
		{
			auto result = Vst::EditController::initialize (context);
			if (result != kResultTrue)
				return result;
			for (auto pid = 0; pid < static_cast<ParamID> (ParameterID::Count); ++pid)
			{
				auto param = new Parameter (pid, parameterDescriptions[pid]);
				parameters.addParameter (param);
			}
		}
	};

 */
class parameter : public Steinberg::Vst::Parameter
{
public:
	using Flags = Steinberg::Vst::ParameterInfo::ParameterFlags;
	using param_id = Steinberg::Vst::ParamID;
	using param_value = Steinberg::Vst::ParamValue;
	using string_128 = Steinberg::Vst::String128;
	using tchar = Steinberg::Vst::TChar;

	inline parameter (param_id pid, const param::description& desc,
					  int32_t flags = Flags::kCanAutomate);

	inline param_value getPlain () const { return toPlain (getNormalized ()); }

	//-- overrides
	inline bool setNormalized (param_value v) override;
	inline void toString (param_value valueNormalized, string_128 string) const override;
	inline bool fromString (const tchar* string, param_value& valueNormalized) const override;
	inline param_value toPlain (param_value valueNormalized) const override;
	inline param_value toNormalized (param_value plainValue) const override;

	//--- listener
	using value_changed_func = std::function<void (parameter&, param_value)>;
	using token = uint64_t;
	static constexpr const token invalid_token = 0u;

	inline token add_listener (const value_changed_func& func);
	inline void remove_listener (token token);

	//-- custom conversion
	using to_plain_func = std::function<param_value (const parameter& param, param_value norm)>;
	using to_normalized_func =
		std::function<param_value (const parameter& param, param_value plain)>;
	using to_string_func =
		std::function<void (const parameter& param, param_value norm, string_128 out_string)>;
	using from_string_func =
		std::function<bool (const parameter& param, const tchar* string, param_value& out_norm)>;

	inline void set_custom_to_plain_func (const to_plain_func& func);
	inline void set_custom_to_normalized_func (const to_normalized_func& func);
	inline void set_custom_to_string_func (const to_string_func& func);
	inline void set_custom_from_string_func (const from_string_func& func);

	const param::description& description () const { return desc; }

private:
	static tchar* tstrncpy (tchar* dest, const tchar* source, size_t count)
	{
		return Steinberg::tstrncpy (dest, source, static_cast<Steinberg::uint32> (count));
	}
	const tchar* str_cast (const char16_t* s) const { return reinterpret_cast<const tchar*> (s); }

	const param::description& desc;
	std::vector<std::pair<value_changed_func, token>> listeners;
	token tokenCounter {0};

	to_plain_func to_plain {};
	to_normalized_func to_normalized {};
	to_string_func to_string {};
	from_string_func from_string {};
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
inline parameter::parameter (param_id pid, const param::description& desc, int32_t flags)
: desc (desc)
{
	using namespace Steinberg;

	info.id = pid;
	info.flags = flags;
	info.defaultNormalizedValue = desc.default_normalized;
	tstrcpy (info.title, str_cast (desc.name));
	if (auto stepCount = std::get_if<param::step_count> (&desc.range_or_step_count))
	{
		info.stepCount = stepCount->num_steps;
		setPrecision (0);
		if (stepCount->unit)
			tstrncpy (info.units, stepCount->unit, std::size (info.units));
		if (stepCount->string_list)
			info.flags |= Flags::kIsList;
	}
	else if (auto range = std::get_if<param::range> (&desc.range_or_step_count))
	{
		setPrecision (range->precision);
		if (range->unit)
			tstrncpy (info.units, range->unit, std::size (info.units));
	}
	if (desc.convert.to_plain)
	{
		set_custom_to_plain_func (
			[] (const auto& p, auto v) -> param_value { return p.desc.convert.to_plain (v); });
	}
	if (desc.convert.to_normalized)
	{
		set_custom_to_normalized_func (
			[] (const auto& p, auto v) -> param_value { return p.desc.convert.to_normalized (v); });
	}
	setNormalized (info.defaultNormalizedValue);
}

//------------------------------------------------------------------------
inline void parameter::set_custom_to_plain_func (const to_plain_func& func) { to_plain = func; }

//------------------------------------------------------------------------
inline void parameter::set_custom_to_normalized_func (const to_normalized_func& func)
{
	to_normalized = func;
}

//------------------------------------------------------------------------
inline void parameter::set_custom_to_string_func (const to_string_func& func) { to_string = func; }

//------------------------------------------------------------------------
inline void parameter::set_custom_from_string_func (const from_string_func& func)
{
	from_string = func;
}

//------------------------------------------------------------------------
inline auto parameter::add_listener (const value_changed_func& func) -> token
{
	auto token = ++tokenCounter;
	listeners.emplace_back (func, token);
	return token;
}

//------------------------------------------------------------------------
inline void parameter::remove_listener (token t)
{
	auto it = std::find_if (listeners.begin (), listeners.end (),
							[&] (const auto& p) { return p.second == t; });
	if (it != listeners.end ())
		listeners.erase (it);
}

//------------------------------------------------------------------------
inline bool parameter::setNormalized (param_value v)
{
	auto res = Steinberg::Vst::Parameter::setNormalized (v);
	if (res)
	{
		std::for_each (listeners.begin (), listeners.end (),
					   [this] (const auto& p) { p.first (*this, getNormalized ()); });
	}
	return res;
}

//------------------------------------------------------------------------
inline void parameter::toString (param_value value_normalized, string_128 string) const
{
	using namespace Steinberg;

	if (to_string)
	{
		to_string (*this, value_normalized, string);
		return;
	}
	auto plain = toPlain (value_normalized);
	auto stepCount = std::get_if<param::step_count> (&desc.range_or_step_count);
	if (stepCount && stepCount->string_list)
	{
		tstrncpy (
			string,
			str_cast (stepCount->string_list[static_cast<size_t> (plain - stepCount->start_value)]),
			sizeof (string_128) / sizeof (tchar));
	}
	else
	{
#if defined(__cpp_lib_to_chars) && (__cpp_lib_to_chars >= 201611L)
		std::string ascii;
		ascii.resize (128);
		auto res = std::to_chars (ascii.data (), ascii.data () + ascii.size (), plain,
								  std::chars_format::fixed, precision);
		if (res.ec == std::errc ())
		{
			ascii.resize (res.ptr - ascii.data ());
			copy_ascii_to_utf16 (ascii, string, string + sizeof (string_128) / sizeof (tchar));
		}
#else
		UString wrapper (string, str16BufferSize (string_128));
		if (!wrapper.printFloat (plain, precision))
			string[0] = 0;
#endif
	}
}

//------------------------------------------------------------------------
inline bool parameter::fromString (const tchar* string, param_value& value_normalized) const
{
	using namespace Steinberg;

	if (from_string)
		return from_string (*this, string, value_normalized);
	auto stepCount = std::get_if<param::step_count> (&desc.range_or_step_count);
	if (stepCount && stepCount->string_list)
	{
		for (auto index = 0u; index <= stepCount->num_steps; ++index)
		{
			if (tstrcmp (string, str_cast (stepCount->string_list[index])) == 0)
			{
				value_normalized = toNormalized (index);
				return true;
			}
		}
		return false;
	}
#if defined(__cpp_lib_to_chars) && (__cpp_lib_to_chars >= 201611L)
	double value = 0.;
	auto ascii = copy_utf16_to_ascii (string);
	auto res = std::from_chars (ascii.data (), ascii.data () + ascii.size (), value,
								std::chars_format::fixed);
	if (res.ec == std::errc ())
	{
		valueNormalized = toNormalized (value);
		return true;
	}
#else
	UString wrapper (const_cast<tchar*> (string), tstrlen (string));
	if (wrapper.scanFloat (value_normalized))
	{
		value_normalized = toNormalized (value_normalized);
		return true;
	}
#endif
	return false;
}

//------------------------------------------------------------------------
inline auto parameter::toPlain (param_value valueNormalized) const -> param_value
{
	if (to_plain)
		return to_plain (*this, valueNormalized);
	if (auto stepCount = std::get_if<param::step_count> (&desc.range_or_step_count))
	{
		return normalized_to_steps (stepCount->num_steps, stepCount->start_value, valueNormalized);
	}
	auto range = std::get<param::range> (desc.range_or_step_count);
	return normalized_to_plain (range.min, range.max, valueNormalized);
}

//------------------------------------------------------------------------
inline auto parameter::toNormalized (param_value plainValue) const -> param_value
{
	if (to_normalized)
		return to_normalized (*this, plainValue);
	if (auto stepCount = std::get_if<param::step_count> (&desc.range_or_step_count))
	{
		return steps_to_normalized (stepCount->num_steps, stepCount->start_value, plainValue);
	}
	auto range = std::get<param::range> (desc.range_or_step_count);
	return plain_to_normalized (range.min, range.max, plainValue);
}

//------------------------------------------------------------------------
} // vst3utils
