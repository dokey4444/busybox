/*
// =====================================================================================
// 
//       Filename:  test_communication_tcp.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  01/26/2014 01:51:14 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "../elib_util.hpp"
#include "../elib_communication_tcp.hpp"

class session_server : public EL_Comm_TCP {
	void session(int socket)
	{
		printf("socket: %d\n", socket);
	}
};

/* 服务端，调试用代码，跟程序用 */
static void test_case_1()
{
	session_server comm;
	comm.init();
	comm.start(65535, 65534);

	puts("test case 1: ok~");
}

/* 客户端，调试用代码，跟程序用 */
static void test_case_2()
{
	session_server comm;
	comm.init();
	int socket = comm.connect("127.0.0.1", 65534);
	comm.send(socket, "test", 4);
	comm.disconnect(socket);

	puts("test case 2: ok~");
}

void test_communication_tcp()
{
	test_case_1();
	test_case_2();

	puts("test communication tcp is finished~");
}
