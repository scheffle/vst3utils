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
#include <array>
#include <random>

//------------------------------------------------------------------------
namespace vst3utils {

//------------------------------------------------------------------------
TEST (attribute_list_test, valid_test)
{
	attribute_list list (Steinberg::Vst::HostAttributeList::make ());
	EXPECT_TRUE (list.is_valid ());

	attribute_list list2 (nullptr);
	EXPECT_FALSE (list2.is_valid ());
}

//------------------------------------------------------------------------
TEST (attribute_list_test, int8_t)
{
	attribute_list list (Steinberg::Vst::HostAttributeList::make ());

	int8_t i8 = -32;
	list.set ("int8_t", i8);
	auto res_i8 = list.get<int8_t> ("int8_t");
	ASSERT_TRUE (res_i8);
	EXPECT_EQ (*res_i8, i8);
	auto res_ui8 = list.get<uint8_t> ("int8_t");
	ASSERT_FALSE (res_ui8);
}

//------------------------------------------------------------------------
TEST (attribute_list_test, uint8_t)
{
	attribute_list list (Steinberg::Vst::HostAttributeList::make ());

	uint8_t ui8 = static_cast<uint8_t> (std::numeric_limits<int8_t>::max ()) + 1;
	list.set ("uint8_t", ui8);
	auto res_ui8 = list.get<uint8_t> ("uint8_t");
	ASSERT_TRUE (res_ui8);
	EXPECT_EQ (*res_ui8, ui8);
	auto res_i8 = list.get<int8_t> ("uint8_t");
	ASSERT_FALSE (res_i8);
}

//------------------------------------------------------------------------
TEST (attribute_list_test, int16_t)
{
	attribute_list list (Steinberg::Vst::HostAttributeList::make ());

	int16_t i16 = -32;
	list.set ("int16_t", i16);
	auto res_i16 = list.get<int16_t> ("int16_t");
	ASSERT_TRUE (res_i16);
	EXPECT_EQ (*res_i16, i16);
	auto res_ui16 = list.get<uint16_t> ("int16_t");
	ASSERT_FALSE (res_ui16);
}

//------------------------------------------------------------------------
TEST (attribute_list_test, uint16_t)
{
	attribute_list list (Steinberg::Vst::HostAttributeList::make ());

	uint16_t ui16 = static_cast<uint16_t> (std::numeric_limits<int16_t>::max ()) + 1;
	list.set ("uint16_t", ui16);
	auto res_ui16 = list.get<uint16_t> ("uint16_t");
	ASSERT_TRUE (res_ui16);
	EXPECT_EQ (*res_ui16, ui16);
	auto res_i16 = list.get<int16_t> ("uint16_t");
	ASSERT_FALSE (res_i16);
}

//------------------------------------------------------------------------
TEST (attribute_list_test, int32_t)
{
	attribute_list list (Steinberg::Vst::HostAttributeList::make ());

	int32_t i32 = -32;
	list.set ("int32_t", i32);
	auto res_i32 = list.get<int32_t> ("int32_t");
	ASSERT_TRUE (res_i32);
	EXPECT_EQ (*res_i32, i32);
	auto res_ui32 = list.get<uint32_t> ("int32_t");
	ASSERT_FALSE (res_ui32);
}

//------------------------------------------------------------------------
TEST (attribute_list_test, uint32_t)
{
	attribute_list list (Steinberg::Vst::HostAttributeList::make ());

	uint32_t ui32 = static_cast<uint32_t> (std::numeric_limits<int32_t>::max ()) + 1;
	list.set ("uint32_t", ui32);
	auto res_ui32 = list.get<uint32_t> ("uint32_t");
	ASSERT_TRUE (res_ui32);
	EXPECT_EQ (*res_ui32, ui32);
	auto res_i32 = list.get<int32_t> ("uint32_t");
	ASSERT_FALSE (res_i32);
}

//------------------------------------------------------------------------
TEST (attribute_list_test, int64_t)
{
	attribute_list list (Steinberg::Vst::HostAttributeList::make ());

	int64_t i64 = -32;
	list.set ("int64_t", i64);
	auto res_i64 = list.get<int64_t> ("int64_t");
	ASSERT_TRUE (res_i64);
	EXPECT_EQ (*res_i64, i64);
	auto res_ui64 = list.get<uint64_t> ("int64_t");
	ASSERT_TRUE (res_ui64);
	EXPECT_EQ (*res_ui64, static_cast<uint64_t> (i64));
}

//------------------------------------------------------------------------
TEST (attribute_list_test, uint64_t)
{
	attribute_list list (Steinberg::Vst::HostAttributeList::make ());

	uint64_t ui64 = static_cast<uint64_t> (std::numeric_limits<int64_t>::max ()) + 1;
	list.set ("uint64_t", ui64);
	auto res_ui64 = list.get<uint64_t> ("uint64_t");
	ASSERT_TRUE (res_ui64);
	EXPECT_EQ (*res_ui64, ui64);
	auto res_i64 = list.get<int64_t> ("uint64_t");
	ASSERT_TRUE (res_i64);
	EXPECT_EQ (*res_i64, static_cast<int64_t> (ui64));
}

//------------------------------------------------------------------------
TEST (attribute_list_test, bool)
{
	attribute_list list (Steinberg::Vst::HostAttributeList::make ());

	bool b = true;
	list.set ("bool", b);
	auto res_b = list.get<bool> ("bool");
	ASSERT_TRUE (res_b);
	EXPECT_TRUE (*res_b);

	list.set ("bool", 2);
	res_b = list.get<bool> ("bool");
	ASSERT_FALSE (res_b);
}

//------------------------------------------------------------------------
TEST (attribute_list_test, float)
{
	attribute_list list (Steinberg::Vst::HostAttributeList::make ());

	float f = 0.46537;
	list.set ("float", f);
	auto res_f = list.get<float> ("float");
	EXPECT_TRUE (res_f);
	EXPECT_FLOAT_EQ (*res_f, f);

	auto res_d = list.get<double> ("float");
	EXPECT_TRUE (res_d);
	EXPECT_DOUBLE_EQ (*res_d, f);
}

//------------------------------------------------------------------------
TEST (attribute_list_test, double)
{
	attribute_list list (Steinberg::Vst::HostAttributeList::make ());

	double d = std::numeric_limits<double>::max ();
	list.set ("double", d);
	auto res_d = list.get<double> ("double");
	EXPECT_TRUE (res_d);
	EXPECT_FLOAT_EQ (*res_d, d);

	auto res_f = list.get<float> ("double");
	EXPECT_FALSE (res_f);
}

//------------------------------------------------------------------------
TEST (attribute_list_test, string)
{
	attribute_list list (Steinberg::Vst::HostAttributeList::make ());

	std::string str = "This is my string";
	list.set ("string", str);
	auto res_str = list.get<std::string> ("string");
	ASSERT_TRUE (res_str);
	auto res_str_view = list.get<std::string_view> ("string");
	ASSERT_TRUE (res_str_view);
	EXPECT_TRUE (*res_str == str);
	EXPECT_TRUE (*res_str_view == str);
}

//------------------------------------------------------------------------
TEST (attribute_list_test, string_view)
{
	attribute_list list (Steinberg::Vst::HostAttributeList::make ());

	std::string_view str = "This is my string";
	list.set ("string", str);
	auto res_str = list.get<std::string> ("string");
	ASSERT_TRUE (res_str);
	auto res_str_view = list.get<std::string_view> ("string");
	ASSERT_TRUE (res_str_view);
	EXPECT_TRUE (*res_str == str);
	EXPECT_TRUE (*res_str_view == str);
}

//------------------------------------------------------------------------
TEST (attribute_list_test, u16string)
{
	attribute_list list (Steinberg::Vst::HostAttributeList::make ());

	std::u16string str = u"This is a utf-16 string";
	list.set ("u16string", str);
	auto res_s = list.get<std::u16string> ("u16string", str.size ());
	ASSERT_TRUE (res_s);
	EXPECT_TRUE (str == *res_s);
}

//------------------------------------------------------------------------
TEST (attribute_list_test, struct)
{
	attribute_list list (Steinberg::Vst::HostAttributeList::make ());

	struct my_struct
	{
		int32_t one;
		int8_t two;
		float three;
		double four;
	};

	my_struct value {22000, 8, 0.2f, -199.9876};
	list.set ("my_struct", value);
	auto res_s = list.get<my_struct> ("my_struct");
	ASSERT_TRUE (res_s);
	EXPECT_EQ (res_s->one, value.one);
	EXPECT_EQ (res_s->two, value.two);
	EXPECT_FLOAT_EQ (res_s->three, value.three);
	EXPECT_DOUBLE_EQ (res_s->four, value.four);
}

//------------------------------------------------------------------------
template<typename distribution_t = std::uniform_int_distribution<>,
		 typename algo_t = std::default_random_engine, typename device_t = std::random_device,
		 typename value_t>
void randomizeMemory (const value_t& begin, const value_t& end)
{
	device_t rd;
	algo_t gen (rd ());
	distribution_t distrib;
	std::for_each (begin, end, [&] (auto& el) { el = distrib (gen); });
}

//------------------------------------------------------------------------
TEST (attribute_list_test, binary)
{
	using array = std::array<uint32_t, 1024>;

	attribute_list list (Steinberg::Vst::HostAttributeList::make ());

	array data;
	constexpr auto memorySize = data.size () * sizeof (array::value_type);

	randomizeMemory<> (data.begin (), data.end ());

	list.set ("binary", data.data (), memorySize);
	auto res_d = list.get<array::value_type, data.size ()> ("binary");
	ASSERT_TRUE (res_d);
	ASSERT_EQ (data.size (), res_d->size);
	for (auto index = 0u; index < data.size (); ++index)
		ASSERT_EQ (data[index], res_d->data[index]);
}

//------------------------------------------------------------------------
TEST (attribute_list_test, array)
{
	using array = std::array<uint32_t, 1024>;

	attribute_list list (Steinberg::Vst::HostAttributeList::make ());

	array data;
	randomizeMemory<> (data.begin (), data.end ());

	list.set_array ("array", data.data (), data.size ());
	auto res_d = list.get<array::value_type, data.size ()> ("array");
	ASSERT_TRUE (res_d);
	ASSERT_EQ (data.size (), res_d->size);
	for (auto index = 0u; index < data.size (); ++index)
		ASSERT_EQ (data[index], res_d->data[index]);
}

//------------------------------------------------------------------------
} // vst3utils
