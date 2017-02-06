/*
// =====================================================================================
// 
//       Filename:  bind.cpp
// 
//    Description:  bind的用法
// 
//        Version:  1.0
//        Created:  01/04/2016 04:41:31 PM
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
#include <functional>

void call(int x, const std::function<void(int)> &fun)
{
	fun(x);
}

void output_odd(int x)
{
	if (x & 1) {
		std::cout << x << " ";
	}
}

void output_even(int x)
{
	if (! (x & 1)) {
		std::cout << x << " ";
	}
}

/* 
// ===  FUNCTION  ======================================================================
//         Name:  main
//  Description:  
// =====================================================================================
*/
int main(int argc, char *argv[])
{
	{
		// bind可以绑定所有参数，也可以绑定部分参数
		auto fun = std::bind(output_odd, std::placeholders::_1);
		for (int i=0; i<10; ++i) {
			call(i, fun);
		}
		std::cout << std::endl;
	}

	{
		auto fun = std::bind(output_even, std::placeholders::_1);
		for (int i=0; i<10; ++i) {
			call(i, fun);
		}
		std::cout << std::endl;
	}
	return EXIT_SUCCESS;
}				// ----------  end of function main  ----------

