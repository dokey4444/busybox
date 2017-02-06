/*
 * =====================================================================================
 *
 *       Filename:  likely-fibonacci.c
 *
 *    Description:  使用fibonacci数列的计算来讨论likely和unlikely对程序优化的影响
 *                  http://stackoverflow.com/questions/2738835/learning-sample-of-likely-and-unlikely-compiler-hints
 *
 *                  说明：
 *                  gcc -O2 -Dopt= -o fib-nrm likely-fibonacci.c
 *                  gcc -O2 -Dopt=likely -o fib-wrs likely-fibonacci.c
 *                  gcc -O2 -Dopt=unlikely -o fib-opt likely-fibonacci.c
 *                  通过对likely和unlikely进行对比，可以看出likely和unlikely对程序确实会产生一定的影响
 *
 *                  另外：
 *                  该文章还提到了一种通过统计执行情况进行优化的另一个程序优化的做法
 *                  gcc -O2 -Dopt= -fprofile-generate -o fib.prof likely-fibonacci.c
 *                  gcc -O2 -Dopt= -fprofile-use -o fib.good likely-fibonacci.c
 *                  通过-fprofile-generate编译执行，-fprofile-generate参数会使用profile 代码，构建程序的框架。
 *                  程序运行的时候，会记录程序的运行事件，保存的log，用于编译的时候判断分支，inline等。
 *                  程序退出的时候，会dump出所有的数据到*.gcda文件。该文件就是测试运行的记录值。
 *                  然后再次编译的时候使用-fprofile-use参数，gcc 会使用*.gcda的数据，进行代码优化。通常可以显著提高程序的运行效率。
 *
 *                  结论：
 *                  likely和unlikely虽然对编译确实有优化，不过这种优化非常有限，而且需要程序员对编译原理非常熟悉，否则很难达到好的优化效果
 *                  相比之下，使用profile的方法根据统计结果对再编译进行优化，非常有针对性，所以效果非常非常好。
 *                  注意，profile对命令类程序优化效果虽然很好，但是对需要长期执行的服务程序很难进行执行统计。进一步来说，profile的优化受到运行时数据影响很大，优化非常有针对性，不太容易做通用性的优化。
 *
 *        Version:  1.0
 *        Created:  11/16/2014 07:15:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include <assert.h>

#define likely(x) __builtin_expect((x),1)
#define unlikely(x) __builtin_expect((x),0)

uint64_t fib(uint64_t n)
{
	if (opt(n == 0 || n == 1)) {
		return n;
	} else {
		return fib(n - 2) + fib(n - 1);
	}
}

int main(int argc, char **argv)
{
	int i, max = 45;
	clock_t tm;

	if (argc == 2) {
		max = atoi(argv[1]);
		assert(max > 0);
	} else {
		assert(argc == 1);
	}

	tm = -clock();
	for (i = 0; i <= max; ++i)
		printf("fib(%d) = %" PRIu64 "\n", i, fib(i));
	tm += clock();

	printf("Time elapsed: %.3fs\n", (double)tm / CLOCKS_PER_SEC);
	return 0;
}
