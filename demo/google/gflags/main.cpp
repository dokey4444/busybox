/*
// =====================================================================================
// 
//       Filename:  main.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  08/23/2015 03:39:01 PM
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
#include <string>

#include <stdio.h>
#include <gflags/gflags.h>

// DEFINE_类型(变量名, 默认值, 说明)
DEFINE_bool(arg_bool, true, "arg_bool comment");
DEFINE_int32(arg_int32, 32, "arg_int32 comment");
DEFINE_int64(arg_int64, 64, "arg_int64 comment");
DEFINE_uint64(arg_uint64, 0, "arg_uint64 comment");
DEFINE_double(arg_double, 3.1415926, "arg_double comment");
DEFINE_string(arg_string, std::string("cpp_string"), "arg_string comment");

static bool check_int32(const char* flagname, int32_t value)
{
	if (value > 0 && value < 100)
		return true;
	printf("Invalid value for --%s: %d\n", flagname, (int)value);
	return false;
}

/* 
// ===  FUNCTION  ======================================================================
//         Name:  main
//  Description:  
// =====================================================================================
*/
int main(int argc, char *argv[])
{
	google::RegisterFlagValidator(&FLAGS_arg_int32, &check_int32);

	google::SetUsageMessage("Usage: ");
	gflags::SetVersionString("1.0.0.0");

	// 第三个参数为remove_flags，表示是否将命令行参数从argv中删除，并减小argc
	// 如果为false，argv并不会删除，但是会重排序
	google::ParseCommandLineFlags(&argc, &argv, true);

	std::cout << "argc=" << argc << std::endl;

	FLAGS_arg_bool = false;	// 直接改全局变量也是可以的
	std::cout << "bool=" << FLAGS_arg_bool << std::endl;
	std::cout << "int32=" << FLAGS_arg_int32 << std::endl;
	std::cout << "int64=" << FLAGS_arg_int64 << std::endl;
	std::cout << "uint64=" << FLAGS_arg_uint64 << std::endl;
	std::cout << "double=" << FLAGS_arg_double << std::endl;
	std::cout << "string=" << FLAGS_arg_string << std::endl;

	return EXIT_SUCCESS;
}				// ----------  end of function main  ----------

