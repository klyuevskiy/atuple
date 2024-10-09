#include <iostream>

#include "atuple.hpp"

//void foo(atuple<int, double, unsigned, std::string> const & mem)
//{
//	std::cout << mem.get<int>() << std::endl << mem.get<unsigned>() << std::endl;
//}

template <typename KeyT, typename ValueT, typename ...Types>
void foo(atuple<KeyT, ValueT, Types...> const& t)
{
	std::cout << typeid(KeyT).name() << " : " << t.get<KeyT>() << std::endl;
	foo(t.getTail());
}

void foo(atuple<> const &t)
{

}

template <typename Tup>
void print_atuple(Tup const& t)
{
	std::cout << typeid(Tup).name() << ":" << std::endl;
	foo(t);
}

int main()
{
	using myType = atuple<int, int, unsigned, std::string>;
	myType mem;
	mem.get<int>() = 3;
	mem.get<unsigned>() = "mem";
	//mem.__get<float>();
	auto mem2 = mem;
	print_atuple(mem2);
	print_atuple(mem);
	atuple<int, int, long, long> t1;
	atuple<int, int> t2;
	//std::cout << (t1 == t2) << std::endl;
}