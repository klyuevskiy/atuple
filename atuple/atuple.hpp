#pragma once

#include <type_traits>

template <typename ...Types>
class atuple;

/* TODO: защита от повторени€ ключевых полей */
/* нужно как-то проверить что типы ключей не повтор€ютс€ */

template <typename KeyT, typename ValueT, typename ...Tail>
class atuple<KeyT, ValueT, Tail...>
{
public:
	using key_type = KeyT;
	using value_type = ValueT;

private:
	value_type head;
	atuple<Tail...> tail;

public:
	value_type& getHead()
	{
		return head;
	}

	value_type const& getHead() const
	{
		return head;
	}

	atuple<Tail...>& getTail()
	{
		return tail;
	}

	atuple<Tail...> const& getTail() const
	{
		return tail;
	}

	atuple()
		: head(value_type{}), tail(atuple<Tail...>{})
	{

	}

	atuple(ValueT const& value, atuple<Tail...> const& tail)
		: head(value), tail(tail)
	{

	}

private:

public:
	template <typename FindKeyT>
	auto& get()
	{
		if constexpr (std::is_same_v<key_type, FindKeyT>)
			return getHead();
		else if constexpr (sizeof...(Tail) == 0)
		{
			static_assert(false, "type not found in atuple");
			int i{};
			return i;
		}
		else
			return getTail().get<FindKeyT>();
	}

	template <typename FindKeyT>
	auto const& get() const
	{
		if constexpr (std::is_same_v<key_type, FindKeyT>)
			return getHead();
		else if constexpr (sizeof...(Tail) == 0)
		{
			static_assert(false, "type not found in atuple");
			int i{};
			return i;
		}
		else
			return getTail().get<FindKeyT>();
	}

public:
	template<typename ...Types>
	atuple(atuple<Types...> const& other)
		: head(other.get<key_type>()), tail(other)
	{

	}

	template<typename ...Types>
	atuple(atuple<Types...> && other)
		: head(std::move(other.get<key_type>())), tail(std::move(other))
	{

	}

	/* TODO: copy and swap */
	template <typename ...Types>
	auto& operator=(atuple<Types...> const& other)
	{
		head = other.get<key_type>();
		tail = other;
		return *this;
	}

	/* TODO: move and swap */
	template <typename ...Types>
	auto& operator=(atuple<Types...>&& other)
	{
		head = std::move(other.get<key_type>());
		tail = std::move(other);
		return *this;
	}

private:
	
	/* == operation: compare value by value by first tuple keys */
	template <typename Tup1, typename Tup2>
	struct equal_op;

	template <typename KeyT, typename ValueT, typename ...Tail, typename ...Types>
	struct equal_op<atuple<KeyT, ValueT, Tail...>, atuple<Types...>>
	{
		static bool apply(atuple<KeyT, ValueT, Tail...> const& first, atuple<Types...> const& second)
		{
			return
				first.getHead() == second.get<KeyT>() &&
				equal_op<atuple<Tail...>, atuple<Types...>>::apply(first.getTail(), second);
		}
	};

	template <typename ...Types>
	struct equal_op<atuple<>, atuple<Types...>>
	{
		static bool apply(atuple<> const& first, atuple<Types...> const& second)
		{
			return true;
		}
	};

	template <typename ...Types1, typename ...Types2>
	friend bool operator==(atuple<Types1...> const& first, atuple<Types2...> const& second);
};

template <typename ...Types1, typename ...Types2>
bool operator==(atuple<Types1...> const& first, atuple<Types2...> const& second)
{
	static_assert(sizeof...(Types1) == sizeof...(Types2), "cannot compare atuple of different sizes");
	return atuple<Types1...>::equal_op<atuple<Types1...>, atuple<Types2...>>::apply(first, second);
}

bool operator==(atuple<> const& first, atuple<> const& second)
{
	return true;
}

template <typename ATup1, typename ATup2>
bool operator!=(ATup1 const& first, ATup2 const& second)
{
	return !(first == second);
}

template <>
class atuple<>
{
public:
	atuple()
	{

	}

	template<typename ...Types>
	atuple(atuple<Types...> const& other)
	{

	}

	template<typename ...Types>
	atuple(atuple<Types...> && other)
	{

	}

	template <typename ...Types>
	auto& operator=(atuple<Types...> const& other)
	{
		return *this;
	}

	template <typename ...Types>
	auto& operator=(atuple<Types...>&& other)
	{
		return *this;
	}
};
