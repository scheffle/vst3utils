//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

//------------------------------------------------------------------------
namespace vst3utils {

//------------------------------------------------------------------------
/** observable template
 *
 *	an observable object wrapper that can inform listeners when the object changes
 *	without a need to store an object reference in the listener
 *
 *	when the observer token is destroyed the listener is removed from the observable object. And the
 *	destruction of the observable object can be monitored by setting the object destroyed callback
 *	in the observer token, and further the token can be asked if the object is still alive.
 *
 *	not thread safe, listeners and objects can only be used on the same thread
 *
 *	Example:
 *
 *		using observable_string = observable<std::string>;
 *
 *		observable_string my_string;
 *		auto token = my_string.add_listener ([] (const std::string& str) { ... });
 *		my_string.edit ([] (const std::string& str) { str = "test"; return true; });
 *
 */
template<typename T>
class observable;

//------------------------------------------------------------------------
template<typename T>
struct observable_token
{
	using object_destroyed_callback = std::function<void ()>;

	observable_token () = default;
	~observable_token () noexcept
	{
		if (token_destroyed_cb)
			token_destroyed_cb (this);
	}

	bool object_alive () const noexcept { return token_destroyed_cb != nullptr; }

	void set_object_destroyed_callback (object_destroyed_callback&& f)
	{
		object_destroyed_cb = std::move (f);
	}
	void set_object_destroyed_callback (const object_destroyed_callback& f)
	{
		object_destroyed_cb = f;
	}

private:
	using token_destroyed_callback = std::function<void (observable_token*)>;

	void object_destroyed ()
	{
		token_destroyed_cb = {};
		if (object_destroyed_cb)
			object_destroyed_cb ();
	}

	token_destroyed_callback token_destroyed_cb;
	object_destroyed_callback object_destroyed_cb;
	friend class observable<T>;
};

template<typename T>
using observable_token_ptr = std::unique_ptr<observable_token<T>>;

//------------------------------------------------------------------------
template<typename T>
class observable
{
public:
	using token = observable_token<T>;
	using token_ptr = observable_token_ptr<T>;
	using listener = std::function<void (const T&)>;

	template<typename std::enable_if_t<std::is_default_constructible_v<T>>* = nullptr>
	observable () : value ()
	{
	}
	template<typename std::enable_if_t<std::is_copy_constructible_v<T>>* = nullptr>
	observable (const T& value) : value (value)
	{
	}
	template<typename std::enable_if_t<std::is_move_constructible_v<T>>* = nullptr>
	observable (T&& value) : value (std::move (value))
	{
	}
	observable (const observable&) = delete;
	observable (observable&&) = delete;
	~observable () noexcept;

	observable& operator= (const observable&) = delete;
	observable& operator= (observable&&) = delete;

	const T& get () const { return value; }
	operator const T* () const { return &get (); }
	const T* operator->() const { return *this; }

	template<typename Proc>
	bool edit (Proc proc);

	bool is_editing () const { return edit_count > 0; }

	[[nodiscard]] token_ptr add_listener (listener&& listener) const;
	void remove_listener (token_ptr& token) const;

private:
	void notify_listeners ();
	void remove_listener (token* token) const;

	T value;
	size_t edit_count {};
	mutable std::vector<std::pair<token*, listener>> listeners;
};

//------------------------------------------------------------------------
template<typename T>
observable<T>::~observable () noexcept
{
	for (auto& el : listeners)
		el.first->object_destroyed ();
}

//------------------------------------------------------------------------
template<typename T>
auto observable<T>::add_listener (listener&& listener) const -> token_ptr
{
	auto lt = std::make_unique<token> ();
	lt->token_destroyed_cb = [This = const_cast<observable<T>*> (this)] (auto* token) {
		This->remove_listener (token);
	};
	listeners.emplace_back (lt.get (), std::move (listener));
	return lt;
}

//------------------------------------------------------------------------
template<typename T>
void observable<T>::remove_listener (token_ptr& token) const
{
	token.reset ();
}

//------------------------------------------------------------------------
template<typename T>
void observable<T>::remove_listener (token* token) const
{
	auto it = std::find_if (listeners.begin (), listeners.end (),
							[&] (const auto& el) { return el.first == token; });
	if (it != listeners.end ())
	{
		if (edit_count != 0u)
		{
			it->second = nullptr;
		}
		else
		{
			listeners.erase (it);
		}
	}
}

//------------------------------------------------------------------------
template<typename T>
void observable<T>::notify_listeners ()
{
	assert (edit_count == 1);
	for (auto it = listeners.begin (); it != listeners.end ();)
	{
		if (it->second)
		{
			it->second (value);
			++it;
		}
		else
			it = listeners.erase (it);
	}
}

//------------------------------------------------------------------------
template<typename T>
template<typename Proc>
bool observable<T>::edit (Proc proc)
{
	if (edit_count > 0)
		return false;
	++edit_count;
	if (proc (value))
		notify_listeners ();
	--edit_count;
	return true;
}

//------------------------------------------------------------------------
} // vst3utils
