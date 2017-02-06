/*
// =====================================================================================
// 
//       Filename:  test_atomic.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  11/17/2014 03:28:51 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "../elib_util.hpp"
#include "../elib_atomic.hpp"

/* 测试__GNUC__ == 4，使用__sync函数族的正确性 */
static void case_1()
{
	EL_Atomic atom;
	int n;
	bool b;
	atom = 4;
	assert(atom.value() == 4);
	n = atom += 4;
	assert(n == 8);
	assert(atom.value() == 8);
	n = atom -= 4;
	assert(n == 4);
	assert(atom.value() == 4);

	n = ++atom;
	assert(n == 5);
	assert(atom.value() == 5);
	n = atom++;
	assert(n == 5);
	assert(atom.value() == 6);

	n = --atom;
	assert(n == 5);
	assert(atom.value() == 5);
	n = atom--;
	assert(n == 5);
	assert(atom.value() == 4);

	atom = 0xf;
	n = atom &= 12;
	assert(n == 12);
	assert(atom.value() == 12);
	atom = 0xf;
	n = atom |= (uint)-1;
	assert(n == -1);
	assert(atom.value() == -1);
	atom = 0xf;
	n = atom ^= 2;
	assert(n == 13);
	assert(atom.value() == 13);

	atom = 0;
	atom.set(1);
	assert(atom.value() == 1);
	b = atom.cas_set(0, 2);
	assert(b == false);
	assert(atom.value() == 1);
	b = atom.cas_set(1, 2);
	assert(b == true);
	assert(atom.value() == 2);

	puts("\n----------------  case 1: ok  ----------------\n");
}

void test_atomic()
{
	puts("********************************  test atomic started  ********************************\n");

	case_1();

	puts("################################  test atomic finished  ################################\n");
}

