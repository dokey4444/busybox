/*
// =====================================================================================
// 
//       Filename:  test_bitmap.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  12/31/2013 09:05:53 AM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "../elib_util.hpp"
#include "../elib_bitmap.hpp"

/* 测试bitmap提供的static方法 */
static void test_case_1()
{
	int n = 0;

	EL_Bitmap::set<int>(n, 31);
	assert((uint)n == 0x80000000);

	EL_Bitmap::set<int>(n, 0);
	assert((uint)n == 0x80000001);

	EL_Bitmap::set<int>(n, 7);
	assert((uint)n == 0x80000081);

	EL_Bitmap::unset<int>(n, 0);
	assert((uint)n == 0x80000080);

	int r = 0;

	r = (int)EL_Bitmap::scan_forward((ulong)n);
	assert(r == 7);

	r = (int)EL_Bitmap::scan_reverse((ulong)(uint)n);	// 有符号类型直接强转ulong会导致最高位1从31变为63
	assert(r == 31);

	r = (int)EL_Bitmap::scan_forward_zero((uint)~n);
	assert(r == 7);

	r = (int)EL_Bitmap::scan_reverse_zero(~((ulong)(uint)n));	// 先取反再强转时会导致高位自动补0
	assert(r == 31);

	puts("test case 1: ok~");
}

/* 测试位图的基本用法 */
static void test_case_2()
{
	EL_Bitmap bm;
	long n = 1024;
	bm.init(n);

	bm.clean();
	for (int i=0; i<n; ++i) {
		assert(bm[i] == false);
	}

	bm.fill();
	for (int i=0; i<n; ++i) {
		assert(bm[i] == true);
	}

	bm.clean();
	bm.set(0UL);
	bm.set(37UL);
	bm.set(99UL);
	bm.set(512UL);
	for (int i=0; i<n; ++i) {
		if (i==0 || i==37 || i==99 ||  i==512) {
			assert(bm[i] == true);
		} else {
			assert(bm[i] == false);
		}
	}

	bm.unset(0);
	bm.unset(99);
	for (int i=0; i<n; ++i) {
		if (i==37 ||  i==512) {
			assert(bm[i] == true);
		} else {
			assert(bm[i] == false);
		}
	}

	bm.destroy();

	puts("test case 2: ok~");
}

/* 测试位图按位扫描的用法 */
static void test_case_3()
{
	EL_Bitmap bm;
	char buf[1<<20];
	bm.init(1000000, buf);
	bm.clean();

	assert(bm.scan_forward() == -1);
	assert(bm.scan_reverse() == -1);

	bm.set(256);
	assert(bm.scan_forward() == 256);
	bm.set(156);
	assert(bm.scan_forward() == 156);
	bm.set(56);
	assert(bm.scan_forward() == 56);

	assert(bm.scan_reverse() == 256);
	bm.set(356);
	assert(bm.scan_reverse() == 356);
	bm.set(456);
	assert(bm.scan_reverse() == 456);
	bm.set(999999);
	assert(bm.scan_reverse() == 999999);

	bm.fill();

	assert(bm.scan_forward_zero() == -1);
	assert(bm.scan_reverse_zero() == -1);

	bm.unset(256);
	assert(bm.scan_forward_zero() == 256);
	bm.unset(156);
	assert(bm.scan_forward_zero() == 156);
	bm.unset(56);
	assert(bm.scan_forward_zero() == 56);

	assert(bm.scan_reverse_zero() == 256);
	bm.unset(356);
	assert(bm.scan_reverse_zero() == 356);
	bm.unset(456);
	assert(bm.scan_reverse_zero() == 456);
	bm.unset(999999);
	assert(bm.scan_reverse_zero() == 999999);

	puts("test case 3: ok~");
}

/* 测试位图的size不能被64整除时的情况 */
static void test_case_4()
{
	EL_Bitmap bm;
	char buf[1<<20];
	bm.init(37, buf);

	bm.clean();
	assert(bm.scan_forward() == -1);
	assert(bm.scan_reverse() == -1);

	bm.set(30);
	assert(bm[30] == true);
	assert(bm.scan_forward() == 30);
	assert(bm.scan_reverse() == 30);

	bm.fill();
	assert(bm.scan_forward_zero() == -1);
	assert(bm.scan_reverse_zero() == -1);

	bm.unset(30);
	assert(bm[30] == false);
	assert(bm.scan_forward_zero() == 30);
	assert(bm.scan_reverse_zero() == 30);

	bm.init(32760);

	bm.clean();
	assert(bm.scan_forward() == -1);
	assert(bm.scan_reverse() == -1);

	bm.set(15000);
	assert(bm[15000] == true);
	assert(bm.scan_forward() == 15000);
	assert(bm.scan_reverse() == 15000);

	bm.fill();
	assert(bm.scan_forward_zero() == -1);
	assert(bm.scan_reverse_zero() == -1);

	bm.unset(15000);
	assert(bm[15000] == false);
	assert(bm.scan_forward_zero() == 15000);
	assert(bm.scan_reverse_zero() == 15000);

	puts("test case 4: ok~");
}

void test_bitmap()
{
	test_case_1();
	test_case_2();
	test_case_3();
	test_case_4();

	puts("test bitmap is finished~");
}
