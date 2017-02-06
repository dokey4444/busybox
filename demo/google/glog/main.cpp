/*
// =====================================================================================
// 
//       Filename:  main.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  08/22/2015 11:26:14 PM
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
#include <iomanip>

#include <glog/logging.h>

/* 
// ===  FUNCTION  ======================================================================
//         Name:  main
//  Description:  
// =====================================================================================
*/
int main(int argc, char *argv[])
{
	google::InitGoogleLogging(argv[0]);

	// 设置环境变量
	// 文档中说明有三种方法都可以指定环境变量
	// 1. 在执行bin程序之前，在shell中指定(export GLOG_xxxx)
	// 2. 联编gflag，然后在命令行参数中指定(--xxxx)
	// 3. 在代码中，直接修改全局变量指定(FLAGS_xxxx=????)
	FLAGS_log_dir = "./log";
	FLAGS_logtostderr = true;

	// 每个宏后面，都可以跟输出，可可以不跟输出
	DLOG(INFO) << "DLOG(INFO)";
	LOG(INFO) << "LOG(INFO)";

	DLOG(WARNING) << "DLOG(WARNING)";
	LOG(WARNING) << "LOG(WARNING)";

	DLOG(ERROR) << "DLOG(ERROR)";
	LOG(ERROR) << "LOG(ERROR)";

//	DLOG(DFATAL) << "DLOG(DFATAL)";	// this process will be core
//	LOG(FATAL) << "LOG(FATAL)";	// this process will be core

	std::cout << std::endl;

	int num = 4;
	DLOG_IF(INFO, num == 0) << "num == 0";
	LOG_IF(INFO, num == 0) << "num == 0";
	DLOG_IF(INFO, num == 4) << "num == 4";
	LOG_IF(INFO, num == 4) << "num == 4";

	std::cout << std::endl;

	for (int i=0; i<10; ++i) {
		DLOG_EVERY_N(INFO, 3) << "i = " << i;
		LOG_EVERY_N(INFO, 3) << "i = " << i;
	}

	std::cout << std::endl;

	for (int i=0; i<10; ++i) {
		DLOG_IF_EVERY_N(INFO, i > 5, 3) << "i = " << i;
		LOG_IF_EVERY_N(INFO, i > 5, 3) << "i = " << i;
	}

	std::cout << std::endl;

	for (int i=0; i<10; ++i) {
		LOG_FIRST_N(INFO, 3) << "i = " << i;
	}

	std::cout << std::endl;

	// CHECK系列函数约等于assert宏，一旦验证失败，直接core
	int *p = &num;
	int *np = NULL;
	CHECK_EQ(np, static_cast<int*>(NULL));
	CHECK_NOTNULL(p);

	DCHECK(num == *p) << "check: num == 4";
	CHECK(num == *p) << "check: num == 4";
	DCHECK_EQ(num, *p) << "check: num == 4";
	CHECK_EQ(num, *p) << "check: num == 4";
	DCHECK_NE(num, 0) << "check: num != 0";
	CHECK_NE(num, 0) << "check: num != 0";

	// C字符串不能直接用CHECK_EQ比
	// 非字符串判断宏，C字符串最终比的是指针地址
	char str1[] = "abc";
	char str2[] = "abc";
	char str3[] = "aBc";
	char str4[] = "ddd";
	CHECK_STREQ(str1, str2);
	CHECK_STRNE(str1, str4);
	CHECK_STRCASEEQ(str1, str3);
	CHECK_STRCASENE(str3, str4);

	double d1 = 3.1415926;
	double d2 = 3.1415927;
	double dx = 0.0000001;
//	CHECK_DOUBLE_EQ(d1, d2);	// this process will be core
	CHECK_NEAR(d1, d2, dx);

	// VLOG可以在INFO级别输出细节日志
	FLAGS_v = 2;
	VLOG(1) << "VLOG(1)";
	VLOG(2) << "VLOG(2)";
	VLOG(3) << "VLOG(3)";
	VLOG(4) << "VLOG(4)";

	// google风格的perror
	write(1, NULL, 2);
//	PLOG(FATAL) << "PLOG";	// this process will be core
	write(1, NULL, 2);
//	PLOG_IF(FATAL, errno != 0) << "PLOG_IF";	// this process will be core
//	PCHECK(write(1, NULL, 2) >= 0) << "Write NULL failed";	// this process will be core

	return EXIT_SUCCESS;
}				// ----------  end of function main  ----------
