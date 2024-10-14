#include <iostream>
#include <cstddef>

#include "atuple.hpp"

template <typename KeyT, typename ValueT, typename ...Types>
void __print_atuple(atuple<KeyT, ValueT, Types...> const& t)
{
	std::cout << typeid(KeyT).name() << " : " << t.get<KeyT>() << std::endl;
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

template <typename StrT, auto StrT::*ptr>
auto Foo_()
{
	return decltype(std::declval<StrT>().*ptr){};
}

struct OtherStruct
{
	int age;
};

template <typename T, typename U>
void foo(T t, U u)
{
	std::cout << t << std::endl;
	std::cout << u << std::endl;
}

int main()
{
	struct MyStruct
	{
		std::string name;
		int age;
	};

	MyStruct my{ "Ilya", 22 };

	auto t = make_atuple_from_struct<&MyStruct::name, &MyStruct::age>(my);
	print_atuple(t);
}