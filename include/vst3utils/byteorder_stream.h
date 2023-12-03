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
/** byte order */
enum class byte_order
{
	little_endian = kLittleEndian,
	big_endian = kBigEndian,
	native_endian = BYTEORDER
};

//------------------------------------------------------------------------
/** seek mode */
enum class seek_mode
{
	set,
	current,
	end
};

//------------------------------------------------------------------------
/** I/O result */
struct io_result
{
	Steinberg::tresult return_code {};
	size_t bytes {};
	operator bool () const { return return_code == Steinberg::kResultTrue; }
};

//------------------------------------------------------------------------
/** an adapter to read/write byte ordered data to an IBStream

The `stream_byte_order` template argument configures the byte order at compile time so that at
runtime no check needs to be done if byte swapping needs to be done.

The `throw_on_error` template argument configures at compile time if an exception is thrown the
underlying IBStream returns anything else than kResultTrue.

Examples:

1) Writing simple types:

	auto s = make_byte_order_stream<byte_order::little_endian> (ib_stream);
	double my_double = 5.88;
	stream << my_double;
	int my_integer = 19;
	stream << my_integer;

2) Reading simple types:

	auto s = make_byte_order_stream<byte_order::little_endian> (ib_stream);
	double my_double;
	stream >> my_double;
	int my_integer;
	stream >> my_integer;

3) Writing an arry of data

	auto s = make_byte_order_stream<byte_order::little_endian> (ib_stream);
	std::vector<double> data;
	data.resize (100);
	stream.write (data.begin (), data.end ());

3) Reading an arry of data

	auto s = make_byte_order_stream<byte_order::little_endian> (ib_stream);
	std::vector<double> data;
	data.resize (100);
	stream.read (data.begin (), data.end ());

 */
template<byte_order stream_byte_order, bool throw_on_error = false>
struct byte_order_ibstream
{
	using ibstream_ptr = Steinberg::IPtr<Steinberg::IBStream>;

	byte_order_ibstream (ibstream_ptr&& stream);
	byte_order_ibstream (byte_order_ibstream&&) = default;
	byte_order_ibstream (const byte_order_ibstream&) = default;
	byte_order_ibstream& operator= (byte_order_ibstream&&) = default;
	byte_order_ibstream& operator= (const byte_order_ibstream&) = default;

	/** seek the stream to a new position */
	io_result seek (seek_mode mode, int64_t position);
	/** return the current stream position */
	io_result tell () const;

	/** read byte ordered data */
	template<typename T>
	io_result operator>> (T& output) const;
	/** write byte ordered data */
	template<typename T>
	io_result operator<< (const T& input);

	/** read byte ordered data */
	template<typename iterator_t>
	io_result read (iterator_t begin, iterator_t end) const;
	/** write byte ordered data */
	template<typename iterator_t>
	io_result write (iterator_t begin, iterator_t end);

	/** read byte ordered continues data */
	template<typename T>
	io_result read (T* dest, size_t count) const;
	/** write byte ordered continues data */
	template<typename T>
	io_result write (const T* src, size_t count);

	/** read bytes without byte order conversion */
	io_result read_raw (void* dest, size_t num_bytes) const;
	/** write bytes without byte order conversion */
	io_result write_raw (const void* src, size_t num_bytes);

private:
	template<size_t size>
	io_result swapAndWrite (const uint8_t* buffer);
	void swap (uint8_t* buffer, uint64_t size) const;

	ibstream_ptr stream;
};

//------------------------------------------------------------------------
/** make a byte ordered stream */
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
	ibstream_ptr&& stream)
: stream (std::move (stream))
{
}

//------------------------------------------------------------------------
template<byte_order stream_byte_order, bool throw_on_error>
inline auto byte_order_ibstream<stream_byte_order, throw_on_error>::seek (seek_mode mode,
																		  int64_t position)
	-> io_result
{
	Steinberg::int64 new_position {};
	auto result = stream->seek (position, static_cast<Steinberg::int32> (mode), &new_position);
	if constexpr (throw_on_error)
	{
		if (result != Steinberg::kResultTrue)
			throw io_error ("seek failure", result, position);
	}
	return {result, static_cast<size_t> (new_position)};
}

//------------------------------------------------------------------------
template<byte_order stream_byte_order, bool throw_on_error>
inline auto byte_order_ibstream<stream_byte_order, throw_on_error>::tell () const -> io_result
{
	Steinberg::int64 pos;
	auto result = stream->tell (&pos);
	if constexpr (throw_on_error)
	{
		if (result != Steinberg::kResultTrue)
			throw io_error ("tell failure", result, pos);
	}
	return {result, static_cast<size_t> (pos)};
}

//------------------------------------------------------------------------
template<byte_order stream_byte_order, bool throw_on_error>
inline auto byte_order_ibstream<stream_byte_order, throw_on_error>::read_raw (
	void* dest, size_t num_bytes) const -> io_result
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
inline auto byte_order_ibstream<stream_byte_order, throw_on_error>::write_raw (const void* src,
																			   size_t num_bytes)
	-> io_result
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
inline auto byte_order_ibstream<stream_byte_order, throw_on_error>::read (T* dest,
																		  size_t count) const
	-> io_result
{
	auto result = read_raw (dest, count * sizeof (T));
	if constexpr (stream_byte_order != byte_order::native_endian)
	{
		if (result)
		{
			for (auto i = 0u; i < count; ++i)
				swap (reinterpret_cast<uint8_t*> (&dest[i]), sizeof (T));
		}
	}
	return result;
}

//------------------------------------------------------------------------
template<byte_order stream_byte_order, bool throw_on_error>
template<typename T>
inline auto byte_order_ibstream<stream_byte_order, throw_on_error>::write (const T* src,
																		   size_t count)
	-> io_result
{
	if constexpr (stream_byte_order != byte_order::native_endian)
	{
		size_t written_bytes {};
		for (auto i = 0u; i < count; ++i)
		{
			auto res = (*this << src[i]);
			if (!res)
				return {res.return_code, written_bytes + res.bytes};
			written_bytes += res.bytes;
		}
		return {Steinberg::kResultTrue, written_bytes};
	}
	return write_raw (src, count * sizeof (T));
}

//------------------------------------------------------------------------
template<byte_order stream_byte_order, bool throw_on_error>
template<typename iterator_t>
inline auto byte_order_ibstream<stream_byte_order, throw_on_error>::read (iterator_t begin,
																		  iterator_t end) const
	-> io_result
{
	size_t read_bytes {};
	while (begin != end)
	{
		auto sr = (*this >> *begin);
		if (!sr)
			return {sr.return_code, read_bytes + sr.bytes};
		read_bytes += sr.bytes;
		++begin;
	}
	return {Steinberg::kResultTrue, read_bytes};
}

//------------------------------------------------------------------------
template<byte_order stream_byte_order, bool throw_on_error>
template<typename iterator_t>
inline auto byte_order_ibstream<stream_byte_order, throw_on_error>::write (iterator_t begin,
																		   iterator_t end)
	-> io_result
{
	size_t written_bytes {};
	while (begin != end)
	{
		auto sr = (*this << *begin);
		if (!sr)
			return {sr.return_code, written_bytes + sr.bytes};
		written_bytes += sr.bytes;
		++begin;
	}
	return {Steinberg::kResultTrue, written_bytes};
}

//------------------------------------------------------------------------
template<byte_order stream_byte_order, bool throw_on_error>
template<typename T>
inline auto byte_order_ibstream<stream_byte_order, throw_on_error>::operator<< (const T& input)
	-> io_result
{
	static_assert (std::is_standard_layout<T>::value, "Supports only standard layout types");
	if constexpr (stream_byte_order == byte_order::native_endian)
		return write_raw (static_cast<const void*> (&input), sizeof (T));

	return swapAndWrite<sizeof (T)> (reinterpret_cast<const uint8_t*> (&input));
}

//------------------------------------------------------------------------
template<byte_order stream_byte_order, bool throw_on_error>
template<typename T>
inline auto byte_order_ibstream<stream_byte_order, throw_on_error>::operator>> (T& output) const
	-> io_result
{
	static_assert (std::is_standard_layout<T>::value, "Supports only standard layout types");
	auto res = read_raw (&output, sizeof (T));
	if constexpr (stream_byte_order == byte_order::native_endian)
		return res;

	swap (reinterpret_cast<uint8_t*> (&output), res.bytes);
	return res;
}

//------------------------------------------------------------------------
template<byte_order stream_byte_order, bool throw_on_error>
template<size_t _size>
inline auto
	byte_order_ibstream<stream_byte_order, throw_on_error>::swapAndWrite (const uint8_t* buffer)
		-> io_result
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
		return write_raw (tmp, _size);
	}
	return write_raw (buffer, 1);
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
