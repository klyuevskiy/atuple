#pragma once

#include <type_traits>

template <typename ...Types>
class atuple;

template <auto member_ptr>
struct member_pointer
{
private:
	template<typename StT, typename T>
	static T __member_type(T StT::* ptr);

	template<typename StT, typename T>
	static StT __struct_type(T StT::* ptr);

public:
	using member_type = decltype(__member_type(member_ptr));
	using struct_type = decltype(__struct_type(member_ptr));
};

template <typename KeyT, typename ValueT, typename ...Tail>
class atuple<KeyT, ValueT, Tail...>
{
private:

	/* check for unique keys */

	template <typename SearchType, typename ...TypesForSearch>
	struct type_contains;

	template <typename SearchType, typename HeadKey, typename HeadValue, typename ...Tail>
	struct type_contains<SearchType, HeadKey, HeadValue, Tail...>
	{
		static constexpr bool value =
			std::is_same_v<SearchType, HeadKey> ||
			type_contains<SearchType, Tail...>::value;
	};

	template<typename SearchType>
	struct type_contains<SearchType>
	{
		static constexpr bool value = false;
	};

	template <typename ...Types>
	struct is_unique_keys;

	template <typename KeyT, typename ValueT, typename ...Tail>
	struct is_unique_keys<KeyT, ValueT, Tail...>
	{
		static constexpr bool value =
			is_unique_keys<Tail...>::value &&
			!type_contains<KeyT, Tail...>::value;
	};

	template <>
	struct is_unique_keys<>
	{
		static constexpr bool value = true;
	};

	static_assert(
		is_unique_keys<KeyT, ValueT, Tail...>::value,
		"atuple need to unique key types");

	using key_type = KeyT;
	using value_type = ValueT;

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

	template <auto mem_ptr>
	member_pointer<mem_ptr>::member_type & get()
	{
		return get<member_pointer<mem_ptr>>();
	}

	template <auto mem_ptr>
	member_pointer<mem_ptr>::member_type const& get() const
	{
		return get<member_pointer<mem_ptr>>();
	}

public:
	template <typename VValueT, typename ...VTail>
	atuple(VValueT&& head, atuple<VTail...>&& tail)
		: head(std::forward<VValueT>(head)),
		  tail(std::forward<atuple<VTail...>>(tail))
	{

	}

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

template <typename HeadKeyT, typename HeadValueT, typename ...Tail>
atuple<HeadKeyT, HeadValueT, Tail...>
make_atuple(HeadValueT const & value, atuple<Tail...>&& tail)
{
	return atuple<HeadKeyT, HeadValueT, Tail...>(value, std::move(tail));
}

template <typename HeadKeyT, typename HeadValueT, typename ...Tail>
atuple<HeadKeyT, HeadValueT, Tail...>
make_atuple(HeadValueT && value, atuple<Tail...>&& tail)
{
	return atuple<HeadKeyT, HeadValueT, Tail...>(std::move(value), std::move(tail));
}

template <typename StructType, auto StructType::*ptr, auto StructType::*... params>
auto make_atuple_from_struct(StructType const &st)
{
	auto tail = make_atuple_from_struct<StructType, params...>(st);
	using member_type = std::remove_reference_t<decltype(std::declval<StructType>().*ptr)>;
	return make_atuple<member_pointer<ptr>, member_type>(st.*ptr, std::move(tail));
}

template <typename StructType, auto StructType::* ptr, auto StructType::*... params>
auto make_atuple_from_struct(StructType && st)
{
	auto member = std::move(st.*ptr);
	auto tail = make_atuple_from_struct<StructType, params...>(std::move(st));
	using member_type = std::remove_reference_t<decltype(std::declval<StructType>().*ptr)>;
	return make_atuple<member_pointer<ptr>, member_type>(std::move(member), std::move(tail));
}

template <typename StructType>
auto make_atuple_from_struct(StructType const& st)
{
	return atuple<>();
}