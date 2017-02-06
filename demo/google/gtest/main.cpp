/*
// =====================================================================================
// 
//       Filename:  main.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  08/23/2015 10:50:48 AM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include <cstdlib>
#include <iostream>

#include "gtest/gtest.h"

class TestClass : public ::testing::Test 
{
public:
	TestClass() : a(-1), b(0), c(0), d(1),
	str1("abc"), str2("abc"), str3("ddd"), str4("aBc") {}
// protected or public, anyone is ok
protected:
	virtual void SetUp()
	{
		std::cout << "TestClass::SetUp()" << std::endl;
	}

	virtual void TearDown()
	{
		std::cout << "TestClass::TearDown()" << std::endl;
	}

	int a; int b; int c; int d;
	std::string str1; std::string str2;
	std::string str3; std::string str4;
};

TEST_F(TestClass, test_name_in_class)
{
	EXPECT_TRUE(true) << "not true ???";
	EXPECT_FALSE(false);
	EXPECT_EQ(b, c);
	EXPECT_NE(a, b);
	EXPECT_LT(a, b);
	EXPECT_LE(a, a);
	EXPECT_GT(d, c);
	EXPECT_GE(d, d);

	EXPECT_EQ(str1, str2);
	EXPECT_EQ(str1, str4);
	EXPECT_STREQ(str1.c_str(), str2.c_str());
	EXPECT_STRNE(str1.c_str(), str3.c_str());
	EXPECT_STRCASEEQ(str1.c_str(), str4.c_str());
	EXPECT_STRCASENE(str3.c_str(), str4.c_str());
}

TEST(test_case_name, test_name)
{
	EXPECT_TRUE(true);
	EXPECT_TRUE(false);
}

TEST(test_case_name, test_name_adv1)
{
	// Explicit Success and Failure
	SUCCEED() << "Do nothing.";
	ADD_FAILURE()<< "ADD_FAILURE to last, you need it when you check a control flow";
	ADD_FAILURE_AT(__FILE__, __LINE__) << "ADD_FAILURE_AT(" << __FILE__ << ", " << __LINE__ << ")";
	ADD_FAILURE_AT("non-exist-file", 9999) << "if you want to locate to a non-exist location, it's ok";
	FAIL() << "We shouldn't get here.";
}

TEST(test_case_name, test_name_adv2)
{
	// Exception Assertions
	EXPECT_THROW(throw 1, int);
	EXPECT_ANY_THROW({ int a = 1;
			if (a) throw "string"; });
	EXPECT_NO_THROW(strcmp("a", "B"));
}

template <typename T>
bool equal(T x, T y) { return x == y; }
::testing::AssertionResult IsEven(int n)
{
	// 这种方法属于嵌入式的测试，改变函数的返回值来融入gtest
	if ((n % 2) == 0)
		return ::testing::AssertionSuccess() << n << " is even";
	else
		return ::testing::AssertionFailure() << n << " is odd";
}
template <typename T>
::testing::AssertionResult format_equal(const char* m_expr, const char* n_expr, T m, T n)
{
	if (equal<T>(m, n))
		return ::testing::AssertionSuccess();

	return ::testing::AssertionFailure()
		<< m_expr << " and " << n_expr << " (" << m << " and " << n
		<< ") are not equal.";
}
TEST(test_case_name, test_name_adv3)
{
	// Predicate Assertions for Better Error Messages
	int a = 1, b = 2;
	EXPECT_PRED2(equal<int>, a, b);	// EXPECT_PRED后面的数字表示函数参数的个数
	EXPECT_PRED1(IsEven, 3);	// 使用EXPECT_PRED*并不会打印原函数中的输出
	EXPECT_FALSE(IsEven(IsEven(3)));// 使用非EXPECT_PRED*才会打印原函数中的输出
	EXPECT_PRED_FORMAT2(format_equal<int>, a, b);	// 格式化输出错误信息
}

TEST(test_case_name, test_name_adv4)
{
	// Floating-Point Comparison
	float fx = 3.1415926, fy = 3.1415927, fn = 0.0000001;
	double dx = 3.1415926, dy = 3.1415927, dn = 0.0000001;
	EXPECT_FLOAT_EQ(fx, fy);	// is equal
	EXPECT_DOUBLE_EQ(dx, dy);
	EXPECT_NEAR(fx, fy, fn);	// Failure ???? why ????
	EXPECT_NEAR(dx, dy, dn);
}

TEST(test_case_name, test_name_adv5)
{
	// Type Assertions
	::testing::StaticAssertTypeEq<int, int>();	// 类型不一致会导致编译错误，常用于模板编程
}

void assert_fun()
{
	// `ASSERT_*()` can only be used in `void` functions.

	int a = -1, b = 0, c = 0, d = 1;

	// 所有的EXPECT_*语句都有一个与之对应的ASSERT_*版本
	ASSERT_TRUE(true) << "not true ???";
	ASSERT_FALSE(false);
	ASSERT_EQ(b, c);
	ASSERT_NE(a, b);
	ASSERT_LT(a, b);
	ASSERT_LE(a, a);
	ASSERT_GT(d, c);
	ASSERT_GE(d, d);

	char str1[] = "abc";
	char str2[] = "abc";
	char str3[] = "ddd";
	char str4[] = "aBc";
	ASSERT_STREQ(str1, str2);
	ASSERT_STRNE(str1, str3);
	ASSERT_STRCASEEQ(str1, str4);
	ASSERT_STRCASENE(str3, str4);
}

/* 
// ===  FUNCTION  ======================================================================
//         Name:  main
//  Description:  
// =====================================================================================
*/
int main(int argc, char *argv[])
{
	int a = -1, b = 0, c = 0, d = 1;

	// 所有的测试后面都可以DIY输出，也可以不输出
	EXPECT_TRUE(true) << "not true ???";
	EXPECT_FALSE(false);
	EXPECT_EQ(b, c);
	EXPECT_NE(a, b);
	EXPECT_LT(a, b);
	EXPECT_LE(a, a);
	EXPECT_GT(d, c);
	EXPECT_GE(d, d);

	char str1[] = "abc";
	char str2[] = "abc";
	char str3[] = "ddd";
	char str4[] = "aBc";
	EXPECT_STREQ(str1, str2);
	EXPECT_STRNE(str1, str3);
	EXPECT_STRCASEEQ(str1, str4);
	EXPECT_STRCASENE(str3, str4);

	assert_fun();

	::testing::InitGoogleTest(&argc, argv);
	int ret = RUN_ALL_TESTS();
	std::cout << "RUN_ALL_TESTS ret = " << ret << std::endl;

	return EXIT_SUCCESS;
}				// ----------  end of function main  ----------

