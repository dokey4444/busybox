/*
// =====================================================================================
// 
//       Filename:  likely.cpp
// 
//    Description:  测试likely和unlikely的性能效果
//                  http://blog.chinaunix.net/uid-24774106-id-2183034.html
//
//                  说明：
//                    likely版本：g++ -o test_ok   likely.cpp -lrt -O2
//                  unlikely版本：g++ -o test_err  likely.cpp -lrt -O2
//                  无likely版本：g++ -o test_null likely.cpp -lrt -O2
//                  ./test_err  100000000
//                  ./test_ok   100000000
//                  ./test_null 100000000
//                  通过执行程序，可以发现test_ok的时间短于test_err
//                  可以看出，likely和unlikely的使用对于程序的确存在影响
//                  但是，由于CPU还有Branch Target Buffer，这个BTB可以根据上次执行的转移，来预测本次转移。所以上面的两个程序，执行时间差的并不多。
//                  疑问：通过测试发现test_null短于test_ok
//
//                  从汇编的角度来看
//                  objdump -d test_err  > objdump_err
//                  objdump -d test_ok   > objdump_ok
//                  objdump -d test_null > objdump_null
//                  通过分析汇编(find malloc)，发现objdump_ok和objdump_err的主要区别在于
//                  程序会把likely的代码紧跟在if语句之后，会把unlikely代码远离if语句
//                  疑问：test_null同样也会把malloc代码紧跟在if后面，但是却以一种很奇怪的方式排列
//                        而且，test_null的性能与test_ok不相上下，甚至有时还会高于test_ok
//
//                  结论：
//                  1、likely确实可以在某种程度上提高程序执行性能
//                  2、除非你非常清楚likely的原理和产生的影响，否则不要轻易使用，把这个工作交给编译器优化也许更好
//
//        Version:  1.0
//        Created:  07/13/2013 11:31:39 AM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/
#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<time.h>

#define TIMES (10000000)
#define BILLION (1000000000)
#define MILLION 1000000

#define likely(x)      __builtin_expect(x,1)
#define unlikely(x)    __builtin_expect(x, 0)

int main(int argc,char *argv[])
{
	int i;
	int j;
	int *array = NULL;
	struct timespec tpstart;
	struct timespec tpend;
	unsigned long long timeif;
	unsigned long long timeif_func;
	struct timeval tv;
	if(argc != 2)
	{
		printf("usage: ./test times\n");
		return -1;
	}

	int times = atoi(argv[1]);

	if(clock_gettime(CLOCK_MONOTONIC,&tpstart))
	{
		fprintf(stderr,"Fail to get start time for NULL\n");
		return -1;
	}
	for(i = 0;i<times;i++)
	{
		;
	}

	if(clock_gettime(CLOCK_MONOTONIC,&tpend))
	{
		fprintf(stderr,"Fail to get start time for NULL\n");
		return -1;
	}

	timeif = MILLION*(tpend.tv_sec - tpstart.tv_sec)+
		(tpend.tv_nsec-tpstart.tv_nsec)/1000;

	printf("%d BLANK OP 's total time is %llu us\n",times,(timeif));


	if(clock_gettime(CLOCK_MONOTONIC,&tpstart))
	{
		fprintf(stderr,"Fail to get start time for branch \n");
		return -1;
	}
	for(i = 0;i<times;i++)
	{
		j = rand();
		if(likely(j > 1))
		{
			array = (int*)malloc(1000*sizeof(int));
			if(array)
				free(array);
		}
		else
		{
			gettimeofday(&tv,NULL);
		}
	}

	if(clock_gettime(CLOCK_MONOTONIC,&tpend))
	{
		fprintf(stderr,"Fail to get start time for branch\n");
		return -1;
	}

	timeif_func =MILLION*(tpend.tv_sec - tpstart.tv_sec)+
		(tpend.tv_nsec-tpstart.tv_nsec)/1000;

	printf("%d FUNCTION's with right prediction cost time is %llu us\n",
			times,(timeif_func));	
}

