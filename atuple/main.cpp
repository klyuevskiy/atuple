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


int main()
{
	//foo(myStruct);
	//foo(std::move(myStruct));
	//foo<MyStruct>(myStruct);
	//auto t = make_atuple_from_struct<MyStruct, &MyStruct::name_, &MyStruct::family_, &MyStruct::age_>(std::move( myStruct));
	auto t = make_atuple_from_struct<MyStruct, &MyStruct::name_, &MyStruct::age_, &MyStruct::male_>(std::move(myStruct));
	std::cout << t.get<&MyStruct::name_>();
	//print_atuple(t);
	//std::cout << typeid(&MyStruct::name_).name() << std::endl;
	//std::cout << myStruct.name_ << myStruct.family_ << static_cast<int>(myStruct.age_);
}