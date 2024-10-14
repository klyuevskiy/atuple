#include <iostream>
#include <cstddef>

#include "atuple.hpp"

template <typename KeyT, typename ValueT, typename ...Types>
void __print_atuple(atuple<KeyT, ValueT, Types...> const& t)
{
	std::cout << typeid(KeyT).name() << " : " << t. template get<KeyT>() << std::endl;
	__print_atuple(t.getTail());
}

void __print_atuple(atuple<> const &t)
{

}

template <typename Tup>
void print_atuple(Tup const& t)
{
	std::cout << typeid(Tup).name() << ":" << std::endl;
	__print_atuple(t);
}

struct MyStruct
{

	std::string name_;

	std::string family_;

	std::uint8_t age_;

	bool male_;
};

MyStruct myStruct{ "Name","Family",30,true };

struct Mystr
{
	std::string& str;
	Mystr(std::string &str) : str(str) {}
};

struct OtherStruct
{
	int age;
};

struct atuple_weak_ordering_policy
{
	template <typename T, typename U>
	static std::weak_ordering compare(T const& first, U const& second)
	{
		return first <=> second;
	}

	static std::weak_ordering combine(std::weak_ordering first, std::weak_ordering second)
	{
		if (first != std::partial_ordering::equivalent)
			return first;
		return second;
	}

	static constexpr std::weak_ordering default_value = std::weak_ordering::equivalent;
};

int main()
{
	struct MyStruct
	{
		std::string name;
		int age;

		auto operator <=>(MyStruct const& other) const = default;
	};

	MyStruct my1{ "Ilya", 22 };
	MyStruct my2{ "Ilya", 21 };

	auto t1 = make_atuple_from_struct<MyStruct, &MyStruct::name, &MyStruct::age>(my1);
	auto t2 = make_atuple_from_struct<MyStruct, &MyStruct::name, &MyStruct::age>(my2);

	std::cout << atuple_greater<&MyStruct::name, &MyStruct::age>(t1, t2) << std::endl;

	auto cmp_res = atuple_comparator<
		atuple_weak_ordering_policy,
		member_pointer<&MyStruct::name>,
		member_pointer<&MyStruct::age>
	>::do_compare(t1, t2);

	if (cmp_res == std::partial_ordering::greater)
		std::cout << "greater" << std::endl;
}