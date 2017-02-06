/*
// =====================================================================================
// 
//       Filename:  elib_communication_tcp.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  01/26/2014 10:37:18 AM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "elib_util.hpp"
#include "elib_communication_tcp.hpp"

EL_Comm_TCP::EL_Comm_TCP()
{
}

EL_Comm_TCP::~EL_Comm_TCP()
{
	destroy();
}

// ====================  INTERFACE     =======================================

int EL_Comm_TCP::init(int worker_thread_count)
{
	this->worker_thread_count = worker_thread_count;

	tids = (pthread_t*) malloc(worker_thread_count * sizeof(pthread_t));
	if (tids == NULL) {
		perror("EL_Comm_TCP init pthreads");
		return -1;
	}

	tids_status = (time_t*) malloc((worker_thread_count+1) * sizeof(time_t));
	if (tids_status == NULL) {
		perror("EL_Comm_TCP init pthreads_status");
		return -1;
	}

	if (connq.init())
		return -1;

	return 0;
}		// -----  end of function init  -----

void EL_Comm_TCP::destroy()
{
	if (tids) {
		free(tids);
		tids = NULL;
	}

	if (tids_status) {
		free(tids_status);
		tids_status = NULL;
	}

	connq.destroy();
}		// -----  end of function destroy  -----

int EL_Comm_TCP::start(ushort port, ushort monitor_port, time_t timeout_sec)
{
	this->worker_port = port;
	this->monitor_port = monitor_port;

	/* create worker threads */
	counter = 0;
	for (int i=0; i<worker_thread_count; ++i) {
		if (pthread_create(&tids[i], NULL, create_session, this) < 0) {
			perror("create worker thread");
			return -1;
		}
	}

	/* create monitor threads */
	if (pthread_create(&monitor_tid, NULL, create_monitor, this) < 0) {
		perror("create monitor thread");
		return -1;
	}

	/* starting workers listening */
	int sockfd;
	struct sockaddr_in servaddr;

	signal(SIGPIPE, SIG_IGN);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0))  == -1) {
		perror("create socket");
		return -1;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {                                                                           
		perror("bind");
		return -1;
	}

	if (listen(sockfd, listenq) == -1) {
		perror("listen");
		return -1;
	}

	for ( ; ; ) {
		int connfd;
		socklen_t addrlen;
		struct sockaddr_in cliaddr;
		char client_ip[16];
		struct timeval timeout;

		if (timeout_sec > 0) {
			timeout.tv_sec = timeout_sec;
			timeout.tv_usec = 0;
		} else {
			timeout.tv_sec = 0;
			timeout.tv_usec = 0;
		}

		addrlen = sizeof(cliaddr);
		if ((connfd = accept(sockfd, (struct sockaddr*)&cliaddr, &addrlen)) == -1) {
			perror("accept");
			return -1;
		}
		inet_ntop(AF_INET, &cliaddr.sin_addr, client_ip, sizeof(client_ip));
		printf("connection from: %s, port: %d\n", client_ip, ntohs(cliaddr.sin_port));

		if (timeout.tv_sec) {
			setsockopt(connfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));                           
			setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
		}

		connq.push(connfd);
//		close(connfd);
	}
	close(sockfd);

	return 0;
}		// -----  end of function start  -----

int EL_Comm_TCP::connect(const char *ip, ushort port, time_t timeout_sec)
{
	int sockfd;
	struct sockaddr_in servaddr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("connect");
		return -1;
	}   

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &servaddr.sin_addr);

	struct timeval timeout;

	if (timeout_sec > 0) {
		timeout.tv_sec = timeout_sec;
		timeout.tv_usec = 0;
	} else {
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
	}

	if (timeout.tv_sec) {
		setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));                           
		setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	}

	if (::connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) {
		perror("connect");
		return -1;
	}   

	close(sockfd);

	return sockfd;
}		// -----  end of function connect  -----

void EL_Comm_TCP::disconnect(int socket)
{
	close(socket);
}		// -----  end of function disconnect  -----

ssize_t EL_Comm_TCP::send(int socket, const void *buf, size_t len, int flags)
{
	ssize_t ret=0, finished=0;

	do {
		ret = ::send(socket, ((char*)buf)+finished, len-finished, flags);
		if (ret >= 0) {
			finished += ret;
		} else {
			return -1; 
		}
	} while(ret && finished < (long)len);

	return finished;
}		// -----  end of function send  -----

ssize_t EL_Comm_TCP::recv(int socket, void *buf, size_t len, int flags)
{
	ssize_t ret=0, finished=0;
	do {
		ret = ::recv(socket, ((char*)buf)+finished, len-finished, flags);
		if (ret >= 0) {
			finished += ret;
		} else {
			return -1;
		}
	} while(ret && finished < (long)len);

	return finished;
}		// -----  end of function recv  -----

int EL_Comm_TCP::monitor(int socket, int status)
{
	ssize_t recvlen;
	char buf[BUFSIZ];
	char *p = buf;

	if ((recvlen = recv(socket, buf, BUFSIZ, 0)) < 0) {
		perror("monitor recv");
		close(socket);
		return -1;
	}

	p = buf;
	memcpy(p, "MonitorP1", 10); p+=10;

	if (recvlen != 4) {
		*(int32_t*)p=4; p+=4;
		*(int32_t*)p=TYPE_NETWORK; p+=4;
	} else {
		if (status < 0) {
			printf("server error: %d\n", status);
			*(int32_t*)(p+4) = TYPE_SERVICE;
			*(int32_t*)(p+8) = LEVEL_A;
		} else {
			*(int32_t*)(p+4) = TYPE_OK;
			*(int32_t*)(p+8) = LEVEL_B;
		}
		*(int32_t*)p = 8; p+=3*4;
	}

	if ((recvlen = send(socket, buf, p-buf, 0)) < 0) {
		perror("thread send back");
		close(socket);
		return -1;
	}

	return 0;
}		// -----  end of function monitor  -----

// ==================== PRIVATE METHOD =======================================

void* EL_Comm_TCP::create_session(void *arg)
{
	EL_Comm_TCP *obj = (EL_Comm_TCP*)arg;
	int index = (int)atom_fetch_and_add(&obj->counter, 1);
	obj->tids_status[index] = 0;

	for ( ; ; ) {
		int socket = obj->connq.pop();
		obj->tids_status[index] = time(NULL);
		obj->session(socket);
		close(socket);
		obj->tids_status[index] = 0;
	}

	return NULL;
}		// -----  end of static function create_session  -----

void* EL_Comm_TCP::create_monitor(void *arg)
{
	EL_Comm_TCP *obj = (EL_Comm_TCP*)arg;

	int sockfd, connfd;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t addrlen;
	char client_ip[16];

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0))  == -1) {
		perror("monitor create socket");
		return NULL;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(obj->monitor_port);

	if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
		perror("monitor bind");
		return NULL;
	}

	if (listen(sockfd, listenq) == -1) {
		perror("monitor listen");
		return NULL;
	}

	for ( ; ; ) {
		addrlen = sizeof(cliaddr);
		if ((connfd = accept(sockfd, (struct sockaddr*)&cliaddr, &addrlen)) == -1) {
			perror("monitor accept");
			return NULL;
		}
		inet_ntop(AF_INET, &cliaddr.sin_addr, client_ip, sizeof(client_ip));
		printf("monitor connection from: %s, port: %d\n", client_ip, ntohs(cliaddr.sin_port));

		struct timeval timeout = {1, 0};
		setsockopt(connfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
		setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

		int status = obj->thread_status();
		obj->monitor(connfd, status);
		close(connfd);
	}

	close(sockfd);

	return NULL;
}		// -----  end of static function create_monitor  -----

int EL_Comm_TCP::thread_status()
{
	time_t current = time(NULL);
	for (int i=0; i<worker_thread_count; ++i) {
		if (tids_status[i] && tids_status[i] - current > 5) {
			return -1;
		}
	}

	return 0;
}		// -----  end of function thread_status  -----
