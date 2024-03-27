//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#pragma once

#include <cstdlib>

#ifdef _MSC_VER
#include <malloc.h>
#endif

//------------------------------------------------------------------------
namespace vst3utils {

//------------------------------------------------------------------------
struct standard_allocator
{
	static void* allocate (size_t numBytes) { return std::malloc (numBytes); }
	static void deallocate (void* ptr, size_t numButes) { std::free (ptr); }
};

//------------------------------------------------------------------------
template<typename T, typename allocatorT = standard_allocator>
struct buffer final
{
	buffer (size_t num_elements = 0) { allocate (num_elements); }
	~buffer () noexcept { deallocate (); }

	void allocate (size_t num_elements)
	{
		deallocate ();
		if (num_elements == 0u)
			return;
		buffer_ptr = reinterpret_cast<T*> (allocatorT::allocate (num_elements * sizeof (T)));
		num_buffer_elements = num_elements;
	}

	void fill (T value)
	{
		assert (buffer_ptr);
		std::fill_n (buffer_ptr, num_buffer_elements, value);
	}

	T& operator[] (size_t index)
	{
		assert (index < num_buffer_elements);
		return buffer_ptr[index];
	}

	const T& operator[] (size_t index) const
	{
		assert (index < num_buffer_elements);
		return buffer_ptr[index];
	}

	T* data () { return buffer_ptr; }
	const T* data () const { return buffer_ptr; }
	size_t size () const { return num_buffer_elements; }

private:
	void deallocate ()
	{
		if (!buffer_ptr)
			return;
		allocatorT::deallocate (buffer_ptr, num_buffer_elements * sizeof (T));
		buffer_ptr = nullptr;
		num_buffer_elements = 0u;
	}

	T* buffer_ptr {nullptr};
	size_t num_buffer_elements {0u};
};

//------------------------------------------------------------------------
template<size_t alignment>
struct alignment_allocator
{
	static void* allocate (size_t numBytes)
	{
		if (alignment == 0)
			return malloc (numBytes);
		void* data {nullptr};
#if SMTG_OS_MACOS && MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_15
		posix_memalign (&data, alignment, numBytes);
#elif defined(_MSC_VER)
		data = _aligned_malloc (numBytes, alignment);
#else
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

template<typename T>
using aligned_buffer = buffer<T>;

//------------------------------------------------------------------------
} // vst3utils
