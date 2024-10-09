#include "pch.h"
#include "CppUnitTest.h"
#include "../atuple/atuple.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace atupletests
{
	TEST_CLASS(atupletests)
	{
	public:

		TEST_METHOD(TestGet)
		{
			atuple<int, int, long, std::string, std::string, const char *> t;
			t.get<int>() = 123;
			t.get<long>() = "string";
			t.get<std::string>() = "const char *";

			/* non-const get */
			Assert::AreEqual(123, t.get<int>());
			Assert::AreEqual(std::string("string"), t.get<long>());
			Assert::AreEqual("const char *", t.get<std::string>());

			auto const& t_ref = t;

			/* const get */
			Assert::AreEqual(123, t_ref.get<int>());
			Assert::AreEqual(std::string("string"), t_ref.get<long>());
			Assert::AreEqual("const char *", t_ref.get<std::string>());
		}

		TEST_METHOD(TestEqual)
		{
			using type = atuple<int, std::string, float, long>;
			type first;
			first.get<int>() = "first";
			first.get<float>() = 1;

			type second;
			second.get<int>() = "first";
			second.get<float>() = 1;

			Assert::IsTrue(first == second);
			Assert::IsFalse(first != second);

			second.get<float>() = 2;
			Assert::IsTrue(first != second);
			Assert::IsFalse(first == second);
		}

		TEST_METHOD(TestEqualReversed)
		{
			atuple<int, std::string, float, long> first;
			first.get<int>() = "first";
			first.get<float>() = 1;

			atuple<float, long, int, std::string> second;
			second.get<int>() = "first";
			second.get<float>() = 1;

			Assert::IsTrue(first == second);
			Assert::IsFalse(first != second);
			second.get<float>() = 2;
			Assert::IsTrue(first != second);
			Assert::IsFalse(first == second);
		}

		TEST_METHOD(TestEqualOtherValueType)
		{
			atuple<int, int> t1;
			t1.get<int>() = 132;

			atuple<int, long> t2;
			t2.get<int>() = 132;

			Assert::IsTrue(t1 == t2);
			Assert::IsFalse(t1 != t2);
			
			t2.get<int>() = 456;
			Assert::IsFalse(t1 == t2);
			Assert::IsTrue(t1 != t2);
		}

		TEST_METHOD(TestEmptyEqual)
		{
			Assert::IsTrue(atuple<>() == atuple<>());
			Assert::IsFalse(atuple<>() != atuple<>());
		}

		TEST_METHOD(TestCopy)
		{
			using tuple_type = atuple<double, long, float, const char*>;

			tuple_type t1;
			t1.get<double>() = 123;
			t1.get<float>() = "val1";

			auto t2 = t1;

			Assert::IsTrue(t1 == t2);
			Assert::IsFalse(t1 != t2);
			Assert::AreEqual(t1.get<double>(), 123l);
			Assert::AreEqual(t1.get<float>(), "val1");

			auto t3 = t2;
			t3.get<double>() = 456;
			Assert::AreEqual(t3.get<double>(), 456l);
			t3 = t1;
			Assert::IsTrue(t1 == t3);
		}

		TEST_METHOD(TestCopyReversed)
		{
			using tuple_type = atuple<double, long, float, const char*>;
			using reversed_type = atuple<float, const char*, double, long>;

			tuple_type t1;
			t1.get<double>() = 123;
			t1.get<float>() = "val1";

			reversed_type t2 = t1;

			Assert::IsTrue(t1 == t2);
			Assert::IsFalse(t1 != t2);
			Assert::AreEqual(t1.get<double>(), 123l);
			Assert::AreEqual(t1.get<float>(), "val1");

			reversed_type t3 = t2;
			t3.get<double>() = 456;
			Assert::AreEqual(t3.get<double>(), 456l);
			t3 = t1;
			Assert::IsTrue(t1 == t3);
		}

		TEST_METHOD(TestCopyOtherValueType)
		{
			atuple<float, int> t1;
			t1.get<float>() = 123;

			atuple<float, long> t2 = t1;

			Assert::IsTrue(t1 == t2);
		}

		TEST_METHOD(TestMove)
		{
			using tuple_type = atuple<int, std::string, float, std::string>;

			tuple_type t1;
			std::string str = "string";
			t1.get<int>() = str;
			t1.get<float>() = str;

			auto t2 = std::move(t1);

			Assert::AreEqual(t2.get<int>(), str);
			Assert::AreEqual(t2.get<float>(), str);
			/* str is moved to t2 */
			Assert::AreNotEqual(t1.get<int>(), str);
			Assert::AreNotEqual(t1.get<float>(), str);

			t1 = std::move(t2);

			Assert::AreEqual(t1.get<int>(), str);
			Assert::AreEqual(t1.get<float>(), str);
			/* str is moved to t1 */
			Assert::AreNotEqual(t2.get<int>(), str);
			Assert::AreNotEqual(t2.get<float>(), str);
		}

		TEST_METHOD(TestMoveReversed)
		{
			using tuple_type = atuple<int, std::string, float, std::string>;
			using reversed_type = atuple<float, std::string, int, std::string>;

			tuple_type t1;
			std::string str = "string";
			t1.get<int>() = str;
			t1.get<float>() = str;

			reversed_type t2 = std::move(t1);

			Assert::AreEqual(t2.get<int>(), str);
			Assert::AreEqual(t2.get<float>(), str);
			/* str is moved to t2 */
			Assert::AreNotEqual(t1.get<int>(), str);
			Assert::AreNotEqual(t1.get<float>(), str);

			t1 = std::move(t2);

			Assert::AreEqual(t1.get<int>(), str);
			Assert::AreEqual(t1.get<float>(), str);
			/* str is moved to t1 */
			Assert::AreNotEqual(t2.get<int>(), str);
			Assert::AreNotEqual(t2.get<float>(), str);
		}
	};
}
