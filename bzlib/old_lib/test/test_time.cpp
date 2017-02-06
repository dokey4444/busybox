/*
// =====================================================================================
// 
//       Filename:  test_time.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  10/25/2013 09:16:24 AM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "../elib_util.hpp"
#include "../elib_time.hpp"

/* test time diff */
static void test_case_1()
{
	EL_Time timer;

	puts("----------------------------------------------------------------");

	timer.set_time_begin();
	usleep(200);
	timer.set_time_end();
	printf("diff time: %ld ms\n", timer.diff_time_ms());
	printf("diff time format: %s\n", timer.diff_time_format());

	puts("test case 1: ok~");
}

/* test date_string */
static void test_case_2()
{
	char buf[64] = {0};
	char *p = NULL;
	EL_Time timer;

	puts("----------------------------------------------------------------");

	printf("current time: %s", timer.date_string());
	printf("current time: %s", timer.date_string(-1));
	p = timer.date_string(-1, buf);
	assert(p == buf);
	printf("current time: %s", p);
	printf("user time: %s", timer.date_string(0));

	puts("test case 2: ok~");
}

/* test date_format */
static void test_case_3()
{
	EL_Time timer;
	char buf[128] = {0};
	long len = 0;

	puts("----------------------------------------------------------------");

	printf("%s\n", timer.date_format("YY-M-D h:m:s W"));
	printf("%s\n", timer.date_format("YYYY-MM-DD hh:mm:ss W"));
	printf("%s\n", timer.date_format("YYYY-MMEN-DDEN hh:mm:ss WEN"));
	printf("%s\n", timer.date_format("YYYY-MMEN-DDEN hh:mm:ss WCN"));

	printf("%s\n", timer.date_format("YY-M-D h12:m:s W"));
	printf("%s\n", timer.date_format("YY-M-D h12:m:s hmen W"));
	printf("%s\n", timer.date_format("YYYY-MM-DD hh12:mm:ss W"));
	printf("%s\n", timer.date_format("YYYY-MM-DD hmcn hh12:mm:ss W"));

	len = timer.date_format("SCNS|YY年 M月 D日 hmcn SDGT|h12时 m分 s秒 周WCN", buf, -1);
	printf("len: %ld: %s\n", len, buf);
	len = timer.date_format("SCNT|YYYY年 MM月 DD日 hmcn SDGT|hh时 mm分 ss秒 星期WCN", buf, -1);
	printf("len: %ld: %s\n", len, buf);

	printf("%s\n", timer.date_format("YY-YSM-D h:m:s W"));

	puts("test case 3: ok~");
}

static void test_case_4()
{
	EL_Time timer;
	char buf[128] = {0};
	long len = 0;

	puts("----------------------------------------------------------------");

	printf("%s\n", timer.time_format(93784, "dd days - hh:mm:ss"));
	printf("%s\n", timer.time_format(93784, "dd2 days - hh2:mm2:ss2"));
	printf("%s\n", timer.time_format(93784, "dd3 days - hh3:mm3:ss3"));
	printf("%s\n", timer.time_format(93784, "dd4 days - hh4:mm4:ss4"));

	printf("%s\n", timer.time_format(0, "NZ(8)dd days - NZ(1)hh:NZ(1)mm:NZ(1)ss"));
	printf("%s\n", timer.time_format(0, "dd4 days - hh4:mm4:ss4"));

	printf("%s\n", timer.time_format(93784, "ALLdd days - ALLhh:ALLmm:ALLss"));

	len = timer.time_format(93784, "dd days hmNA- NZhh:mm:ss", buf);
	printf("len: %ld, %s\n", len, buf);

	puts("test case 4: ok~");
}

void test_time()
{
	test_case_1();
	test_case_2();
	test_case_3();
	test_case_4();

	puts("test time is finished~");
}
