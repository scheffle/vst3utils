//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/vst/ivstmessage.h"
#include <limits>
#include <optional>
#include <string>
#include <string_view>

//------------------------------------------------------------------------
namespace vst3utils {
struct attribute_list;

//------------------------------------------------------------------------
/** message adapter

an adapter to safely use a Steinberg::Vst::IMessage object

 */
struct message
{
	using IMessage = Steinberg::Vst::IMessage;

	inline message (Steinberg::IPtr<IMessage> msg);

	/** check if the message is valid */
	inline bool is_valid () const;

	/** set the message ID */
	inline void set_id (const char* mId);
	/** get the message ID */
	inline std::string_view get_id () const;
	/** get the messages attributes*/
	inline attribute_list get_attributes () const;

	/** access the underlying IMessage pointer (no ref counting) */
	inline operator IMessage* () const;

private:
	Steinberg::IPtr<IMessage> msg;
};

//------------------------------------------------------------------------
/** attribute list adapter

an adapter for Steinberg::Vst::IAttributeList

supports setting and getting arbitrary data

Example of getting a double value:

	attribute_list list = message.get_attributes ();
	if (auto value = list.get<double> ("MyValue"))
	{
	}

Example of setting/getting a custom struct value:

	struct my_data {
		int major;
		int minor;
		int sub;
		double value;
	};

	my_data data = {1,2,3,400.};
	attribute_list list = message.get_attributes ();
	list.set ("MyData", data);

	if (auto value = list.get<my_data> ("MyData"))
	{
		assert (value->major == data.major);
		// etc
	}

 */
struct attribute_list
{
	using IAttributeList = Steinberg::Vst::IAttributeList;
	using attribute_id = const char*;

	inline attribute_list (Steinberg::IPtr<IAttributeList> list);

	/** check if the attribute list is valid */
	inline bool is_valid () const;

	/** set an integer value */
	template<typename value_t, typename std::enable_if_t<std::is_integral_v<value_t>>* = nullptr>
	inline void set (attribute_id aId, value_t value);

	/** set a floating-point value */
	template<typename value_t,
			 typename std::enable_if_t<std::is_floating_point_v<value_t>>* = nullptr>
	inline void set (attribute_id, value_t value);

	/** set a standard and trivial value */
	template<typename value_t,
			 typename std::enable_if_t<std::is_standard_layout_v<value_t> &&
									   std::is_trivial_v<value_t> && !std::is_integral_v<value_t> &&
									   !std::is_floating_point_v<value_t>>* = nullptr>
	inline void set (attribute_id aId, const value_t& value);

	/** get an integer value */
	template<typename value_t, typename std::enable_if_t<std::is_integral_v<value_t>>* = nullptr>
	inline std::optional<value_t> get (attribute_id aId) const;

	/** get a floating point value */
	template<typename value_t,
			 typename std::enable_if_t<std::is_floating_point_v<value_t>>* = nullptr>
	inline std::optional<value_t> get (attribute_id aId) const;

	/** get a standard and trivial value */
	template<typename value_t,
			 typename std::enable_if_t<std::is_standard_layout_v<value_t> &&
									   std::is_trivial_v<value_t> && !std::is_integral_v<value_t> &&
									   !std::is_floating_point_v<value_t>>* = nullptr>
	inline std::optional<value_t> get (attribute_id aId) const;

	/** set a string value */
	template<typename value_t,
			 typename std::enable_if_t<std::is_same_v<value_t, std::string_view> ||
									   std::is_same_v<value_t, std::string>>* = nullptr>
	inline void set (attribute_id aId, value_t str);

	/** get a string value */
	template<typename value_t,
			 typename std::enable_if_t<std::is_same_v<value_t, std::string> ||
									   std::is_same_v<value_t, std::string_view>>* = nullptr>
	inline std::optional<value_t> get (attribute_id aId) const;

	/** set binary data */
	inline void set (attribute_id aId, const void* data, size_t data_size);

	template<typename value_t>
	struct span
	{
		const value_t* data;
		size_t size;
	};

	/** get a span value
	 *
	 *  returns the data as a span without copying it (data only valid as long as the attribute list
	 * is not changed or destroyed)
	 */
	template<typename value_t, size_t num_elements,
			 typename std::enable_if_t<std::is_standard_layout_v<value_t> &&
									   std::is_trivial_v<value_t> && !std::is_integral_v<value_t> &&
									   !std::is_floating_point_v<value_t>>* = nullptr>
	inline std::optional<span<value_t>> get (attribute_id aId) const;

private:
	Steinberg::IPtr<IAttributeList> list;
};

//------------------------------------------------------------------------
inline message::message (Steinberg::IPtr<IMessage> inMsg) : msg (std::move (inMsg)) {}

//------------------------------------------------------------------------
inline bool message::is_valid () const { return msg != nullptr; }

//------------------------------------------------------------------------
inline void message::set_id (const char* mId)
{
	if (msg)
		msg->setMessageID (mId);
}

//------------------------------------------------------------------------
inline std::string_view message::get_id () const
{
	if (msg)
		return msg->getMessageID ();
	return {};
}

//------------------------------------------------------------------------
inline attribute_list message::get_attributes () const
{
	return {msg ? msg->getAttributes () : nullptr};
}

//------------------------------------------------------------------------
inline message::operator IMessage* () const { return msg; }

//------------------------------------------------------------------------
inline attribute_list::attribute_list (Steinberg::IPtr<IAttributeList> list)
: list (std::move (list))
{
}

//------------------------------------------------------------------------
inline bool attribute_list::is_valid () const { return list != nullptr; }

//------------------------------------------------------------------------
template<typename value_t, typename std::enable_if_t<std::is_integral_v<value_t>>*>
inline void attribute_list::set (attribute_id aId, value_t value)
{
	static_assert (sizeof (value_t) <= sizeof (Steinberg::int64));
	if (list)
		list->setInt (aId, static_cast<Steinberg::int64> (value));
}

//------------------------------------------------------------------------
template<typename value_t, typename std::enable_if_t<std::is_floating_point_v<value_t>>*>
inline void attribute_list::set (attribute_id aId, value_t value)
{
	static_assert (sizeof (value_t) <= sizeof (double));
	if (list)
		list->setFloat (aId, static_cast<double> (value));
}

//------------------------------------------------------------------------
template<typename value_t, typename std::enable_if_t<
							   std::is_standard_layout_v<value_t> && std::is_trivial_v<value_t> &&
							   !std::is_integral_v<value_t> && !std::is_floating_point_v<value_t>>*>
inline void attribute_list::set (attribute_id aId, const value_t& value)
{
	if (list)
		list->setBinary (aId, &value, sizeof (value_t));
}

//------------------------------------------------------------------------
template<typename value_t, typename std::enable_if_t<std::is_integral_v<value_t>>*>
inline std::optional<value_t> attribute_list::get (attribute_id aId) const
{
	static_assert (sizeof (value_t) <= sizeof (Steinberg::int64));
	if (list)
	{
		Steinberg::int64 v {};
		if (list->getInt (aId, v) == Steinberg::kResultTrue)
		{
			if (v > std::numeric_limits<value_t>::max ())
				return {};
			if (v < std::numeric_limits<value_t>::min ())
				return {};
			return std::make_optional (static_cast<value_t> (v));
		}
	}
	return {};
}

//------------------------------------------------------------------------
template<typename value_t, typename std::enable_if_t<std::is_floating_point_v<value_t>>*>
inline std::optional<value_t> attribute_list::get (attribute_id aId) const
{
	static_assert (sizeof (value_t) <= sizeof (double));
	if (list)
	{
		double v {};
		if (list->getFloat (aId, v) == Steinberg::kResultTrue)
		{
			if (v > std::numeric_limits<value_t>::max ())
				return {};
			if (v < std::numeric_limits<value_t>::min ())
				return {};
			return std::make_optional (static_cast<value_t> (v));
		}
	}
	return {};
}

//------------------------------------------------------------------------
template<typename value_t, typename std::enable_if_t<
							   std::is_standard_layout_v<value_t> && std::is_trivial_v<value_t> &&
							   !std::is_integral_v<value_t> && !std::is_floating_point_v<value_t>>*>
inline std::optional<value_t> attribute_list::get (attribute_id aId) const
{
	if (list)
	{
		const void* data {};
		Steinberg::uint32 data_size {};
		if (list->getBinary (aId, data, data_size) == Steinberg::kResultTrue)
		{
			if (data_size == sizeof (value_t))
			{
				value_t v;
				memcpy (&v, data, data_size);
				return std::make_optional (std::move (v));
			}
		}
	}
	return {};
}

//------------------------------------------------------------------------
template<typename value_t, typename std::enable_if_t<std::is_same_v<value_t, std::string_view> ||
													 std::is_same_v<value_t, std::string>>*>
inline void attribute_list::set (attribute_id aId, value_t str)
{
	if (list)
	{
		list->setBinary (aId, str.data (), static_cast<uint32_t> (str.size ()));
	}
}

//------------------------------------------------------------------------
template<typename value_t, typename std::enable_if_t<std::is_same_v<value_t, std::string> ||
													 std::is_same_v<value_t, std::string_view>>*>
inline std::optional<value_t> attribute_list::get (attribute_id aId) const
{
	if (list)
	{
		const void* data {};
		Steinberg::uint32 data_size {};
		if (list->getBinary (aId, data, data_size) == Steinberg::kResultTrue)
		{
			return std::make_optional (
				std::string (reinterpret_cast<const char*> (data), data_size));
		}
	}
	return {};
}

//------------------------------------------------------------------------
inline void attribute_list::set (attribute_id aId, const void* data, size_t data_size)
{
	if (list)
	{
		list->setBinary (aId, data, static_cast<Steinberg::uint32> (data_size));
	}
}

//------------------------------------------------------------------------
template<
	typename value_t, size_t num_elements,
	typename std::enable_if_t<std::is_standard_layout_v<value_t> && std::is_trivial_v<value_t> &&
							  !std::is_integral_v<value_t> && !std::is_floating_point_v<value_t>>*>
inline std::optional<attribute_list::span<value_t>> attribute_list::get (attribute_id aId) const
{
	if (list)
	{
		const void* data {};
		Steinberg::uint32 data_size {};
		if (list->getBinary (aId, data, data_size) == Steinberg::kResultTrue)
		{
			if ((data_size % sizeof (value_t)) == 0)
			{
				span<value_t> s;
				s.data = reinterpret_cast<const value_t*> (data);
				s.size = static_cast<size_t> (data_size / sizeof (value_t));
				if (s.size == num_elements)
					return std::make_optional (s);
			}
		}
	}
	return {};
}

//------------------------------------------------------------------------
} // vst3utils
