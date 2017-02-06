/*
// =====================================================================================
// 
//       Filename:  elib_hash.hpp
// 
//    Description:  本版本最初打算使用无锁链表的方法，基本思路：
//                  在每个Node内放置一个引用计数，一个冻结标志和一个删除标志
//                  对于节点的访问，每次引用计数原子加一，经过后或读取后引用计数原子减一
//                  对于节点的插入(A->C)，先冻结A节点，然后插入B节点，原子修改A的指针(A->B->C)
//                  对于节点的删除(A->B->C)，先冻结A节点，再冻结B节点，然后删除B节点(A->C)
//            缺陷：这样做可以保证所有的读不被干扰，但是所有的读操作都必须反复修改引用计数（加完再减）
//                  受CPU的Cache一致性策略(EMSI协议)影响，这样做将导致cache的灾难
//                  当多CPU访问同一内存页时，将导致Cache频繁过期，反复刷新，不停的回写内存
//                  读操作本来不需要修改内存，但是由于引用计数不停的修改，内存不得不做大量无意义的回写
//            结论：由于Hash冲体的概率本来就低，即使无锁链表可以提高效率，也不值得花这么高的编程代价
//                  况且，上述方法反而会严重影响效率，所以最终决定只需给每个桶加锁即可
//
// 
//        Version:  1.0
//        Created:  10/26/2013 07:08:33 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "elib_util.hpp"
#include "elib_lock.hpp"
#include "elib_mm_block.hpp"

// #define HASH_VARIABLE_VALUE

#ifndef  _ELIB_HASH_H_
#define  _ELIB_HASH_H_

/* 
// =====================================================================================
//        Class:  EL_Hash
//  Description:  拉链法实现散列表
//                      默认（无宏）: value为一个指针，查找数据时，直接返回指针
//
//             HASH_VARIABLE_VALUE宏: 如果value的内容大于一个指针长度时，必须定义该宏，
//                                    定义了该宏后，查找函数find将无法使用，取而代之
//                                    的是pop和push函数
//
//                HASH_EARSE_BLOCK宏: 如果对Hash表有严格的读写顺序要求，即一旦删除成功，
//                                    那么之后所有读操作的结果都不得返回删除前的无效值，
//                                    则需要定义该宏。
//                                    非阻塞式删除将会导致下面的问题：
//                                    read key ok -> erase key ok -> use key out of date
//
//                              注意: 如果要定义HASH_EARSE_BLOCK宏，必须先定义HASH_VARIABLE_VALUE宏
//                                    否则HASH_EARSE_BLOCK不会发挥任何作用
//                                    这意味着，默认无宏的情况下，也会出现使用过期数据的现象
//
//              HASH_THREAD_UNSAFE宏: 定义改宏，将散列表变为线程不安全版本
//                                    线程不安全版本将极大的提高程序执行效率
//
//                          并发性能: 默认 > HASH_VARIABLE_VALUE > HASH_VARIABLE_VALUE + HASH_EARSE_BLOCK
//
//                      关于内存分配: 内存分配器默认存储1G数据，如果预计内存使用量超过1G时，应适当调整代码
//                                    经验值：数据超过千万 且 value_size>64byte时，就应该考虑修改内存分配容量了
// =====================================================================================
*/
class EL_Hash
{
private:
	typedef struct hash_node {
		struct hash_node *next;
		ulong key;
#ifndef HASH_VARIABLE_VALUE
		void* value;	// 64-bit指针可以通过压缩减少内存占用
#else
		volatile uint ref_cnt;	// 节点被引用的次数
		char value[1];	// 定义变长结构体
#endif
	}Node;

	class Iterator
	{
	public:
		Iterator();
		Iterator(EL_Hash *p);
		~Iterator();
		Iterator(const Iterator &copy);
		Iterator& operator=(const Iterator &it);

		const void* operator*();

		Iterator& operator++();	// ++n
		Iterator  operator++(int);	// n++

		bool operator==(const Iterator &it);
		bool operator!=(const Iterator &it);
		bool operator==(const Iterator &it)const;
		bool operator!=(const Iterator &it)const;

		ulong key();
		const void* value();

		/* 反复使用一个迭代器时，需要先调用clean()才能进行下一次迭代 */
		void clean();

		EL_Hash *pobj;
		size_t index;
		Node **pos;
	};				// ----------  end of struct EL_Hash_Iterator  ----------
	// ==================== OUTER TYPEDEF =======================================
public:
	enum lock_style {
		LOCK_READ = 0,
		LOCK_INSERT,
		LOCK_ERASE,
		LOCK_VOID
	};

	typedef Iterator iterator;

public:
	// ====================  LIFECYCLE     =======================================
	EL_Hash();
	~EL_Hash();

	// ====================  INTERFACE     =======================================

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  init
	//  Description:  初始化Hash表
	//   Parameters:  bucket_size: hash表桶的个数
	//                key_max: 最大允许存储多少个节点
	//                value_size: value的大小，默认为8（即一个指针的空间）
	//  ReturnValue:  成功返回0，失败返回-1
	// =====================================================================================
	*/
	int init(size_t bucket_size, size_t key_max, size_t value_size = 8);

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  destroy
	//  Description:  销毁hash表
	// =====================================================================================
	*/
	void destroy();
	
	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  clean
	//  Description:  清除所有元素，但保留所有的空间（为重复使用）
	//   Parameters:  
	//  ReturnValue:  成功返回0，失败返回-1
	// =====================================================================================
	*/
	int clean();

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  insert
	//  Description:  向Hash中插入一条数据
	//   Parameters:  key: 映射的key值
	//                value: 映射的value值，如果是一段缓冲区，则给定首地址
	//  ReturnValue:  成功返回0，失败返回-1
	// =====================================================================================
	*/
	int insert(ulong key, void *value);

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  erase
	//  Description:  从Hash中删除一条数据
	//   Parameters:  key: 映射的key值
	//  ReturnValue:  成功返回0，失败返回-1
	// =====================================================================================
	*/
	int erase(ulong key);

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  erase
	//  Description:  使用迭代器从Hash中删除一条数据，执行删除完毕之后，迭代器指向自动指向
	//                下一个有效节点
	//                另外，erase不支持多线程遍历删除，强行使用有死锁的风险
	//   Parameters:  it: hash迭代器
	//  ReturnValue:  成功返回0，失败返回-1
	// =====================================================================================
	*/
	int erase(iterator &it);

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  find
	//  Description:  查找数据，只能用于value为指针时，或指定非线程安全时使用
	//   Parameters:  key: 映射的key值
	//  ReturnValue:  映射的value值，没有找到返回NULL
	// =====================================================================================
	*/
	const void* find(ulong key);

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  pop
	//  Description:  获取数据，只能用于value为char[]时使用
	//   Parameters:  key: 映射的key值
	//  ReturnValue:  映射的value值，没有找到返回NULL
	// =====================================================================================
	*/
	const void* pop(ulong key);

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  push
	//  Description:  归还数据，只能用于value为char[]时使用
	//   Parameters:  value: 映射的value值，即pop()取出的指针
	//  ReturnValue:  
	// =====================================================================================
	*/
	int push(const void *value);

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  begin
	//  Description:  返回指向第一个有效数据的迭代器
	// =====================================================================================
	*/
	iterator begin();

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  end
	//  Description:  返回最后迭代器，end()所返回的迭代器指向最后节点的下一个，不可访问
	// =====================================================================================
	*/
	iterator& end();

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  size
	//  Description:  返回当前Hash中存储的元素个数
	// =====================================================================================
	*/
	size_t size();

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  max_size
	//  Description:  返回Hash允许的最大元素个数
	// =====================================================================================
	*/
	size_t max_size();

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  bucket_count
	//  Description:  返回Hash分桶的个数
	// =====================================================================================
	*/
	size_t bucket_count();

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  is_empty
	//  Description:  判断Hash是否为空
	// =====================================================================================
	*/
	bool is_empty();

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  is_full
	//  Description:  判断Hash是否已满
	// =====================================================================================
	*/
	bool is_full();

private:
	// ==================== INNER TYPEDEF =======================================
	typedef struct {
		/* 
		 * 没有将lock做成独立数组，而是和Node指针放在一起的原因有三个：
		 * 1、逻辑更清楚，lock就是锁一个桶的，自然应该放在每个桶里
		 * 2、访问桶时，自然cache住了lock，将桶和锁分则会增大的cache miss的概率
		 * 3、一般来说，连续存储对于独立访问可以提高cache命中率，但是对于共享数据来说，反而会导致更多的cache miss
		 *    这是因为受cache一致性策略(EMSI协议)的影响，多个cpu访问一个频繁写的内存，将使得n-1个cpu频繁cache过期
		 *    所以尽量时每个节点自己的数据集中，节点间的数据远离，有利于并发访问
		 */
		Node *head;
		volatile uint locker;
	}Head;

	// ==================== PRIVATE METHOD =======================================

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  lock
	//  Description:  给指定桶使用指定方式加锁
	//                删除锁是对插入锁的进一步限制，只能在已经挂插入锁的情况下再挂删除锁
	//                特别注意：绝对不能直接挂删除锁，删除锁于插入锁是不互斥的
	// =====================================================================================
	*/
	void lock(size_t index, int style);
#ifndef HASH_THREAD_UNSAFE
#endif

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  unlock
	//  Description:  对指定桶使用指定方式进行解锁
	// =====================================================================================
	*/
	void unlock(size_t index, int style);
#ifndef HASH_THREAD_UNSAFE
#endif

	// ====================  DATA MEMBERS  =======================================
	static const uint LOCK_MARK[4];
	static const uint LOCK_MASK[4];
	static const uint DEL_MARK = 0x80000000;

	size_t max_nodes;
	size_t max_buckets;
	size_t node_count;
	size_t value_size;

	iterator last;

	Head *hash_table;
	EL_MM_Block node_mm;

};		// -----  end of class EL_Hash  -----


#endif   // ----- #ifndef _ELIB_HASH_H_  -----
