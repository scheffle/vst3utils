//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#pragma once

#include <cstdlib>
#include <algorithm>
#include <cassert>

#ifdef _MSC_VER
#include <malloc.h>
#endif

#if __APPLE__
#include <AvailabilityMacros.h>
#endif

//------------------------------------------------------------------------
namespace vst3utils {

//------------------------------------------------------------------------
/** standard allocator using malloc and free */
struct standard_allocator
{
	static void* allocate (size_t numBytes) { return std::malloc (numBytes); }
	static void deallocate (void* ptr, size_t numButes) { std::free (ptr); }
};

//------------------------------------------------------------------------
/** simple RAII buffer implementation */
template<typename T, typename allocatorT = standard_allocator>
struct buffer final
{
	buffer (size_t num_elements = 0) { allocate (num_elements); }
	~buffer () noexcept { deallocate (); }

	/** allocate */
	void allocate (size_t num_elements)
	{
		deallocate ();
		if (num_elements == 0u)
			return;
		buffer_ptr = reinterpret_cast<T*> (allocatorT::allocate (num_elements * element_size ()));
		if (buffer_ptr)
			num_buffer_elements = num_elements;
	}

	/** fill all elements in the buffer with the same value */
	void fill (T value)
	{
		assert (buffer_ptr);
		std::fill_n (buffer_ptr, num_buffer_elements, value);
	}

	/** access specified element */
	T& operator[] (size_t index)
	{
		assert (index < num_buffer_elements);
		return buffer_ptr[index];
	}

	/** access specified element */
	const T& operator[] (size_t index) const
	{
		assert (index < num_buffer_elements);
		return buffer_ptr[index];
	}

	/** returns a pointer to the first element */
	T* data () { return buffer_ptr; }
	/** returns a pointer to the first element */
	const T* data () const { return buffer_ptr; }

	/** returns the number of elements */
	size_t size () const { return num_buffer_elements; }
	/** returns the byte size of one element */
	constexpr size_t element_size () const { return sizeof (T); }

	/** returns an iterator to the beginning */
	T* begin () { return &buffer_ptr[0]; }
	/** returns an iterator to the end */
	T* end () { return &buffer_ptr[size ()]; }

	/** returns an iterator to the beginning */
	const T* begin () const { return &buffer_ptr[0]; }
	/** returns an iterator to the end */
	const T* end () const { return &buffer_ptr[size ()]; }

private:
	void deallocate ()
	{
		if (!buffer_ptr)
			return;
		allocatorT::deallocate (buffer_ptr, num_buffer_elements * element_size ());
		buffer_ptr = nullptr;
		num_buffer_elements = 0u;
	}

	T* buffer_ptr {nullptr};
	size_t num_buffer_elements {0u};
};

//------------------------------------------------------------------------
/** allocator that uses memory aligned allocations */
template<size_t alignment>
struct alignment_allocator
{
	static void* allocate (size_t numBytes)
	{
		if (alignment == 0)
			return malloc (numBytes);
		void* data {nullptr};
#if __APPLE__ && MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_15
		posix_memalign (&data, alignment, numBytes);
#elif defined(_MSC_VER)
		data = _aligned_malloc (numBytes, alignment);
#else
		size_t d = numBytes % alignment;
		if (d != 0u)
			numBytes += (alignment - d);
		data = std::aligned_alloc (alignment, numBytes);
#endif
		return data;
	}
	static void deallocate (void* ptr, size_t numButes)
	{
		if (alignment == 0)
			std::free (ptr);
		else
		{
#if defined(_MSC_VER)
			_aligned_free (ptr);
#else
			std::free (ptr);
#endif
		}
	}
};

//------------------------------------------------------------------------
template<typename T, size_t alignment>
using aligned_buffer = buffer<T, alignment_allocator<alignment>>;

//------------------------------------------------------------------------
} // vst3utils
