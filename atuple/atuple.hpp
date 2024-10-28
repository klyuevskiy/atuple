#pragma once

#include <type_traits>
#include <compare>

/* wrap type for pointer to member */
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

template <typename ...Types>
class atuple;

template <typename HeadKeyT, typename HeadValueT, typename ...Tail>
class atuple<HeadKeyT, HeadValueT, Tail...>
{
private:
	
	template <typename ...Types>
	friend class atuple;

	HeadValueT head;
	atuple<Tail...> tail;

	/* check for unique keys */

	template <typename SearchType, typename ...TypesForSearch>
	struct type_contains;

	template <typename SearchType, typename VHeadKey, typename VHeadValue, typename ...VTail>
	struct type_contains<SearchType, VHeadKey, VHeadValue, VTail...>
	{
		static constexpr bool value =
			std::is_same_v<SearchType, VHeadKey> ||
			type_contains<SearchType, VTail...>::value;
	};

	template<typename SearchType>
	struct type_contains<SearchType>
	{
		static constexpr bool value = false;
	};

	template <typename ...Types>
	struct is_unique_keys;

	template <typename VHeadKey, typename VHeadValue, typename ...VTail>
	struct is_unique_keys<VHeadKey, VHeadValue, VTail...>
	{
		static constexpr bool value =
			is_unique_keys<VTail...>::value &&
			!type_contains<VHeadKey, VTail...>::value;
	};

	template <>
	struct is_unique_keys<>
	{
		static constexpr bool value = true;
	};

	static_assert(
		is_unique_keys<HeadKeyT, HeadValueT, Tail...>::value,
		"atuple need to unique key types");

	constexpr HeadValueT& getHead() noexcept
	{
		return head;
	}

	constexpr HeadValueT const& getHead() const noexcept
	{
		return head;
	}

	constexpr atuple<Tail...>& getTail() noexcept
	{
		return tail;
	}

	constexpr atuple<Tail...> const& getTail() const noexcept
	{
		return tail;
	}

public:

	constexpr atuple()
		: head(HeadValueT{}), tail(atuple<Tail...>{})
	{
		
	}

	template <typename ValueT, typename TailTuple>
	constexpr atuple(ValueT&& head, TailTuple&& tail)
		: head(std::forward<ValueT>(head)),
		  tail(std::forward<TailTuple>(tail))
	{

	}

	~atuple() noexcept = default;

	template <typename FindKeyT>
		requires type_contains<FindKeyT, HeadKeyT, HeadValueT, Tail...>::value
	constexpr auto& get() noexcept
	{
		if constexpr (std::is_same_v<HeadKeyT, FindKeyT>)
			return this->getHead();
		else
			return this->getTail(). template get<FindKeyT>();
	}

	template <typename FindKeyT>
		requires type_contains<FindKeyT, HeadKeyT, HeadValueT, Tail...>::value
	constexpr auto const& get() const noexcept
	{
		if constexpr (std::is_same_v<HeadKeyT, FindKeyT>)
			return this->getHead();
		else
			return this->getTail(). template get<FindKeyT>();
	}

	template <auto mem_ptr>
		requires type_contains<member_pointer<mem_ptr>, HeadKeyT, HeadValueT, Tail...>::value
	constexpr typename member_pointer<mem_ptr>::member_type const& get() const noexcept
	{
		return this->template get<member_pointer<mem_ptr>>();
	}

	template <auto mem_ptr>
		requires type_contains<member_pointer<mem_ptr>, HeadKeyT, HeadValueT, Tail...>::value
	constexpr typename member_pointer<mem_ptr>::member_type& get() noexcept
	{
		return this-> template get<member_pointer<mem_ptr>>();
	}

	template<typename Atuple>
	constexpr atuple(Atuple const& other)
		: head(other. template get<HeadKeyT>()), tail(other)
	{

	}

	template<typename Atuple>
	constexpr atuple(Atuple&& other)
		: head(std::move(other. template get<HeadKeyT>())), tail(std::move(other))
	{

	}

private:
	/* swap(a, b) != swap(b, a) because truncate types by first argument */
	/* because it private, use it for copy and swap, move and swap only */
	template <typename Tup>
	constexpr void __swap(Tup& t) noexcept
	{
		using std::swap;
		swap(head, t. template get<HeadKeyT>());
		if constexpr (sizeof...(Tail) > 0)
			this->getTail().__swap(t);
	}

public:
	template <typename ...Types>
		requires requires()
	{
		requires sizeof...(Types) == sizeof...(Tail) + 2;
	}
	constexpr void swap(atuple<Types...>& t) noexcept
	{
		this->__swap(t);
	}

	template <typename ...Types>
	constexpr auto& operator=(atuple<Types...> const& other)
	{
		/* self-assigment check don't work with constexpr */
		//if (reinterpret_cast<char const*>(this) == reinterpret_cast<char const*>(&other))
		//	return *this;
		atuple<HeadKeyT, HeadValueT, Tail...> tmp(other);
		this->__swap(tmp);
		return *this;
	}

	template <typename ...Types>
	constexpr auto& operator=(atuple<Types...>&& other)
	{
		/* self-assigment check don't work with constexpr */
		//if (reinterpret_cast<char*>(this) == reinterpret_cast<char*>(&other))
		//	return *this;
		atuple<HeadKeyT, HeadValueT, Tail...> tmp(std::move(other));
		this->__swap(tmp);
		return *this;
	}

	template <typename Atuple>
	constexpr void fill_from(Atuple&& other)
	{
		*this = std::forward<Atuple>(other);
	}

private:

	/* == operation: compare value by value by first tuple keys */
	template <typename Tup1, typename Tup2>
	struct equal_op;

	template <typename VHeadKeyT, typename VHeadValueT, typename ...VTail, typename ...Types>
	struct equal_op<atuple<VHeadKeyT, VHeadValueT, VTail...>, atuple<Types...>>
	{
		static constexpr bool apply(atuple<VHeadKeyT, VHeadValueT, VTail...> const& first, atuple<Types...> const& second)
		{
			return
				first.getHead() == second. template get<VHeadKeyT>() &&
				equal_op<atuple<VTail...>, atuple<Types...>>::apply(first.getTail(), second);
		}
	};

	template <typename ...Types>
	struct equal_op<atuple<>, atuple<Types...>>
	{
		static constexpr bool apply(atuple<> const& first, atuple<Types...> const& second)
		{
			return true;
		}
	};

	template <typename ...Types1, typename ...Types2>
	friend constexpr bool operator==(atuple<Types1...> const& first, atuple<Types2...> const& second);
};

template <typename ...Types1, typename ...Types2>
constexpr bool operator==(atuple<Types1...> const& first, atuple<Types2...> const& second)
{
	static_assert(sizeof...(Types1) == sizeof...(Types2), "cannot compare atuple of different sizes");
	return atuple<Types1...>:: template equal_op<atuple<Types1...>, atuple<Types2...>>::apply(first, second);
}

constexpr bool operator==(atuple<> const& first, atuple<> const& second)
{
	return true;
}

template <typename ATup1, typename ATup2>
constexpr bool operator!=(ATup1 const& first, ATup2 const& second)
{
	return !(first == second);
}

template <>
class atuple<>
{
public:
	constexpr atuple() noexcept
	{

	}

	~atuple() noexcept = default;

	template<typename Atuple>
	constexpr atuple(Atuple const& other) noexcept
	{

	}

	template<typename Atuple>
	constexpr atuple(Atuple&& other) noexcept
	{

	}

	template <typename ...Types>
	constexpr auto& operator=(atuple<Types...> const& other) noexcept
	{
		return *this;
	}

	template <typename ...Types>
	constexpr auto& operator=(atuple<Types...>&& other) noexcept
	{
		return *this;
	}

	constexpr void swap(atuple<>& t) noexcept
	{

	}
};

/* aux struct for deduction atuple type by member pointers */
template <auto ...ptrs>
struct atuple_type_from_ptrs;

template <auto ptr, auto ...ptrs>
struct atuple_type_from_ptrs<ptr, ptrs...>
{
private:
	template <typename HeadKey, typename HeadValue, typename Atuple>
	struct atuple_add_pair;

	template <typename HeadKey, typename HeadValue, typename ...Types>
	struct atuple_add_pair<HeadKey, HeadValue, atuple<Types...>>
	{
		using type = atuple<HeadKey, HeadValue, Types... >;
	};

	using key_type = member_pointer<ptr>;
	using value_type = typename key_type::member_type;
	using tail = typename atuple_type_from_ptrs<ptrs...>::type;
public:
	using type = typename atuple_add_pair<key_type, value_type, tail>::type;
};

template <>
struct atuple_type_from_ptrs<>
{
	using type = atuple<>;
};

template <auto ...ptrs>
struct is_pointers_of_single_struct;

template <auto ptr1, auto ptr2, auto ...ptrs>
struct is_pointers_of_single_struct<ptr1, ptr2, ptrs...>
{
	constexpr static bool value = std::is_same_v<
		typename member_pointer<ptr1>::struct_type,
		typename member_pointer<ptr2>::struct_type
	> && is_pointers_of_single_struct<ptr2, ptrs...>::value;
};

template <auto ptr>
struct is_pointers_of_single_struct<ptr>
{
	constexpr static bool value = true;
};

template <>
struct is_pointers_of_single_struct<>
{
	constexpr static bool value = true;
};

constexpr atuple<> atuple_from_struct(auto const& st) noexcept
{
	return atuple<>{};
}

template <auto ptr, auto ... ptrs>
constexpr auto atuple_from_struct(auto const& st)
	requires
		is_pointers_of_single_struct<ptr, ptrs...>::value &&
		std::is_lvalue_reference_v<decltype(st)>
{
	return typename atuple_type_from_ptrs<ptr, ptrs...>::type(
		st.*ptr,
		atuple_from_struct<ptrs...>(st)
	);
}

constexpr atuple<> atuple_from_struct(auto&& st) noexcept
{
	return atuple<>{};
}

template <auto ptr, auto ...ptrs>
constexpr auto atuple_from_struct(auto&& st)
	requires
		is_pointers_of_single_struct<ptr, ptrs...>::value &&
		std::is_rvalue_reference_v<decltype(st)>
{
	return typename atuple_type_from_ptrs<ptr, ptrs...>::type(
		std::move(st.*ptr),
		atuple_from_struct<ptrs...>(std::move(st))
	);
}

template <auto ...ptrs>
	requires requires() {
	requires sizeof...(ptrs) == 0;
}
constexpr atuple<> atuple_from_struct() noexcept
{
	return atuple<>{};
}

template<auto ptr, auto ...ptrs>
requires is_pointers_of_single_struct<ptr, ptrs...>::value
constexpr typename atuple_type_from_ptrs<ptr, ptrs...>::type atuple_from_struct()
{
	return typename atuple_type_from_ptrs<ptr, ptrs...>::type(
		typename member_pointer<ptr>::member_type{},
		atuple_from_struct<ptrs...>()
	);
}

/* use policy:
* Use user-defind type for compare result (CmpT)
* template <T, U>
* static CmpT compare(T const & first, U const & second)
*  compare 2 values
* static CmpT default_value
	default value to compare_result
* static bool combine_results(CmpT fist, CmpT second)
*	method to combine 2 compare results
*/
template <typename ComparePolicy, typename ...Types>
struct atuple_comparator;

template <typename ComparePolicy, typename HeadKey, typename ...Types>
struct atuple_comparator<ComparePolicy, HeadKey, Types...>
{
	template <typename Atuple1, typename Atuple2>
	static constexpr auto do_compare(Atuple1 const& first, Atuple2 const& second)
	{
		auto const& left = first.template get<HeadKey>();
		auto const& right = second.template get<HeadKey>();

		return ComparePolicy::combine(
				ComparePolicy::compare(left, right),
				atuple_comparator<ComparePolicy, Types...>::do_compare(first, second)
			);
	}
};

template <typename ComparePolicy>
struct atuple_comparator<ComparePolicy>
{
	template <typename Atuple1, typename Atuple2>
	static constexpr auto do_compare(Atuple1 const& first, Atuple2 const& second)
	{
		return ComparePolicy::default_value;
	}
};

struct atuple_less_policy
{
	template <typename T, typename U>
	static constexpr bool compare(T const& first, U const& second) noexcept
	{
		return first < second;
	}

	static constexpr bool combine(bool first, bool second) noexcept
	{
		return first || second;
	}

	static constexpr bool default_value = false;
};

struct atuple_greater_policy : atuple_less_policy
{
	template <typename T, typename U>
	static constexpr bool compare(T const& first, U const& second) noexcept
	{
		return first > second;
	}
};

struct atuple_equal_policy
{
	template <typename T, typename U>
	static constexpr bool compare(T const& first, U const& second) noexcept
	{
		return first == second;
	}

	static constexpr bool combine(bool first, bool second) noexcept
	{
		return first && second;
	}

	static constexpr bool default_value = true;
};

template <typename ...Types>
constexpr bool atuple_less(auto const& t1, auto const& t2)
{
	return atuple_comparator<atuple_less_policy, Types...>::do_compare(t1, t2);
}

template <auto ...Ptrs>
constexpr bool atuple_less(auto const& t1, auto const& t2)
{
	return atuple_less<member_pointer<Ptrs>...>(t1, t2);
}

template <typename ...Types>
constexpr bool atuple_greater(auto const& t1, auto const& t2)
{
	return atuple_comparator<atuple_greater_policy, Types...>::do_compare(t1, t2);
}

template <auto ...Ptrs>
constexpr bool atuple_greater(auto const& t1, auto const& t2)
{
	return atuple_greater<member_pointer<Ptrs>...>(t1, t2);
}

template <typename ...Types>
constexpr bool atuple_equal(auto const& t1, auto const& t2)
{
	return atuple_comparator<atuple_equal_policy, Types...>::do_compare(t1, t2);
}

template <auto ...Ptrs>
constexpr bool atuple_equal(auto const& t1, auto const& t2)
{
	return atuple_equal<member_pointer<Ptrs>...>(t1, t2);
}