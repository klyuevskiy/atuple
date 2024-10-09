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

	template<typename Tup, typename FindKeyT>
	struct tuple_get;

	template<typename FindKeyT, typename ValueT, typename ...Tail>
	struct tuple_get<atuple<FindKeyT, ValueT, Tail...>, FindKeyT>
	{
		static ValueT& apply(atuple<FindKeyT, ValueT, Tail...>& t)
		{
			return t.getHead();
		}

		static ValueT const& apply(atuple<FindKeyT, ValueT, Tail...> const& t)
		{
			return t.getHead();
		}
	};

	template <typename FindKeyT, typename KeyT, typename ValueT, typename ...Tail>
	struct tuple_get<atuple<KeyT, ValueT, Tail...>, FindKeyT>
	{
		static auto& apply(atuple<KeyT, ValueT, Tail...>& t)
		{
			return tuple_get<atuple<Tail...>, FindKeyT>::apply(t.getTail());
		}

		static const auto& apply(atuple<KeyT, ValueT, Tail...>const& t)
		{
			return tuple_get<atuple<Tail...>, FindKeyT>::apply(t.getTail());
		}
	};

	template <typename FindKeyT>
	struct tuple_get<atuple<>, FindKeyT>
	{
		static int& apply(atuple<>& t)
		{
			static_assert(false, "type not found");
			int i{};
			return i;
		}

		static int const& apply(atuple<> const& t)
		{
			static_assert(false, "type not found");
			int i{};
			return i;
		}
	};

public:
	template <typename FindKeyT>
	auto& get()
	{
		return tuple_get<atuple<key_type, value_type, Tail...>, FindKeyT>::apply(*this);
	}

	template <typename FindKeyT>
	auto const& get() const
	{
		return tuple_get<atuple<key_type, value_type, Tail...>, FindKeyT>::apply(*this);
	}
};

template <>
class atuple<>
{
};