//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/base/fplatform.h"
#include "pluginterfaces/base/ibstream.h"

//------------------------------------------------------------------------
namespace vst3utils {

//------------------------------------------------------------------------
enum class byte_order
{
	little_endian = kLittleEndian,
	big_endian = kBigEndian,
	native_endian = BYTEORDER
};

//------------------------------------------------------------------------
template<byte_order stream_byte_order, bool throw_on_error = false>
struct byte_order_ibstream
{
	using stream_ptr = Steinberg::IPtr<Steinberg::IBStream>;
	byte_order_ibstream (stream_ptr&& stream);

	struct Result
	{
		Steinberg::tresult return_code {};
		size_t bytes {};
		operator bool () const { return return_code == Steinberg::kResultTrue; }
	};

	template<typename T>
	Result operator<< (const T& input) const;
	template<typename T>
	Result operator>> (T& output);

	Result read (void* dest, size_t num_bytes);
	Result write (const void* src, size_t num_bytes) const;

private:
	template<size_t size>
	Result swapAndWrite (const uint8_t* buffer) const;
	void swap (uint8_t* buffer, uint64_t size) const;

	stream_ptr stream;
};

//------------------------------------------------------------------------
template<byte_order stream_byte_order, bool throw_on_error = false>
byte_order_ibstream<stream_byte_order, throw_on_error>
	make_byte_order_stream (Steinberg::IPtr<Steinberg::IBStream> stream)
{
	return byte_order_ibstream<stream_byte_order, throw_on_error> (std::move (stream));
}

//------------------------------------------------------------------------
struct io_error : std::runtime_error
{
	io_error (const char* what, Steinberg::tresult result, Steinberg::int32 bytes_read_or_written)
	: std::runtime_error (what), result (result), bytes_read_or_written (bytes_read_or_written)
	{
	}

	Steinberg::tresult result;
	Steinberg::int32 bytes_read_or_written;
};

//------------------------------------------------------------------------
template<byte_order stream_byte_order, bool throw_on_error>
inline byte_order_ibstream<stream_byte_order, throw_on_error>::byte_order_ibstream (
	stream_ptr&& stream)
: stream (std::move (stream))
{
}

//------------------------------------------------------------------------
template<byte_order stream_byte_order, bool throw_on_error>
auto byte_order_ibstream<stream_byte_order, throw_on_error>::read (void* dest, size_t num_bytes)
	-> Result
{
	if (num_bytes > std::numeric_limits<Steinberg::int32>::max ())
		return {Steinberg::kInvalidArgument, 0};
	Steinberg::int32 read_bytes {};
	auto result = stream->read (dest, static_cast<Steinberg::int32> (num_bytes), &read_bytes);
	if constexpr (throw_on_error)
	{
		if (result != Steinberg::kResultTrue)
			throw io_error ("read failure", result, read_bytes);
	}
	return {result, static_cast<size_t> (read_bytes)};
}

//------------------------------------------------------------------------
template<byte_order stream_byte_order, bool throw_on_error>
auto byte_order_ibstream<stream_byte_order, throw_on_error>::write (const void* src,
																	size_t num_bytes) const
	-> Result
{
	if (num_bytes > std::numeric_limits<Steinberg::int32>::max ())
		return {Steinberg::kInvalidArgument, 0};
	Steinberg::int32 written_bytes {};
	auto result = stream->write (const_cast<void*> (src), static_cast<Steinberg::int32> (num_bytes),
								 &written_bytes);
	if constexpr (throw_on_error)
	{
		if (result != Steinberg::kResultTrue)
			throw io_error ("write failure", result, written_bytes);
	}
	return {result, static_cast<size_t> (written_bytes)};
}

//------------------------------------------------------------------------
template<byte_order stream_byte_order, bool throw_on_error>
template<typename T>
inline auto
	byte_order_ibstream<stream_byte_order, throw_on_error>::operator<< (const T& input) const
	-> Result
{
	static_assert (std::is_standard_layout<T>::value, "Supports only standard layout types");
	if constexpr (stream_byte_order == byte_order::native_endian)
		return write (static_cast<const void*> (&input), sizeof (T));

	return swapAndWrite<sizeof (T)> (reinterpret_cast<const uint8_t*> (&input));
}

//------------------------------------------------------------------------
template<byte_order stream_byte_order, bool throw_on_error>
template<typename T>
inline auto byte_order_ibstream<stream_byte_order, throw_on_error>::operator>> (T& output) -> Result
{
	static_assert (std::is_standard_layout<T>::value, "Supports only standard layout types");
	auto res = read (&output, sizeof (T));
	if constexpr (stream_byte_order == byte_order::native_endian)
		return res;

	swap (reinterpret_cast<uint8_t*> (&output), res.bytes);
	return res;
}

//------------------------------------------------------------------------
template<byte_order stream_byte_order, bool throw_on_error>
template<size_t _size>
inline auto byte_order_ibstream<stream_byte_order, throw_on_error>::swapAndWrite (
	const uint8_t* buffer) const -> Result
{
	if constexpr (_size > 1)
	{
		int8_t tmp[_size];

		constexpr auto halfSize = _size / 2;
		auto size = _size;
		auto low = buffer;
		auto high = buffer + size - 1;

		while (size > halfSize)
		{
			tmp[size - 2] = buffer[(_size - size) + 1];
			tmp[(_size - size) + 1] = buffer[size - 2];
			tmp[_size - size] = *high;
			tmp[size - 1] = *low;
			low += 2;
			high -= 2;
			size -= 2;
		}
		return write (tmp, _size);
	}
	return write (buffer, 1);
}

//------------------------------------------------------------------------
template<byte_order stream_byte_order, bool throw_on_error>
inline void byte_order_ibstream<stream_byte_order, throw_on_error>::swap (uint8_t* buffer,
																		  uint64_t size) const
{
	if (size < 2)
		return;
	auto low = buffer;
	auto high = buffer + size - 1;
	while (size >= 2)
	{
		auto tmp = *low;
		*low = *high;
		*high = tmp;
		low += 2;
		high -= 2;
		size -= 2;
	}
}

//------------------------------------------------------------------------
} // vst3utils
