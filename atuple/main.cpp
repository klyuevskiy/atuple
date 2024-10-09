#include <iostream>

#include "atuple.hpp"

void foo(atuple<int, double, unsigned, std::string> const & mem)
{
	std::cout << mem.get<int>() << std::endl << mem.get<unsigned>();
}

class A
{
private:
	void f()
	{
		std::cout << "my hello";
	}

	class B;
	friend class B;

	class B
	{
	public:
		static void bar(A& a) { std::cout << "hello"; a.f(); }
	};

public:
	void foo()
	{
		B::bar(*this);
	}
};

int main()
{
	using myType = atuple<int, double, unsigned, std::string>;
	myType mem;
	mem.get<int>() = 3.3;
	mem.get<unsigned>() = "Ilya";
	/*atuple_get<int>(mem) = 3.3;
	atuple_get<unsigned>(mem) = "Ilya";*/
	myType mem2 = mem;

	A a;
	a.foo();
	foo(mem2);
}