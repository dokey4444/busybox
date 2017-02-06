/*
 * =====================================================================================
 *
 *       Filename:  rpc_server.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/07/2016 11:23:10 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <string>
#include <map>
#include <google/protobuf/service.h>

/*
 * =====================================================================================
 *        Class:  MyRPCController
 *  Description:  
 * =====================================================================================
 */
class MyRPCController : public google::protobuf::RpcController
{
public:
	/* ====================  LIFECYCLE     ======================================= */
	MyRPCController() {}
	~MyRPCController() {}

	/* ====================  INTERFACE     ======================================= */

	/* ====================  Client-side methods  ================================ */

	/* 
	 * ===  FUNCTION  ======================================================================
	 *         Name:  Reset
	 *  Description:  Resets the RpcController to its initial state so that it may be reused in a new call.
	 *                Must not be called while an RPC is in progress.
	 * =====================================================================================
	 */
	void Reset()
	{
	}		/* -----  end of function Reset  ----- */

	/* 
	 * ===  FUNCTION  ======================================================================
	 *         Name:  Failed
	 *  Description:  After a call has finished, returns true if the call failed.
	 *                The possible reasons for failure depend on the RPC implementation.
	 *                Failed() must not be called before a call has finished.
	 *                If Failed() returns true, the contents of the response message are undefined.
	 * =====================================================================================
	 */
	bool Failed() const
	{
		return _failed_flag;
	}		/* -----  end of function Failed  ----- */

	/* 
	 * ===  FUNCTION  ======================================================================
	 *         Name:  ErrorText
	 *  Description:  If Failed() is true, returns a human-readable description of the error.
	 * =====================================================================================
	 */
	std::string ErrorText() const
	{
		return _error_reasion;
	}		/* -----  end of function ErrorText  ----- */

	/* 
	 * ===  FUNCTION  ======================================================================
	 *         Name:  StartCancel
	 *  Description:  Advises the RPC system that the caller desires that the RPC call be canceled.
	 *                The RPC system may cancel it immediately, may wait awhile and then cancel it,
	 *                or may not even cancel the call at all. If the call is canceled,
	 *                the "done" callback will still be called and the RpcController will indicate
	 *                that the call failed at that time.
	 * =====================================================================================
	 */
	void StartCancel()
	{
		_cancel_flag = true;
	}		/* -----  end of function StartCancel  ----- */

	/* ====================  Server-side methods  ================================ */

	/* 
	 * ===  FUNCTION  ======================================================================
	 *         Name:  SetFailed
	 *  Description:  Causes Failed() to return true on the client side.
	 *                "reason" will be incorporated into the message returned by ErrorText().
	 *                If you find you need to return machine-readable information about failures,
	 *                you should incorporate it into your response protocol buffer and should NOT call SetFailed().
	 * =====================================================================================
	 */
	void SetFailed(const std::string& reason)
	{
		_failed_flag = true;
	}		/* -----  end of function SetFailed  ----- */

	/* 
	 * ===  FUNCTION  ======================================================================
	 *         Name:  IsCanceled
	 *  Description:  If true, indicates that the client canceled the RPC, so the server may as well give up on replying to it.
	 *                The server should still call the final "done" callback.
	 * =====================================================================================
	 */
	bool IsCanceled() const
	{
		return _cancel_flag;
	}		/* -----  end of function IsCanceled  ----- */

	/* 
	 * ===  FUNCTION  ======================================================================
	 *         Name:  NotifyOnCancel
	 *  Description:  Asks that the given callback be called when the RPC is canceled.
	 *                The callback will always be called exactly once.
	 *                If the RPC completes without being canceled,
	 *                the callback will be called after completion.
	 *                If the RPC has already been canceled when NotifyOnCancel() is called,
	 *                the callback will be called immediately.
	 * =====================================================================================
	 */
	void NotifyOnCancel(google::protobuf::Closure* callback)
	{
	}		/* -----  end of function NotifyOnCancel  ----- */

private:
	/* ==================== PRIVATE METHOD ======================================= */

	/* ====================  DATA MEMBERS  ======================================= */
	bool _failed_flag = false;
	std::string _error_reasion;
	
	bool _cancel_flag = false;
};		/* -----  end of class MyRPCController  ----- */

/*
 * =====================================================================================
 *        Class:  MyRPCServer
 *  Description:  
 * =====================================================================================
 */
class MyRPCServer
{
public:
	/* ====================  LIFECYCLE     ======================================= */
	MyRPCServer() {}
	~MyRPCServer() {}

	/* ====================  INTERFACE     ======================================= */

	/* 
	 * ===  FUNCTION  ======================================================================
	 *         Name:  start
	 *  Description:  启动server
	 * =====================================================================================
	 */
	int start()
	{
		return 0;
	}		/* -----  end of function start  ----- */

	/* 
	 * ===  FUNCTION  ======================================================================
	 *         Name:  stop
	 *  Description:  停止server
	 * =====================================================================================
	 */
	int stop()
	{
		return 0;
	}		/* -----  end of function stop  ----- */

	/* 
	 * ===  FUNCTION  ======================================================================
	 *         Name:  register_service
	 *  Description:  注册service
	 * =====================================================================================
	 */
	void register_service(google::protobuf::Service *service)
	{
		const google::protobuf::ServiceDescriptor *psd = service->GetDescriptor();
		for (int i = 0; i < psd->method_count(); ++i) {
			const google::protobuf::MethodDescriptor *pmd = psd->method(i);
			int rpc_index = make_rpc_index(psd->index(), pmd->index());
			rpc_map[rpc_index] = std::make_pair(service, pmd);
		}
		
	}		/* -----  end of function register_service  ----- */
	
	/* 
	 * ===  FUNCTION  ======================================================================
	 *         Name:  call
	 *  Description:  模拟网络调用
	 * =====================================================================================
	 */
	void call(int service_index, int method_index,
			google::protobuf::RpcController* controller,
			const google::protobuf::Message* request,
			google::protobuf::Message* response,
			google::protobuf::Closure* done)
	{
		int rpc_index = make_rpc_index(service_index, method_index);
		google::protobuf::Service *service = rpc_map[rpc_index].first;
		const google::protobuf::MethodDescriptor *pmd = rpc_map[rpc_index].second;
		service->CallMethod(pmd, controller, request, response, done);
	}		/* -----  end of function call  ----- */

private:
	/* ==================== PRIVATE METHOD ======================================= */
	
	/* 
	 * ===  FUNCTION  ======================================================================
	 *         Name:  make_rpc_index
	 *  Description:   
	 * =====================================================================================
	 */
	int make_rpc_index(int service_index, int method_index)
	{
		return (service_index << 16) + method_index;
	}		/* -----  end of function make_rpc_index  ----- */

	/* ====================  DATA MEMBERS  ======================================= */
	std::map< int,
		std::pair<google::protobuf::Service*,
		const google::protobuf::MethodDescriptor*> > rpc_map;
};		/* -----  end of class MyRPCServer  ----- */

/*
 * =====================================================================================
 *        Class:  MyRPCChannel
 *  Description:  
 * =====================================================================================
 */
class MyRPCChannel : public google::protobuf::RpcChannel
{
public:
	/* ====================  LIFECYCLE     ======================================= */
	MyRPCChannel(MyRPCServer *server) : _server(server) {}
	~MyRPCChannel() {}

	/* ====================  INTERFACE     ======================================= */

	/* 
	 * ===  FUNCTION  ======================================================================
	 *         Name:  CallMethod
	 *  Description:   
	 * =====================================================================================
	 */
	void CallMethod(const google::protobuf::MethodDescriptor* method,
			google::protobuf::RpcController* controller,
			const google::protobuf::Message* request,
			google::protobuf::Message* response,
			google::protobuf::Closure* done)
	{
		// 这里实现数据传输的具体内容
		_server->call(method->service()->index(), method->index(),
				controller, request, response, NULL);

		// done可以作为程序执行后的清理，也可以作为异步回调
		if (done != NULL) {
			done->Run();
		}
	}		/* -----  end of function CallMethod  ----- */
private:
	/* ==================== PRIVATE METHOD ======================================= */

	/* ====================  DATA MEMBERS  ======================================= */
	MyRPCServer *_server;

};		/* -----  end of class MyRPCChannel  ----- */

/*
 * =====================================================================================
 *        Class:  MyClosure
 *  Description:  
 * =====================================================================================
 */
class MyClosure : public google::protobuf::Closure
{
public:
	// ====================  LIFECYCLE     =======================================
	MyClosure() {}
	~MyClosure() {}

	// ====================  INTERFACE     =======================================

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  Run
	//  Description:   
	//   Parameters:  
	//  ReturnValue:  
	// =====================================================================================
	*/
	void Run()
	{
		std::cout << "Closure::Run()" << std::endl;
	}		// -----  end of function Run  -----

private:
	// ==================== PRIVATE METHOD =======================================

	// ====================  DATA MEMBERS  =======================================

};		// -----  end of class MyClosure  -----

