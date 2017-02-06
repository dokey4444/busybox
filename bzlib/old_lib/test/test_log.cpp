/*
// =====================================================================================
// 
//       Filename:  test_log.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  11/27/2013 07:58:38 AM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "../elib_util.hpp"
#include "../elib_log.hpp"

/* 反复执行该函数，即可测试日志类的各项功能 */
static void test_case_1()
{
	EL_Log logger;
	logger.init("/dev/shm/ceshi/log", "test", 4<<10, 8);
	for (int i=0; i<100; ++i) {
		logger.fatal("message %d", i);
		logger.error("message %d", i);
		logger.warn("message %d", i);
		logger.info("message %d", i);
		logger.debug("message %d", i);
		usleep(200*1000);
	}

	puts("test case 1: ok~");
}

void test_log()
{
	test_case_1();

	puts("test log is finished~");
}
