//------------------------------------------------------------------------
/* This source code is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//------------------------------------------------------------------------

#pragma once

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
 *	when the listener token is destroyed the listener is removed from the
 *	observable object, and when the observable object is destroyed the listeners
 *	can check the tokens object_alive() function if the object is still valid.
 *
 *	not thread safe, listeners and objects can only be used on the same thread
 *
 *	Example:
 *
 *		using observable_string = observable<std::string>;
 *
 *		observable_string my_string;
 *		auto listener_token = my_string.add_listener ([] (const std::string& str) { ... });
 *		my_string.edit ([] (const std::string& str) { str = "test"; return true; });
 *
 */
template<typename T>
class observable
{
public:
	struct listener_token
	{
		~listener_token () noexcept
		{
			if (token_destroyed)
				token_destroyed (this);
		}

		bool object_alive () const noexcept { return token_destroyed != nullptr; }

	private:
		using token_destroyed_callback = std::function<void (listener_token*)>;

		void object_destroyed () { token_destroyed = {}; }

		token_destroyed_callback token_destroyed;
		friend class observable<T>;
	};
	using listener_token_ptr = std::unique_ptr<listener_token>;

	template<typename std::enable_if_t<std::is_default_constructible_v<T>>* = nullptr>
	observable ()
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

	template<typename Proc>
	bool edit (Proc proc)
	{
		if (is_editing > 0)
			return false;
		++is_editing;
		if (proc (value))
			notify_listeners ();
		--is_editing;
		return true;
	}

	using listener = std::function<void (const T&)>;
	listener_token_ptr add_listener (listener&& listener) const;

private:
	void notify_listeners ();
	void remove_listener (listener_token* token);

	T value;
	size_t is_editing {};
	mutable std::vector<std::pair<listener_token*, listener>> listeners;
};

//------------------------------------------------------------------------
template<typename T>
observable<T>::~observable () noexcept
{
	for (auto& token : listeners)
		token.first->value_destroyed ();
}

//------------------------------------------------------------------------
template<typename T>
auto observable<T>::add_listener (listener&& listener) const -> listener_token_ptr
{
	auto token = std::make_unique<listener_token> ();
	token->token_destroyed = [This = const_cast<observable<T>*> (this)] (auto* token) {
		This->remove_listener (token);
	};
	listeners.emplace_back (token.get (), std::move (listener));
	return token;
}

//------------------------------------------------------------------------
template<typename T>
void observable<T>::remove_listener (listener_token* token)
{
	auto it = std::find_if (listeners.begin (), listeners.end (),
							[&] (const auto& el) { return el.first == token; });
	if (it != listeners.end ())
	{
		if (is_editing != 0u)
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
	assert (is_editing == 1);
	for (auto it = listeners.begin (); it != listeners.end (); ++it)
	{
		if (it->second)
			it->second (value);
		else
			it = listeners.erase (it);
	}
}

//------------------------------------------------------------------------
} // vst3utils
