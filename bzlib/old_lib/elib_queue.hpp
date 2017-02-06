/*
// =====================================================================================
// 
//       Filename:  elib_queue.hpp
// 
//    Description:  普通顺序队列
// 
//        Version:  1.0
//        Created:  07/13/2013 05:18:10 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#ifndef  _ELIB_QUEUE_H_
#define  _ELIB_QUEUE_H_

#include "elib_util.hpp"

// =====================================================================================
//        Class:  EL_Queue
//  Description:  
// =====================================================================================

template <typename T>
class EL_Queue
{
public:
	// ====================  LIFECYCLE     =======================================
	EL_Queue();
	~EL_Queue();

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  init
	//  Description:  初始化队列（非幂等）
	// =====================================================================================
	*/
	int init(long length = 4096, byte *buf = NULL);

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  destroy
	//  Description:  销毁队列（非幂等）
	// =====================================================================================
	*/
	int destroy();

	// ====================  INTERFACE     =======================================
	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  push
	//  Description:  
	// =====================================================================================
	*/
	int push(T node);

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  pop
	//  Description:  
	// =====================================================================================
	*/
	T pop();

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  is_empty
	//  Description:  队列为空返回true，否则返回false
	// =====================================================================================
	*/
	bool is_empty();

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  is_full
	//  Description:  队列已满返回true，否则返回false
	// =====================================================================================
	*/
	bool is_full();

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  size
	//  Description:  获取当前队列的大小
	// =====================================================================================
	*/
	long size();

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  reset
	//  Description:  清空队列，注意该函数不同于destroy
	// =====================================================================================
	*/
	void reset();

private:
	// ====================  DATA MEMBERS  =======================================
	const int max_limit;	// default max limit: 1048576(1M)

	T *queue_buf;
	long queue_len;

	long pos_begin;
	long pos_end;

	long current_size;
};		// -----  end of template class EL_Queue  -----

/* -------------------------------- implementation -------------------------------- */

template <typename T>
EL_Queue<T>::EL_Queue() : max_limit(1<<20) ,queue_buf(NULL)
{
	pos_begin = 0;
	pos_end = 0;
	
	current_size = 0;
}

template <typename T>
EL_Queue<T>::~EL_Queue()
{
	if (queue_buf)
		destroy();
}

template <typename T>
int EL_Queue<T>::init(long length, byte *buf)
{
	if (length > max_limit) {
		log_printf("length out of limit: %d\n", max_limit);
		return -1;
	}

	if (queue_buf) {
		err_printf("EL_Queue::init: queue exist, can't init again\n");
		assert(queue_buf == NULL);	// for debug
		return -1;
	}

	if (buf) {
		queue_buf = (T*) new(buf) T[length];
	} else {
		queue_buf = (T*) new(std::nothrow) T[length];
		if (queue_buf == NULL) {
			sys_perror("EL_Queue::init");
			return -1;
		}
	}

	queue_len = length;

	pos_begin = 0;
	pos_end = 0;
	
	current_size = 0;

	return 0;
}

template <typename T>
int EL_Queue<T>::destroy()
{
	if (current_size != 0) {
		log_printf("destroy warning: queue is not empty\n");
	}

	if (queue_buf)
		delete[] queue_buf;
	queue_buf = NULL;

	return 0;
}

template <typename T>
int EL_Queue<T>::push(T node)
{
	if (current_size == queue_len)
		return -1;

	queue_buf[pos_end++] = node;
	pos_end %= queue_len;
	++current_size;

	return 0;
}

template <typename T>
T EL_Queue<T>::pop()
{
	T node;

	assert(current_size);

	node = queue_buf[pos_begin++];
	pos_begin %= queue_len;
	--current_size;

	return node;
}

template <typename T>
bool EL_Queue<T>::is_empty()
{
	return current_size == 0;
}

template <typename T>
bool EL_Queue<T>::is_full()
{
	return current_size == queue_len;
}

template <typename T>
long EL_Queue<T>::size()
{
	return current_size;
}

template <typename T>
void EL_Queue<T>::reset()
{
	pos_begin = 0;
	pos_end = 0;
	current_size = 0;
}


// =====================================================================================
//        Class:  EL_Queue_TS
//  Description:  
// =====================================================================================

template <typename T>
class EL_Queue_TS
{
public:
	// ====================  LIFECYCLE     =======================================
	EL_Queue_TS();
	~EL_Queue_TS();

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  init
	//  Description:  初始化队列（非幂等）
	// =====================================================================================
	*/
	int init(int length = 4096, byte *buf = NULL);

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  destroy
	//  Description:  销毁队列（非幂等）
	// =====================================================================================
	*/
	int destroy();

	// ====================  INTERFACE     =======================================
	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  push
	//  Description:  阻塞式入队列
	// =====================================================================================
	*/
	void push(T node);

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  pop
	//  Description:  阻塞式出队列
	// =====================================================================================
	*/
	T pop();

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  try_push
	//  Description:  非阻塞式入队列，成功返回0，失败返回-1
	// =====================================================================================
	*/
	int try_push(T node);

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  try_pop
	//  Description:  非阻塞式出队列，成功返回0，失败返回-1
	// =====================================================================================
	*/
	int try_pop(T *node);

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  is_empty
	//  Description:  队列为空返回true，否则返回false
	// =====================================================================================
	*/
	bool is_empty();

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  is_full
	//  Description:  队列已满返回true，否则返回false
	// =====================================================================================
	*/
	bool is_full();

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  size
	//  Description:  获取当前队列的大小
	// =====================================================================================
	*/
	long size();

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  reset
	//  Description:  清空队列，注意该函数不同于destroy
	// =====================================================================================
	*/
	void reset();

private:
	// ====================  DATA MEMBERS  =======================================
	const int max_limit;	// default max limit: 1048576(1M)

	T *queue_buf;
	long queue_len;

	long pos_begin;
	long pos_end;

	long current_size;

	pthread_mutex_t mutex;
	pthread_cond_t cond;

};		// -----  end of template class EL_Queue_TS  -----

/* -------------------------------- implementation -------------------------------- */

template <typename T>
EL_Queue_TS<T>::EL_Queue_TS() : max_limit(1<<20) ,queue_buf(NULL)
{
	pos_begin = 0;
	pos_end = 0;
	
	current_size = 0;
}

template <typename T>
EL_Queue_TS<T>::~EL_Queue_TS()
{
	if (queue_buf)
		destroy();
}

template <typename T>
int EL_Queue_TS<T>::init(int length, byte *buf)
{
	if (length > max_limit) {
		log_printf("length out of limit: %d\n", max_limit);
		return -1;
	}

	if (queue_buf) {
		err_printf("EL_Queue_TS::init: queue exist, can't init again\n");
		assert(queue_buf == NULL);	// for debug
		return -1;
	}

	if (buf) {
		queue_buf = (T*) new(buf) T[length];
	} else {
		queue_buf = (T*) new(std::nothrow) T[length];
		if (queue_buf == NULL) {
			sys_perror("EL_Queue_TS::init");
			return -1;
		}
	}

	queue_len = length;

	pos_begin = 0;
	pos_end = 0;
	
	current_size = 0;

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	return 0;
}

template <typename T>
int EL_Queue_TS<T>::destroy()
{
	if (current_size != 0) {
		log_printf("destroy warning: queue is not empty\n");
	}

	if(pthread_cond_destroy(&cond) < 0) {
		sys_perror("EL_Queue_TS::destroy");
		return -1;
	}

	if (pthread_mutex_destroy(&mutex) < 0) {
		sys_perror("EL_Queue_TS::destroy");
		return -1;
	}

	if (queue_buf)
		delete[] queue_buf;
	queue_buf = NULL;

	return 0;
}

template <typename T>
void EL_Queue_TS<T>::push(T node)
{
	pthread_mutex_lock(&mutex);

	while (current_size == queue_len)
		pthread_cond_wait(&cond, &mutex);

	queue_buf[pos_end++] = node;
	pos_end %= queue_len;
	++current_size;

	if (current_size == 1)
		pthread_cond_broadcast(&cond);

	pthread_mutex_unlock(&mutex);

}

template <typename T>
T EL_Queue_TS<T>::pop()
{
	T node;

	pthread_mutex_lock(&mutex);

	while (current_size == 0)
		pthread_cond_wait(&cond, &mutex);

	node = queue_buf[pos_begin++];
	pos_begin %= queue_len;
	--current_size;

	if (current_size + 1 == queue_len)
		pthread_cond_broadcast(&cond);

	pthread_mutex_unlock(&mutex);


	return node;
}

template <typename T>
int EL_Queue_TS<T>::try_push(T node)
{
	if (current_size == queue_len)
		return -1;

	pthread_mutex_lock(&mutex);

	if (current_size == queue_len) {
		pthread_mutex_unlock(&mutex);
		return -1;
	}

	queue_buf[pos_end++] = node;
	pos_end %= queue_len;
	++current_size;

	pthread_mutex_unlock(&mutex);

	return 0;
}

template <typename T>
int EL_Queue_TS<T>::try_pop(T *node)
{
	if (current_size == 0)
		return -1;

	pthread_mutex_lock(&mutex);

	while (current_size == 0) {
		pthread_mutex_unlock(&mutex);
		return -1;
	}

	*node = queue_buf[pos_begin++];
	pos_begin %= queue_len;
	--current_size;

	pthread_mutex_unlock(&mutex);

	return 0;
}

template <typename T>
bool EL_Queue_TS<T>::is_empty()
{
	return current_size == 0;
}

template <typename T>
bool EL_Queue_TS<T>::is_full()
{
	return current_size == queue_len;
}

template <typename T>
long EL_Queue_TS<T>::size()
{
	return current_size;
}

template <typename T>
void EL_Queue_TS<T>::reset()
{
	pos_begin = 0;
	pos_end = 0;
	current_size = 0;
}

#endif   // ----- #ifndef _ELIB_QUEUE_H_  -----

