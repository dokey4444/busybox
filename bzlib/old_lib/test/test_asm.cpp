/*
// =====================================================================================
// 
//       Filename:  test_asm.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  12/30/2013 06:16:10 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "../elib_util.hpp"
#include "../elib_asm.hpp"

static void case_1()
{
	assert(bsf(0x8000000000000008) == 3);
	assert(bsr(0x8000000000000008) == 8*sizeof(ulong)-1);

	puts("\n----------------  case 1: ok  ----------------\n");
}

void test_asm()
{
	puts("********************************  test asm started  ********************************\n");

	case_1();

	puts("################################  test asm finished  ################################\n");
}
