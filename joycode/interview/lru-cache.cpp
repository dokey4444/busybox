/*
// =====================================================================================
// 
//       Filename:  lru-cache.cpp
// 
//    Description:  多线程安全的LRUCache实现
// 
//        Version:  1.0
//        Created:  2017/10/05 16时47分16秒
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
#include <string>


// =====================================================================================
//        Class:  LRUCache
//  Description:  
// =====================================================================================
template typename<T>
class LRUCache
{
	struct CacheNode {
		// 将hash_code作为Node的falg，来确保Node在各个线程中的可见性
		std::atomic<uint64_t> hash_code;	// 利用uint的比较代替string的比较
		std::string key;
		std::shared_ptr<T value>;	// 返回value的指针，节省get时的拷贝构造
		struct CacheNode *next;		// 用于组织hash table
		struct CacheNode *before;	// 用于组织访问序列的双向链表
		struct CacheNode *after;	// 用于组织访问序列的双向链表
	};

	struct ArrayNode {
		ArrayNode() : lock(ATOMIC_FLAG_INIT), head(nullptr) {}
		std::atomic_flag lock;	// 自旋锁
		struct CacheNode head;	// 哈希链表首地址
	};
public:
	// ====================  LIFECYCLE     =======================================
	LRUCache(size_t array_size, size_t key_max)
	{
	}

	~LRUCache()
	{
	}

	// ====================  INTERFACE     =======================================

private:
	// ==================== PRIVATE METHOD =======================================

	// ====================  DATA MEMBERS  =======================================
	const size_t array_size;
	const size_t key_max;
	std::vector<struct ArrayNode>

};		// -----  end of class LRUCache  -----


/* 
// ===  FUNCTION  ======================================================================
//         Name:  main
//  Description:  
// =====================================================================================
*/
int main(int argc, char *argv[])
{
	std::cout << "Hello World" << std::endl;
	return EXIT_SUCCESS;
}				// ----------  end of function main  ----------

