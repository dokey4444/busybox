/*
// =====================================================================================
// 
//       Filename:  test_mm_fixed.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  01/03/2014 11:45:54 AM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "elib_util.hpp"
#include "elib_mm_fixed.hpp"

/* for tracing the program */
static void test_case_0()
{/*
	EL_MM_Fixed mm;
	mm.init(256, 32, 24, 1024);

//	for (uint i=0; i<(1<<24)-1; ++i) {
//		int n = mm.get_index_to_alloc();
//		assert(n == i);
//		if (i % 4096) {
//			printf("i = %d\tn = %d\n", i, n);
//		}
//	}

	int r, n;

	r = mm.get_index_to_reuse();
	printf("r = %d\n", r);

	memset(mm.alloc_bm[0], 0xff, (1+64+64*64+64*64*64)*sizeof(uint64_t));	// same as for loop upward
	n = mm.get_index_to_alloc();
	printf("n = %d\n", n);

	mm.set_recycle(1234);
	mm.set_recycle(1235);

	r = mm.get_index_to_reuse();
	printf("r = %d\n", r);
	r = mm.get_index_to_reuse();
	printf("r = %d\n", r);

	mm.set_recycle_empty(1234);
	r = mm.get_index_to_reuse();
	printf("r = %d\n", r);

	mm.set_recycle(1235);
	mm.set_recycle(1235);
	mm.set_recycle(1235);
	mm.set_recycle(1235);
	r = mm.get_index_to_reuse();
	printf("r = %d\n", r);

	mm.set_recycle_full(1235);
	r = mm.get_index_to_reuse();
	printf("r = %d\n", r);
	r = mm.get_index_to_alloc();
	printf("r = %d\n", r);

	mm.set_recycle(1000);
	mm.set_recycle_full(1000);
	r = mm.get_index_to_reuse();
	printf("r = %d\n", r);
	r = mm.get_index_to_alloc();
	printf("r = %d\n", r);

	mm.destroy();
*/
}

/* just alloc */
static void test_case_1()
{
	EL_MM_Fixed mm;
	mm.init(300, 10, 10, 2);
	char *a, *b, *c;

	for (int i=0; i<1024; ++i) {
		assert(a = (char*)mm.alloc());
		assert(b = (char*)mm.alloc());
		assert(c = (char*)mm.alloc());
		assert(a < b);
		assert(b < c);
		assert(c < a+1024);
	}

	assert(mm.alloc() == NULL);
	mm.destroy();

	mm.init(256, 32, 24, 2);

	puts("test case 1: ok~");
}

/* alloc & recycle */
static void test_case_2()
{
	EL_MM_Fixed mm;
	mm.init(300, 10, 10, 2);
	char *a[1024], *b[1024], *c[1024];

	for (int i=0; i<1024; ++i) {
		assert(a[i] = (char*)mm.alloc());
		assert(b[i] = (char*)mm.alloc());
		assert(c[i] = (char*)mm.alloc());
		assert(*(int*)(a[i]-4) == i);	// check index
		assert(a[i]+304 == b[i]);	// 4 byte for control
		assert(b[i]+304 == c[i]);	// 4 byte for control
	}

	assert(mm.alloc() == NULL);

	mm.recycle(a[0]);
	mm.recycle(b[0]);
	mm.recycle(c[0]);
	mm.recycle(a[1]);
	mm.recycle(b[1]);
	mm.recycle(c[1]);

	assert((char*)mm.alloc() == c[0]);
	assert((char*)mm.alloc() == b[0]);
	assert((char*)mm.alloc() == a[0]);
	assert((char*)mm.alloc() == c[1]);
	assert((char*)mm.alloc() == b[1]);
	assert((char*)mm.alloc() == a[1]);

	for (int i=2; i<1024; ++i) {
		mm.recycle(a[i]);
		mm.recycle(b[i]);
		mm.recycle(c[i]);
	}

	mm.destroy();

	mm.init(256);

	puts("test case 2: ok~");
}

static void* pthread_run(void *arg)
{
	EL_MM_Fixed *mm = (EL_MM_Fixed*)arg;

	char *head = NULL;

	for (int i=0; i<100; ++i) {
		if (rand() & 0x3) {
			char *p = (char*)mm->alloc();
			*(char**)p = head;
			head = p;
		} else {
			if (head == NULL)
				continue;
			char *p = *(char**)head;
			mm->recycle(head);
			head = p;
		}
	}

	while (head) {
			char *p = *(char**)head;
			mm->recycle(head);
			head = p;
	}

	return NULL;
}

/* stress testing */
static void test_case_3()
{
	EL_MM_Fixed mm;
	mm.init(8, 32, 24, 4);
 // :BUG:02/09/2014 11:06:38 PM:elwin: when the fourth param bigger than 6, valgrind will core dump

	srand((uint)time(NULL));

	pthread_t tids[1024];
	for (int i=0; i<1024; ++i) {
		pthread_create(&tids[i], NULL, pthread_run, &mm);
	}

	for (int i=0; i<1024; ++i) {
		pthread_join(tids[i], NULL);
	}

	mm.destroy();

	puts("test case 3: ok~");
}

void test_mm_fixed()
{
	test_case_0();
	test_case_1();
	test_case_2();
	test_case_3();

	puts("test mm fixed is finished~");
}
