/*
// =====================================================================================
// 
//       Filename:  test_mm_seg.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  09/02/2013 05:40:25 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "elib_util.hpp"
#include "elib_mm_seg.hpp"

static void test_case_1()
{
	void *p[32] = {NULL};

	EL_MM_Seg mm;
	assert(mm.init(2, 2) == 0);
	assert(mm.init(2, 2) == -1);

	p[0] = mm.alloc(5);
	assert(p[0] == NULL);

	p[0] = mm.alloc(1);
	assert(p[0] != NULL);
	p[1] = mm.alloc(2);
	assert(p[1] != NULL);
	assert(p[1] == (char*)p[0]+1);

	p[3] = mm.alloc(3);
	assert(p[3] != NULL);
	assert(p[3] != (char*)p[0]+3);	// not always true

	p[4] = mm.alloc(4);
	assert(p[4] != NULL);

	p[5] = mm.alloc(4);
	assert(p[5] != NULL);

	p[6] = mm.alloc(4);
	assert(p[6] == NULL);

	mm.clear();

	p[7] = mm.alloc(4);
	assert(p[7] != NULL);

	p[8] = mm.alloc(4);
	assert(p[8] != NULL);

	p[9] = mm.alloc(4);
	assert(p[9] != NULL);

	p[10] = mm.alloc(4);
	assert(p[10] != NULL);

	p[11] = mm.alloc(4);
	assert(p[11] == NULL);
	
	mm.destroy();

	puts("test case 1: ok~");
}

void test_mm_seg()
{
	test_case_1();

	puts("test mm seg is finished~");
}
