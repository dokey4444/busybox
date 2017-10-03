/*
// =====================================================================================
// 
//       Filename:  atomic.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  2017/10/03 13时27分04秒
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

#include <atomic>

/* 
// ===  FUNCTION  ======================================================================
//         Name:  main
//  Description:  
// =====================================================================================
*/
int main(int argc, char *argv[])
{
	// 原子变量
	// 原子变量的用法十分简单，但是接口众多，详细内容参考：
	// http://en.cppreference.com/w/cpp/atomic/atomic
	std::atomic<int> i(0);	// 原子变量的使用和普通int类型一样，基本没有差别
	std::atomic<int> j(1);
	i=1;		// 赋值操作
	++i; --i;	// 自增操作
	i+=2; i+=j;	// 累加操作

	struct A { int a[100]; };
	struct B { int x, y; };
	std::cout << std::boolalpha	// 将bool值显示为true or false
		<< "std::atomic<A> is lock free? "
		<< std::atomic<A>{}.is_lock_free() << '\n'
		<< "std::atomic<B> is lock free? "
		<< std::atomic<B>{}.is_lock_free() << '\n';

	std::cout << "Hello World" << std::endl;
	return EXIT_SUCCESS;
}				// ----------  end of function main  ----------

