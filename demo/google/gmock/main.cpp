/*
// =====================================================================================
// 
//       Filename:  main.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  08/23/2015 06:56:06 PM
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
#include "gmock/gmock.h"

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::InSequence;
using ::testing::AnyNumber;
using ::testing::Ge;
using ::testing::_;

class ClassDemo
{
public:
	int function0() { std::cout << "function0" << std::endl; return 0; }
	int function1(int x) { std::cout << "function1" << std::endl; return x; }
	int function2(int x, int y) { std::cout << "function2" << std::endl; return x + y; }
};

class MockClassDemo : public ClassDemo
{
public:
	// MOCK_METHOD*(fun_name, ret_type(args list))
	MOCK_METHOD0(function0, int());
	MOCK_METHOD1(function1, int(int x));
	MOCK_METHOD2(function2, int(int x, int y));
};

TEST(mock_test, test1)
{
	MockClassDemo demo;
	// EXPECT_CALL(obj_name, fun_name(args list))
	// 预期demo的function0()函数至少被调用1次
	// EXPECT_CALL表示对未来的期望，决不能在函数调用过程中或之后指定EXPECT_CALL，那样是未定义的
	EXPECT_CALL(demo, function0()).Times(AtLeast(1));

	demo.function0();
}

TEST(mock_test, test2)
{
	MockClassDemo demo;

	// 预期demo的function0函数一共指定5次，第一次返回100，第二次返回150，其余返回200
	// 如果有明确的计数，那么Mock的期望是有限次的，所有的期望耗尽则不会再有任何期望规则
	EXPECT_CALL(demo, function0())
		.Times(5)
		.WillOnce(Return(100))
		.WillOnce(Return(150))
		.WillRepeatedly(Return(200));

	std::cout << "demo.function0() ret = " << demo.function0() << std::endl;
	std::cout << "demo.function0() ret = " << demo.function0() << std::endl;
	std::cout << "demo.function0() ret = " << demo.function0() << std::endl;
	std::cout << "demo.function0() ret = " << demo.function0() << std::endl;
	std::cout << "demo.function0() ret = " << demo.function0() << std::endl;
}

TEST(mock_test, test3)
{
	MockClassDemo demo;

	// ‘_’表示任意参数
	EXPECT_CALL(demo, function1(_));
	demo.function1(9);

	// Ge = greater or equal
	// 还有更多的方法，详见doc中的CheatSheet
	EXPECT_CALL(demo, function1(Ge(10)));
	demo.function1(10);
}

TEST(mock_test, test4)
{
	MockClassDemo demo;

	// 有冲突的老规则和新规则可以理解为穿透式压栈式覆盖
	// 如果第二次也给参数10则会出现warning，因为参数为10的期望已经饱和，Mock会认为无法匹配
	// 如果换成10以外的数，就不会出现warning了（感觉有点奇怪，“_”不是可以替代一切么？）
	EXPECT_CALL(demo, function1(_));
	EXPECT_CALL(demo, function1(10));
	demo.function1(10);
	demo.function1(12);

	// 期望参数会根据穿透式压栈的顺序，一直像栈底搜索，直到找到满足匹配的期望为止
	EXPECT_CALL(demo, function1(_));
	EXPECT_CALL(demo, function1(10)).Times(2);
	demo.function1(10);
	demo.function1(12);
	demo.function1(10);
}

TEST(mock_test, test5)
{
	MockClassDemo demo;

	// 这个例子表示在所有调用中，预期有且只有2次参数为0
	EXPECT_CALL(demo, function1(_)).Times(AnyNumber());
	EXPECT_CALL(demo, function1(10)).Times(2);

	demo.function1(11);
	demo.function1(10);
	demo.function1(12);
	demo.function1(13);
	demo.function1(10);
	demo.function1(14);
	demo.function1(15);
}

TEST(mock_test, test6)
{
	MockClassDemo demo;

	// 使用RetiresOnSaturation可以使饱和的（用过的）期望自动失效（退栈）
	// 如果不加RetiresOnSaturation，Mock会因为最后一次的预期饱和而在后2次执行时报wraning
	EXPECT_CALL(demo, function0()).WillOnce(Return(10)).RetiresOnSaturation();
	EXPECT_CALL(demo, function0()).WillOnce(Return(20)).RetiresOnSaturation();
	EXPECT_CALL(demo, function0()).WillOnce(Return(30)).RetiresOnSaturation();

	std::cout << "demo.function0() ret = " << demo.function0() << std::endl;
	std::cout << "demo.function0() ret = " << demo.function0() << std::endl;
	std::cout << "demo.function0() ret = " << demo.function0() << std::endl;

}

TEST(mock_test, test7)
{
	MockClassDemo demo;

	// 定义一个InSequence对象即可从“栈顺序”变为“队列顺序”
	// 就定义一个变量就行了？太神奇了～
	InSequence s;

	// 使用RetiresOnSaturation可以使饱和的（用过的）期望自动失效（退栈）
	// 如果不加RetiresOnSaturation，Mock会因为最后一次的预期饱和而在后2次执行时报wraning
	EXPECT_CALL(demo, function0()).WillOnce(Return(10)).RetiresOnSaturation();
	EXPECT_CALL(demo, function0()).WillOnce(Return(20)).RetiresOnSaturation();
	EXPECT_CALL(demo, function0()).WillOnce(Return(30)).RetiresOnSaturation();

	std::cout << "demo.function0() ret = " << demo.function0() << std::endl;
	std::cout << "demo.function0() ret = " << demo.function0() << std::endl;
	std::cout << "demo.function0() ret = " << demo.function0() << std::endl;

}

/* 
// ===  FUNCTION  ======================================================================
//         Name:  main
//  Description:  
// =====================================================================================
*/
int main(int argc, char *argv[])
{
	::testing::InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();
}				// ----------  end of function main  ----------

