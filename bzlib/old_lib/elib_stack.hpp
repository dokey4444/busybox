/*
// =====================================================================================
// 
//       Filename:  elib_stack.hpp
// 
//    Description:  普通顺序栈
// 
//        Version:  1.0
//        Created:  07/17/2013 09:14:10 AM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#ifndef  _ELIB_STACK_H_
#define  _ELIB_STACK_H_

#include "elib_util.hpp"

// =====================================================================================
//        Class:  EL_Stack
//  Description:  
// =====================================================================================

template <typename T>
class EL_Stack
{
public:
	// ====================  LIFECYCLE     =======================================
	EL_Stack();
	~EL_Stack();

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  init
	//  Description:  初始化栈（非幂等）
	// =====================================================================================
	*/
	int init(int length = 4096, byte *buf = NULL);

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  destroy
	//  Description:  销毁栈（非幂等）
	// =====================================================================================
	*/
	int destroy();

	// ====================  INTERFACE     =======================================
	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  push
	//  Description:  阻塞式入栈
	// =====================================================================================
	*/
	int push(T node);

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  pop
	//  Description:  阻塞式出栈
	// =====================================================================================
	*/
	T pop();

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  check
	//  Description:  阻塞式查看栈顶元素，但并不弹栈
	// =====================================================================================
	*/
	T check();

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  is_empty
	//  Description:  栈为空返回true，否则返回false
	// =====================================================================================
	*/
	bool is_empty();

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  is_full
	//  Description:  栈已满返回true，否则返回false
	// =====================================================================================
	*/
	bool is_full();

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  size
	//  Description:  获取当前栈的大小
	// =====================================================================================
	*/
	long size();

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  reset
	//  Description:  清空栈，注意该函数不同于destroy
	// =====================================================================================
	*/
	void reset();

private:
	// ====================  DATA MEMBERS  =======================================
	const int max_limit;	// default max limit: 1048576(1M)

	T *stack_buf;
	long stack_len;

	long current_size;
};		// -----  end of template class EL_Stack  -----

/* -------------------------------- implementation -------------------------------- */

template <typename T>
EL_Stack<T>::EL_Stack() : max_limit(1<<20) ,stack_buf(NULL)
{
	current_size = 0;
}

template <typename T>
EL_Stack<T>::~EL_Stack()
{
	if (stack_buf)
		destroy();
}

template <typename T>
int EL_Stack<T>::init(int length, byte *buf)
{
	if (length > max_limit) {
		log_printf("length out of limit: %d\n", max_limit);
		return -1;
	}

	if (stack_buf) {
		err_printf("EL_Stack::init: stack exist, can't init again\n");
		assert(stack_buf == NULL);	// for debug
		return -1;
	}

	if (buf) {
		stack_buf = (T*) new(buf) T[length];
	} else {
		stack_buf = (T*) new(std::nothrow) T[length];
		if (stack_buf == NULL) {
			sys_perror("EL_Stack::init");
			return -1;
		}
	}

	stack_len = length;

	current_size = 0;

	return 0;
}

template <typename T>
int EL_Stack<T>::destroy()
{
	if (current_size != 0) {
		log_printf("destroy warning: stack is not empty\n");
	}

	if (stack_buf)
		delete[] stack_buf;
	stack_buf = NULL;

	return 0;
}

template <typename T>
int EL_Stack<T>::push(T node)
{
	if (current_size == stack_len)
		return -1;

	stack_buf[current_size++] = node;
	return 0;
}

template <typename T>
T EL_Stack<T>::pop()
{
	assert(current_size);
	return stack_buf[--current_size];
}

template <typename T>
T EL_Stack<T>::check()
{
	assert(current_size);
	return stack_buf[current_size - 1];
}

template <typename T>
bool EL_Stack<T>::is_empty()
{
	return current_size == 0;
}

template <typename T>
bool EL_Stack<T>::is_full()
{
	return current_size == stack_len;
}

template <typename T>
long EL_Stack<T>::size()
{
	return current_size;
}

template <typename T>
void EL_Stack<T>::reset()
{
	current_size = 0;
}

// =====================================================================================
//        Class:  EL_Stack_TS
//  Description:  
// =====================================================================================

template <typename T>
class EL_Stack_TS
{
public:
	// ====================  LIFECYCLE     =======================================
	EL_Stack_TS();
	~EL_Stack_TS();

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  init
	//  Description:  初始化栈（非幂等）
	// =====================================================================================
	*/
	int init(int length = 4096, byte *buf = NULL);

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  destroy
	//  Description:  销毁栈（非幂等）
	// =====================================================================================
	*/
	int destroy();

	// ====================  INTERFACE     =======================================
	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  push
	//  Description:  阻塞式入栈
	// =====================================================================================
	*/
	void push(T node);

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  pop
	//  Description:  阻塞式出栈
	// =====================================================================================
	*/
	T pop();

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  check
	//  Description:  阻塞式查看栈顶元素，但并不弹栈
	// =====================================================================================
	*/
	T check();

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  try_push
	//  Description:  非阻塞式入栈，成功返回0，失败返回-1
	// =====================================================================================
	*/
	int try_push(T node);

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  try_pop
	//  Description:  非阻塞式出栈，成功返回0，失败返回-1
	// =====================================================================================
	*/
	int try_pop(T *node);

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  try_check
	//  Description:  非阻塞式查看栈顶元素，但并不弹栈，成功返回0，失败返回-1
	// =====================================================================================
	*/
	int try_check(T *node);

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  is_empty
	//  Description:  栈为空返回true，否则返回false
	// =====================================================================================
	*/
	bool is_empty();

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  is_full
	//  Description:  栈已满返回true，否则返回false
	// =====================================================================================
	*/
	bool is_full();

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  size
	//  Description:  获取当前栈的大小
	// =====================================================================================
	*/
	long size();

	/*
	// ===  FUNCTION  ======================================================================
	//         Name:  reset
	//  Description:  清空栈，注意该函数不同于destroy
	// =====================================================================================
	*/
	void reset();

private:
	// ====================  DATA MEMBERS  =======================================
	const int max_limit;	// default max limit: 1048576(1M)

	T *stack_buf;
	long stack_len;

	long current_size;

	pthread_mutex_t mutex;
	pthread_cond_t cond;

};		// -----  end of template class EL_Stack_TS  -----

/* -------------------------------- implementation -------------------------------- */

template <typename T>
EL_Stack_TS<T>::EL_Stack_TS() : max_limit(1<<20) ,stack_buf(NULL)
{
	current_size = 0;
}

template <typename T>
EL_Stack_TS<T>::~EL_Stack_TS()
{
	if (stack_buf)
		destroy();
}

template <typename T>
int EL_Stack_TS<T>::init(int length, byte *buf)
{
	if (length > max_limit) {
		log_printf("length out of limit: %d\n", max_limit);
		return -1;
	}

	if (stack_buf) {
		err_printf("EL_Stack_TS::init: stack exist, can't init again\n");
		assert(stack_buf == NULL);	// for debug
		return -1;
	}

	if (buf) {
		stack_buf = (T*) new(buf) T[length];
	} else {
		stack_buf = (T*) new(std::nothrow) T[length];
		if (stack_buf == NULL) {
			sys_perror("EL_Stack_TS::init");
			return -1;
		}
	}

	stack_len = length;

	current_size = 0;

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	return 0;
}

template <typename T>
int EL_Stack_TS<T>::destroy()
{
	if (current_size != 0) {
		log_printf("destroy warning: stack is not empty\n");
	}

	if(pthread_cond_destroy(&cond) < 0) {
		sys_perror("EL_Stack_TS::destroy");
		return -1;
	}

	if (pthread_mutex_destroy(&mutex) < 0) {
		sys_perror("EL_Stack_TS::destroy");
		return -1;
	}

	if (stack_buf)
		delete[] stack_buf;
	stack_buf = NULL;

	return 0;
}

template <typename T>
void EL_Stack_TS<T>::push(T node)
{
	pthread_mutex_lock(&mutex);

	while (current_size == stack_len)
		pthread_cond_wait(&cond, &mutex);

	stack_buf[current_size++] = node;

	if (current_size == 1)
		pthread_cond_broadcast(&cond);

	pthread_mutex_unlock(&mutex);

}

template <typename T>
T EL_Stack_TS<T>::pop()
{
	T node;

	pthread_mutex_lock(&mutex);

	while (current_size == 0)
		pthread_cond_wait(&cond, &mutex);

	node = stack_buf[--current_size];

	if (current_size + 1 == stack_len)
		pthread_cond_broadcast(&cond);

	pthread_mutex_unlock(&mutex);


	return node;
}

template <typename T>
T EL_Stack_TS<T>::check()
{
	T node;

	pthread_mutex_lock(&mutex);

	while (current_size == 0)
		pthread_cond_wait(&cond, &mutex);

	node = stack_buf[current_size - 1];

	pthread_mutex_unlock(&mutex);


	return node;
}

template <typename T>
int EL_Stack_TS<T>::try_push(T node)
{
	if (current_size == stack_len)
		return -1;

	pthread_mutex_lock(&mutex);

	if (current_size == stack_len) {
		pthread_mutex_unlock(&mutex);
		return -1;
	}

	stack_buf[current_size++] = node;

	pthread_mutex_unlock(&mutex);

	return 0;
}

template <typename T>
int EL_Stack_TS<T>::try_pop(T *node)
{
	if (current_size == 0)
		return -1;

	pthread_mutex_lock(&mutex);

	if (current_size == 0) {
		pthread_mutex_unlock(&mutex);
		return -1;
	}

	*node = stack_buf[--current_size];

	pthread_mutex_unlock(&mutex);

	return 0;
}

template <typename T>
int EL_Stack_TS<T>::try_check(T *node)
{
	if (current_size == 0)
		return -1;

	pthread_mutex_lock(&mutex);

	if (current_size == 0) {
		pthread_mutex_unlock(&mutex);
		return -1;
	}

	*node = stack_buf[current_size - 1];

	pthread_mutex_unlock(&mutex);

	return 0;
}

template <typename T>
bool EL_Stack_TS<T>::is_empty()
{
	return current_size == 0;
}

template <typename T>
bool EL_Stack_TS<T>::is_full()
{
	return current_size == stack_len;
}

template <typename T>
long EL_Stack_TS<T>::size()
{
	return current_size;
}

template <typename T>
void EL_Stack_TS<T>::reset()
{
	current_size = 0;
}

#endif   // ----- #ifndef _ELIB_STACK_H_  -----

