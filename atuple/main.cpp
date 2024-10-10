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
	//auto str = Foo_<MyStruct, &MyStruct::name_>();
	auto myTup = make_atuple_from_struct<MyStruct, &MyStruct::name_, &MyStruct::family_>(myStruct);
	auto myTup2 = myTup;
	myTup2.get<&MyStruct::name_>() = "Ilya";
	print_atuple(myTup);
	print_atuple(myTup2);

	auto other = myStruct;
	auto myTup3 = make_atuple_from_struct< MyStruct, &MyStruct::name_, &MyStruct::family_>(std::move(other));
	print_atuple(myTup3);
	std::cout << other.name_ << " " << other.family_ << std::endl;
}