/*
// =====================================================================================
// 
//       Filename:  atomic.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  2017/10/03 13时27分04秒
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include <cstdlib>
#include <iostream>

#include <atomic>

// =====================================================================================
//        Class:  SpinLock
//  Description:  利用std::atomic_flag实现自旋锁
// =====================================================================================
class SpinLock
{
public:
	// ====================  LIFECYCLE     =======================================
	SpinLock() : flag(ATOMIC_FLAG_INIT) {};
	~SpinLock() {}

	// ====================  INTERFACE     =======================================
	void lock()
	{
		while (flag.test_and_set(std::memory_order_acquire));
	}

	void unlock()
	{
		flag.clear(std::memory_order_release);
	}

private:
	// ==================== PRIVATE METHOD =======================================

	// ====================  DATA MEMBERS  =======================================
	std::atomic_flag flag;

};		// -----  end of class SpinLock  -----


/* 
// ===  FUNCTION  ======================================================================
//         Name:  main
//  Description:  
// =====================================================================================
*/
int main(int argc, char *argv[])
{
	// 最简单的原子类型，只表示一个bool标记，但与atomic_bool(std::atomic<bool>)是不同的
	// std::atomic_flag对象必须被ATOMIC_FLAG_INIT初始化
	std::atomic_flag flag = ATOMIC_FLAG_INIT;		// init false
	// std::atomic_flag只支持clear()和test_and_set()操作
	flag.clear(std::memory_order_release);			// set flag to false;
	flag.test_and_set(std::memory_order_acquire);	// set flag to true and return false;
	flag.test_and_set(std::memory_order_acquire);	// set flag to true and return true;
	std::cout << "std::atomic_flag ok" << std::endl;

	// std::atomic_flag实现自旋锁
	SpinLock spinlock;
	spinlock.lock();
	spinlock.unlock();

	// 基本原子类型操作
	// 两个atomic变量之间，是不能拷贝构造与拷贝赋值的，因为这种操作破坏了原子性
	std::atomic<bool> b(false);
	bool x;
	x = b.load(std::memory_order_acquire);			// read and return
	std::cout << std::boolalpha << x << std::endl;
	b.store(true, std::memory_order_release);		// just write 
	x = b.exchange(false, std::memory_order_acq_rel);	// read and write
	std::cout << std::boolalpha << x << std::endl;
	// atomic的cas操作
	// 当原子变量与预期值相等时，修改原子变量为目标值，并返回true
	// 当原子变量与预期值不等时，修改预期值为原子变量的值，并返回false
	// compare_exchange_weak()在缺乏cas指令的机器上，可能会出现“伪失败”，
	// 即原子变量与预期值相等，但依旧赋值失败的现象，所以通常需要一个循环
	std::cout << "std::atomic<bool> cas" << std::endl;
	bool expected = false;
	while(!b.compare_exchange_weak(expected, true,
				std::memory_order_release,
				std::memory_order_relaxed) && !expected);
	std::cout << std::boolalpha << expected << std::endl;
	std::cout << std::boolalpha << b << std::endl;
	// compare_exchange_strong()与compare_exchange_weak()功能一致
	// 区别在于compare_exchange_strong不会出现“伪失败”问题
	// 所以compare_exchange_strong()不需要使用循环来进行保护
	b.compare_exchange_strong(expected, false);
	std::cout << std::boolalpha << expected << std::endl;
	std::cout << std::boolalpha << b << std::endl;
	b.compare_exchange_strong(expected, false);
	std::cout << std::boolalpha << expected << std::endl;
	std::cout << std::boolalpha << b << std::endl;
	std::cout << "std::atomic<bool> ok" << std::endl;

	// 查看原子变量是否是lock free的
	struct A { int a[100]; };
	struct B { int x, y; };
	std::cout << std::boolalpha	// 将bool值显示为true or false
		<< "std::atomic<A> is lock free? "
		<< std::atomic<A>{}.is_lock_free() << '\n'
		<< "std::atomic<B> is lock free? "
		<< std::atomic<B>{}.is_lock_free() << '\n';

	std::cout << "Hello World" << std::endl;

	return EXIT_SUCCESS;
}				// ----------  end of function main  ----------

