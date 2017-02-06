/*
// =====================================================================================
// 
//       Filename:  test_stack.cpp
// 
//    Description:  测试顺序栈
// 
//        Version:  1.0
//        Created:  07/17/2013 09:18:19 AM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "elib_util.hpp"
#include "elib_stack.hpp"

static void test_case_1()
{
	EL_Stack_TS<int> stack;

	assert(stack.init(8) == 0);
	assert(stack.size() == 0);
	assert(stack.is_empty()); assert(!stack.is_full());

	stack.push(0); stack.push(1); stack.push(2); stack.push(3);
	assert(stack.size() == 4);
	assert(!stack.is_empty()); assert(!stack.is_full());
	stack.push(4); stack.push(5); stack.push(6); stack.push(7);
	assert(stack.size() == 8);
	assert(!stack.is_empty()); assert(stack.is_full());

	assert(stack.pop() == 7); assert(stack.pop() == 6);
	assert(stack.pop() == 5); assert(stack.pop() == 4);
	assert(stack.size() == 4);
	assert(!stack.is_empty()); assert(!stack.is_full());
	assert(stack.pop() == 3); assert(stack.pop() == 2);
	assert(stack.pop() == 1); assert(stack.pop() == 0);
	assert(stack.size() == 0);
	assert(stack.is_empty()); assert(!stack.is_full());

	assert(stack.destroy() == 0);

	puts("test case 1: ok~");
}

static void test_case_2()
{
	EL_Stack_TS<int> stack;

	assert(stack.init(8) == 0);
	assert(stack.size() == 0);
	assert(stack.is_empty()); assert(!stack.is_full());

	int node;
	assert(stack.try_push(0) == 0); assert(stack.try_push(1) == 0);
	assert(stack.try_push(2) == 0); assert(stack.try_push(3) == 0);
	assert(stack.size() == 4);
	assert(!stack.is_empty()); assert(!stack.is_full());
	assert(stack.try_push(4) == 0); assert(stack.try_push(5) == 0);
	assert(stack.try_push(6) == 0); assert(stack.try_push(7) == 0);
	assert(stack.size() == 8);
	assert(!stack.is_empty()); assert(stack.is_full());
	assert(stack.try_push(8) == -1);

	assert(stack.try_pop(&node) == 0); assert(node == 7);
	assert(stack.try_pop(&node) == 0); assert(node == 6);
	assert(stack.try_pop(&node) == 0); assert(node == 5);
	assert(stack.try_pop(&node) == 0); assert(node == 4);
	assert(stack.size() == 4);
	assert(!stack.is_empty()); assert(!stack.is_full());
	assert(stack.try_pop(&node) == 0); assert(node == 3);
	assert(stack.try_pop(&node) == 0); assert(node == 2);
	assert(stack.try_pop(&node) == 0); assert(node == 1);
	assert(stack.try_pop(&node) == 0); assert(node == 0);
	assert(stack.size() == 0);
	assert(stack.is_empty()); assert(!stack.is_full());
	assert(stack.try_pop(&node) == -1);

	// stack.destroy();	// check memory leak when no destroy

	puts("test case 2: ok~");
}

static void test_case_3()
{
	EL_Stack_TS<int> stack;

	assert(stack.init(8) == 0);
	assert(stack.size() == 0);
	assert(stack.is_empty()); assert(!stack.is_full());

	assert(stack.try_push(0) == 0); assert(stack.try_push(1) == 0);
	assert(stack.try_push(2) == 0); assert(stack.try_push(3) == 0);
	assert(stack.size() == 4);
	stack.reset();
	assert(stack.size() == 0); assert(stack.is_empty());
	assert(stack.try_push(0) == 0); assert(stack.try_push(1) == 0);
	assert(stack.try_push(2) == 0); assert(stack.try_push(3) == 0);
	assert(stack.size() == 4);
	assert(!stack.is_empty()); assert(!stack.is_full());

	assert(stack.destroy() == 0);

	puts("test case 3: ok~");
}

/* 测试非线程安全版本，接口功能与线程安全版本完全一致（没有try接口） */
static void test_case_4()
{
	EL_Stack<int> stack;

	assert(stack.init(8) == 0);
	assert(stack.size() == 0);
	assert(stack.is_empty()); assert(!stack.is_full());

	stack.push(0); stack.push(1); stack.push(2); stack.push(3);
	assert(stack.size() == 4);
	assert(!stack.is_empty()); assert(!stack.is_full());
	stack.push(4); stack.push(5); stack.push(6); stack.push(7);
	assert(stack.size() == 8);
	assert(!stack.is_empty()); assert(stack.is_full());

	assert(stack.pop() == 7); assert(stack.pop() == 6);
	assert(stack.pop() == 5); assert(stack.pop() == 4);
	assert(stack.size() == 4);
	assert(!stack.is_empty()); assert(!stack.is_full());
	assert(stack.pop() == 3); assert(stack.pop() == 2);
	assert(stack.pop() == 1); assert(stack.pop() == 0);
	assert(stack.size() == 0);
	assert(stack.is_empty()); assert(!stack.is_full());

	assert(stack.destroy() == 0);

	puts("test case 4: ok~");
}

void test_stack()
{
	test_case_1();
	test_case_2();
	test_case_3();

	test_case_4();

	puts("test stack is finished~");
}
