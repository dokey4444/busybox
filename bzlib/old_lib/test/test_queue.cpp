/*
// =====================================================================================
// 
//       Filename:  test_queue.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  07/16/2013 03:13:23 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "elib_util.hpp"
#include "elib_queue.hpp"

static int test_queue_count = 0;

static void test_case_1()
{
	EL_Queue_TS<int> q;

	assert(q.is_empty());
	assert(q.size() == 0);

	q.init(8);
	assert(q.is_empty()); assert(!q.is_full());

	q.push(1); q.push(2); q.push(3); q.push(4);
	q.push(5); q.push(6); q.push(7); q.push(8);

	assert(q.size() == 8);
	assert(!q.is_empty()); assert(q.is_full());

	assert(q.pop() == 1); assert(q.pop() == 2);
	assert(q.pop() == 3); assert(q.pop() == 4);
	assert(q.pop() == 5); assert(q.pop() == 6);
	assert(q.pop() == 7); assert(q.pop() == 8);
	assert(q.size() == 0);

	q.destroy();

	q.init(8);
	assert(q.is_empty()); assert(!q.is_full());
	assert(q.size() == 0);

	assert(q.try_push(1) == 0); assert(q.try_push(2) == 0);
	assert(q.try_push(3) == 0); assert(q.try_push(4) == 0);
	assert(q.try_push(5) == 0); assert(q.try_push(6) == 0);
	assert(q.try_push(7) == 0); assert(q.try_push(8) == 0);
	assert(q.size() == 8);

	assert(!q.is_empty()); assert(q.is_full());

	int node;
	assert(q.try_pop(&node) == 0); assert(node == 1);
	assert(q.try_pop(&node) == 0); assert(node == 2);
	assert(q.try_pop(&node) == 0); assert(node == 3);
	assert(q.try_pop(&node) == 0); assert(node == 4);
	assert(q.try_pop(&node) == 0); assert(node == 5);
	assert(q.try_pop(&node) == 0); assert(node == 6);
	assert(q.try_pop(&node) == 0); assert(node == 7);
	assert(q.try_pop(&node) == 0); assert(node == 8);
	assert(q.size() == 0);

	q.destroy();

	puts("test case 1: ok~");
}

static void test_case_2()
{
	EL_Queue_TS<int> q;
	q.init(8);
	assert(q.is_empty()); assert(!q.is_full());

	q.push(1); q.push(2); q.push(3); q.push(4);
	assert(!q.is_empty()); assert(!q.is_full());

	q.push(5); q.push(6); q.push(7); q.push(8);
	assert(!q.is_empty()); assert(q.is_full());

	q.reset();
	assert(q.is_empty()); assert(!q.is_full());

	puts("test case 2: ok~");
}

static void* thread_run(void *arg)
{
	EL_Queue_TS<int> *p = (EL_Queue_TS<int>*) arg;
	int list[128];

	for (int i = 0; i<128; ++i) {
		list[i] = p->pop();
	}
	for (int i = 0; i<128; ++i) {
		assert(list[i] == i);
	}

	return NULL;
}

static void* thread_tryrun(void *arg)
{
	EL_Queue_TS<int> *p = (EL_Queue_TS<int>*) arg;
	int list[128];

	for (int i = 0; i<128; ++i) {
		if (p->try_pop(&list[i]) < 0)
			--i;
	}
	for (int i = 0; i<128; ++i) {
		assert(list[i] == i);
	}

	return NULL;
}

static void test_case_3()
{
	EL_Queue_TS<int> q;

	pthread_t tid;
	q.init(8);
	pthread_create(&tid, NULL, thread_run, &q);
	for (int i = 0; i<128; ++i) {
		q.push(i);
	}
	pthread_join(tid, NULL);
	q.destroy();

	q.init(128);
	pthread_create(&tid, NULL, thread_tryrun, &q);
	for (int i = 0; i<128; ++i) {
		q.try_push(i);
	}
	pthread_join(tid, NULL);
	q.destroy();

	puts("test case 3: ok~");
}

static void* thread_trypush(void *arg)
{
	EL_Queue_TS<int> *p = (EL_Queue_TS<int>*) arg;

	for (int i = 0; i<100000; ++i) {
		if (p->try_push(i) < 0)
			--i;
	}

	return NULL;
}

static void* thread_trypop(void *arg)
{
	EL_Queue_TS<int> *p = (EL_Queue_TS<int>*) arg;

	while (test_queue_count != 4 * 100000) {
		int node;
		if (p->try_pop(&node) == 0)
			atom_add_and_fetch(&test_queue_count, 1);
	}

	return NULL;
}

static void test_case_4()
{
	EL_Queue_TS<int> q;
	q.init();

	test_queue_count = 0;

	pthread_t push_tids[4], pop_tids[4];
	for (int i=0; i<4; ++i) {
		pthread_create(&push_tids[i], NULL, thread_trypush, &q);
		pthread_create(&pop_tids[i], NULL, thread_trypop, &q);
	}

	for (int i=0; i<4; ++i) {
		pthread_join(push_tids[i], NULL);
		pthread_join(pop_tids[i], NULL);
	}

	assert(test_queue_count == 4 * 100000);

	q.destroy();

	puts("test case 4: ok~");
}

static void* thread_push(void *arg)
{
	EL_Queue_TS<int> *p = (EL_Queue_TS<int>*) arg;

	for (int i = 0; i<100000; ++i) {
		p->push(i);
	}

	return NULL;
}

static void* thread_pop(void *arg)
{
	EL_Queue_TS<int> *p = (EL_Queue_TS<int>*) arg;

	while (test_queue_count != 4 * 100000) {
		p->pop();
		atom_add_and_fetch(&test_queue_count, 1);
	}

	return NULL;
}

static void test_case_5()
{
	EL_Queue_TS<int> q;
	q.init();

	test_queue_count = 0;

	pthread_t push_tids[4], pop_tids[4];
	for (int i=0; i<4; ++i) {
		pthread_create(&push_tids[i], NULL, thread_push, &q);
		pthread_create(&pop_tids[i], NULL, thread_pop, &q);
	}

	while (test_queue_count != 4 * 100000)
		usleep(100 * 1000);

	assert(test_queue_count == 4 * 100000);

	for (int i=0; i<4; ++i) {
		pthread_join(push_tids[i], NULL);
//		pthread_join(pop_tids[i], NULL);	// lead to memory leak
	}

	puts("test case 5: ok~");
}

/* 测试非线程安全队列，接口和功能与线程安全版本完全一直（没有try接口） */
static void test_case_6()
{
	EL_Queue<int> q;

	assert(q.is_empty());
	assert(q.size() == 0);

	q.init(8);
	assert(q.is_empty()); assert(!q.is_full());

	q.push(1); q.push(2); q.push(3); q.push(4);
	q.push(5); q.push(6); q.push(7); q.push(8);

	assert(q.size() == 8);
	assert(!q.is_empty()); assert(q.is_full());

	assert(q.pop() == 1); assert(q.pop() == 2);
	assert(q.pop() == 3); assert(q.pop() == 4);
	assert(q.pop() == 5); assert(q.pop() == 6);
	assert(q.pop() == 7); assert(q.pop() == 8);
	assert(q.size() == 0);

	q.destroy();

	puts("test case 6: ok~");
}

void test_queue()
{
	test_case_1();
	test_case_2();
	test_case_3();
	test_case_4();
	test_case_5();

	test_case_6();

	puts("test queue is finished~");
}
