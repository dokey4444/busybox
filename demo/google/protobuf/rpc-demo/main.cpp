/*
// =====================================================================================
// 
//       Filename:  main.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  07/07/2016 11:29:06 AM
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

#include "rpc_sample.pb.h"
#include "rpc_server.h"
#include <google/protobuf/service.h>

using google::protobuf::RpcChannel;
using google::protobuf::RpcController;
using example::EchoService;

// =====================================================================================
//        Class:  EchoServiceImpl
//  Description:  
// =====================================================================================
class EchoServiceImpl : public EchoService
{
public:
	// ====================  LIFECYCLE     =======================================
	EchoServiceImpl() {}
	~EchoServiceImpl() {}

	// ====================  INTERFACE     =======================================
	virtual void Echo(::google::protobuf::RpcController* controller,
			const ::example::EchoRequest* request,
			::example::EchoResponse* response,
			::google::protobuf::Closure* done)
	{
		std::cout << "EchoServiceImpl::Echo()" << std::endl;
	}

private:
	// ==================== PRIVATE METHOD =======================================

	// ====================  DATA MEMBERS  =======================================

};		// -----  end of class EchoServiceImpl  -----

/* 
// ===  FUNCTION  ======================================================================
//         Name:  main
//  Description:  
// =====================================================================================
*/
int main(int argc, char *argv[])
{
	MyRPCServer server;
	EchoServiceImpl echo_service_impl;
	server.register_service(&echo_service_impl);
	server.start();

	MyRPCChannel my_rpc_channel(&server);
	MyRPCController my_rpc_controller;
	MyClosure my_closure;

	example::EchoService_Stub stub(&my_rpc_channel);
	example::EchoRequest request;
	example::EchoResponse response;
	request.set_message("Hello World");
	stub.Echo(&my_rpc_controller, &request, &response, &my_closure);

	server.stop();
	return EXIT_SUCCESS;
}				// ----------  end of function main  ----------

