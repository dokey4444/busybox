/*
// =====================================================================================
// 
//       Filename:  test_util.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  10/23/2013 11:01:56 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "../elib_util.hpp"

static void case_1()
{                                                                                                                                                  
	log_printf("log_printf don't show in release\n");
	err_printf("log_printf don't show in release\n");
	sys_perror("log_printf don't show in release");

	puts("\n----------------  case 1: ok  ----------------\n");
}

void test_util()
{
	puts("********************************  test util started  ********************************\n");

	case_1();

	puts("################################  test util finished  ################################\n");
}

