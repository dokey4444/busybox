/*
// =====================================================================================
// 
//       Filename:  elib_communication_tcp.hpp
// 
//    Description:  TCP服务模型
// 
//        Version:  1.0
//        Created:  01/26/2014 10:36:25 AM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#ifndef  _ELIB_COMMUNICATION_TCP_H_
#define  _ELIB_COMMUNICATION_TCP_H_

#include "elib_queue.hpp"

// =====================================================================================
//        Class:  EL_Comm_TCP
//  Description:  
// =====================================================================================
class EL_Comm_TCP
{
public:
	// ====================  LIFECYCLE     =======================================
	EL_Comm_TCP();
	~EL_Comm_TCP();

	// ====================  INTERFACE     =======================================

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  init
	//  Description:  初始化TCP服务
	//   Parameters:  port: 服务端（绑定端口）。客户端（服务端端口）
	//                ip: 服务端：NULL。客户端（服务端IP）
	//                worker_thread_count: 工作线程数
	//  ReturnValue:  成功返回0，失败返回-1
	// =====================================================================================
	*/
	virtual int init(int worker_thread_count = 1);
	
	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  destroy
	//  Description:  销毁对象
	// =====================================================================================
	*/
	void destroy();
	
	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  start
	//  Description:  启动服务，成功返回0，失败返回-1
	// =====================================================================================
	*/
	int start(ushort port, ushort monitor_port, time_t timeout_sec = -1);

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  session
	//  Description:  建立一个会话（用于子类的继承和重写）
	//   Parameters:  会话连接的文件描述符
	//  ReturnValue:  NULL
	// =====================================================================================
	*/
	virtual void session(int socket) = 0;

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  connect
	//  Description:  连接到指定服务端，成功返回sockfd，失败返回-1
	// =====================================================================================
	*/
	int connect(const char *ip, ushort port, time_t timeout_sec = -1);
	
	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  disconnect
	//  Description:  断开客户端连接
	// =====================================================================================
	*/
	void disconnect(int socket);

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  send
	//  Description:  发送数据
	//   Parameters:  与系统调用保持一致
	//  ReturnValue:  与系统调用保持一致
	// =====================================================================================
	*/
	ssize_t send(int socket, const void *buf, size_t len, int flags = 0);

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  recv
	//  Description:  接收数据
	//   Parameters:  与系统调用保持一致
	//  ReturnValue:  与系统调用保持一致
	// =====================================================================================
	*/
	ssize_t recv(int socket, void *buf, size_t len, int flags = 0);

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  monitor
	//  Description:  根据线程状态，发送对应的监控协议
	//   Parameters:  线程状态
	//  ReturnValue:  成功返回0，失败返回-1
	// =====================================================================================
	*/
	int monitor(int socket, int statue);

private:
	// ==================== PRIVATE METHOD =======================================

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  create_session
	//  Description:  根据工作线程数建立工作线程
	// =====================================================================================
	*/
	static void* create_session(void *arg);
	
	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  create_monitor
	//  Description:  创建监控线程
	// =====================================================================================
	*/
	static void* create_monitor(void *arg);
	
	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  thread_status
	//  Description:  返回工作线程状态
	// =====================================================================================
	*/
	int thread_status();

	// ====================  DATA MEMBERS  =======================================
	const static int listenq = 7;
	ushort worker_port;
	ushort monitor_port;

	int worker_thread_count;
	int counter;	// for monitor

	pthread_t *tids;
	pthread_t monitor_tid;
	time_t *tids_status;

	EL_Queue_TS<int> connq;

	enum ErrorType {                                                   
		TYPE_OK = 0,	// 运行正常
		TYPE_MONITOR,	// monitor错误
		TYPE_NETWORK,	// 网络故障
		TYPE_SERVICE,	// 服务错误
		TYPE_OTHER,	// 其它错误
	};  

	enum ErrorLevel {
		LEVEL_A = 1,	// 严重
		LEVEL_B,	// 重要
		LEVEL_C,	// 一般
		LEVEL_D,	// 调试
		LEVEL_E		// 可忽略
	};

};		// -----  end of class EL_Comm_TCP  -----

#endif   // ----- #ifndef _ELIB_COMMUNICATION_TCP_H_  -----

