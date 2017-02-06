/*
// =====================================================================================
// 
//       Filename:  test_mm_block.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  09/02/2013 10:07:11 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "elib_util.hpp"
#include "elib_mm_block.hpp"

static void test_case_1()
{
	void *p[32] = {NULL};

	EL_MM_Block mm;
	assert(mm.init(4, 3) == -1);
	assert(mm.init(8, 2) == -1);
	assert(mm.init(8, 3) == 0);
	assert(mm.init(8, 3) == -1);

	p[0] = mm.alloc();
	assert(p[0] != NULL);

	p[1] = mm.alloc();
	assert(p[1] != NULL);

	p[2] = mm.alloc();
	assert(p[2] != NULL);

	p[3] = mm.alloc();
	assert(p[3] != NULL);

	mm.recycle(p[0]);
	mm.recycle(p[1]);
	mm.recycle(p[2]);
	mm.recycle(p[3]);

	p[4] = mm.alloc();
	assert(p[4] != NULL);
	assert(p[4] != p[0]);

	p[5] = mm.alloc();
	assert(p[5] != NULL);
	assert(p[5] != p[1]);

	p[6] = mm.alloc();
	assert(p[6] != NULL);
	assert(p[6] != p[2]);

	p[7] = mm.alloc();
	assert(p[7] != NULL);
	assert(p[7] != p[3]);

	p[8] = mm.alloc();
	assert(p[8] != NULL);

	mm.clear();

	p[9] = mm.alloc();
	assert(p[9] != NULL);

	p[10] = mm.alloc();
	assert(p[10] != NULL);

	mm.destroy();

	puts("test case 1: ok~");
}

static void test_case_2()
{
	void *p[32] = {NULL};

	EL_MM_Block mm;
	assert(mm.init(4, 3) == -1);
	assert(mm.init(8, 2) == -1);
	assert(mm.init(8, 3) == 0);
	assert(mm.init(8, 3) == -1);

	p[0] = mm.alloc();
	assert(p[0] != NULL);
	mm.recycle(p[0]);

	assert(mm.init(8, 3) == -1);

	puts("test case 2: ok~");
}

static void test_case_3()
{
	EL_MM_Block mm;
	assert(mm.init(10, 3) == -1);

	puts("test case 3: ok~");
}

void test_mm_block()
{
	test_case_1();
	test_case_2();
	test_case_3();

	puts("test mm block is finished~");
}
