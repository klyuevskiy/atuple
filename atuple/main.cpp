#include <iostream>
#include <cstddef>

#include "atuple.hpp"

template <typename KeyT, typename ValueT, typename ...Types>
void __print_atuple(auto const& t)
{
	std::cout << typeid(KeyT).name() << " : " << t. template get<KeyT>() << std::endl;
	__print_atuple<Types...>(t);
}

void __print_atuple(auto const &t)
{

}

template <typename ...Types>
void print_atuple(atuple<Types...> const& t)
{
	std::cout << typeid(decltype(t)).name() << ":" << std::endl;
	__print_atuple<Types...>(t);
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

	//auto cmp_res = atuple_comparator<
	//	atuple_weak_ordering_policy,
	//	member_pointer<&MyStruct::name>,
	//	member_pointer<&MyStruct::age>
	//>::do_compare(t1, t2);

	//if (cmp_res == std::partial_ordering::greater)
	//	std::cout << "greater" << std::endl;
}