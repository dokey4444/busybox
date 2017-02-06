/*
// =====================================================================================
// 
//       Filename:  test_hash.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  10/30/2013 08:01:09 AM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "../elib_util.hpp"
#include "../elib_hash.hpp"

/* value为普通void*类型 */
#ifndef HASH_VARIABLE_VALUE
/* 单线程-简单初始化、插入、查找、删除 */
static void test_case_10()
{
	EL_Hash hash;

	hash.init(10, 100);
	assert(hash.size() == 0);
	assert(hash.max_size() == 100);
	assert(hash.bucket_count() == 10);
	assert(hash.is_empty());
	assert(!hash.is_full());

	assert(hash.insert(1, (void*)1) == 0);
	assert(hash.size() == 1);
	assert(!hash.is_empty());
	assert(!hash.is_full());
	assert(hash.find(1) == (void*)1);

	assert(hash.insert(1, (void*)2) == 1);
	assert(hash.size() == 1);
	assert(!hash.is_empty());
	assert(!hash.is_full());
	assert(hash.find(1) == (void*)2);

	assert(hash.erase(1) == 0);
	assert(hash.size() == 0);
	assert(hash.is_empty());
	assert(!hash.is_full());
	assert(hash.find(1) == NULL);

	assert(hash.erase(1) == 1);
	assert(hash.size() == 0);
	assert(hash.is_empty());
	assert(!hash.is_full());
	assert(hash.find(1) == NULL);

	hash.destroy();
	puts("test case 10: ok~");
}

/* 单线程-多次插入 */
static void test_case_11()
{
	EL_Hash hash;
	hash.init(10, 100);

	assert(hash.size() == 0);
	assert(hash.is_empty());
	assert(!hash.is_full());

	for (int i=0; i<100; ++i) {
		assert(hash.insert(i, (void*)(long)i) == 0);
	}

	assert(hash.size() == 100);
	assert(!hash.is_empty());
	assert(hash.is_full());

	for (int i=0; i<100; ++i) {
		assert(hash.find(i) == (void*)(long)i);
	}
	assert(hash.insert(100, (void*)(long)100) == -1);

	puts("test case 11: ok~");
}

/* 单线程-多次删除 */
static void test_case_12()
{
	EL_Hash hash;
	hash.init(10, 100);

	assert(hash.size() == 0);
	assert(hash.is_empty());
	assert(!hash.is_full());

	for (int i=0; i<100; ++i) {
		assert(hash.insert(i, (void*)(long)i) == 0);
	}

	assert(hash.size() == 100);
	assert(!hash.is_empty());
	assert(hash.is_full());

	for (int i=0; i<100; ++i) {
		assert(hash.find(i) == (void*)(long)i);
		assert(hash.insert(i, (void*)(long)(i+100)) == 1);
	}
	for (int i=0; i<100; ++i) {
		assert(hash.find(i) == (void*)(long)(i+100));
	}

	for (int i=99; i>=0; --i) {
		assert(hash.erase(i) == 0);
		assert(hash.find(i) == NULL);
		assert(hash.erase(i) == 1);
	}

	assert(hash.size() == 0);
	assert(hash.is_empty());
	assert(!hash.is_full());

	puts("test case 12: ok~");
}

/* 单线程-迭代器 */
static void test_case_13()
{
	EL_Hash hash;
	hash.init(10, 100);

	/* 插入满数据 */
	for (int i=0; i<100; ++i) {
		assert(hash.insert(i, (void*)(long)i) == 0);
	}

	/* 单独使用begin不允许持续挂锁 */
	hash.begin();

	/* 测试++n语句（满数据） */
	EL_Hash::iterator it = hash.begin();
	int i = 0;
	for ( ; it != hash.end(); ++it, ++i) {
		int x = i % 10;
		int y = i / 10;
		int n = x*10 + y;
		assert((long)*it == n);
	}
	/* 迭代结束，自动放锁 */
	assert(hash.erase(99) == 0);	// 如果最后的锁不放，删除会导致死锁
	assert(hash.insert(99, (void*)(long)99) == 0);
	assert(i == 100);
	assert(it == hash.end());
	/* 迭代结束后，确保安全自增 */
	++it; ++it; ++it; ++it;
	assert(it == hash.end());

	/* 测试n++语句（满数据） */
	{
		EL_Hash::iterator tmp = hash.begin();
		it.clean();	// 如果迭代已经结束，那么clean不是必须的
		it = tmp++;
	}
	assert(it == hash.begin());
	i = 0;
	for ( ; it != hash.end(); it++, i++) {
		int x = i % 10;
		int y = i / 10;
		int n = x*10 + y;
		assert((long)*it == n);
	}

	/* 迭代结束，自动放锁 */
	assert(hash.erase(99) == 0);	// 如果最后的锁不放，删除会导致死锁
	assert(hash.insert(99, (void*)(long)99) == 0);
	assert(i == 100);
	assert(it == hash.end());
	/* 迭代结束后，确保安全自增 */
	it++; it++; it++; it++;
	assert(it == hash.end());

	/* 迭代到一半推出，不允许遗留锁 */
	i = 0;
	for ( EL_Hash::iterator it = hash.begin(); i<10; ++it, ++i) {
		++it;
		it++;
	}
	i = 0;
	/* 反复使用同一变量，必须在使用前清零 */
	it.clean();
	it = hash.begin();
	it.clean();
	it = hash.begin();
	it.clean();

	/* 多次clean是无害的 */
	it.clean();
	it.clean();
	it.clean();

	/* 清除满数据，添加稀疏数据 */
	it = hash.begin();
	for (int i=0; it != hash.end(); ++i) {
		int x = i % 10;
		int y = i / 10;
		int n = x*10 + y;
		assert((long)*it == (long)n);
		assert(hash.erase(it) == 0);
	}
	it.clean();

	assert(hash.insert(3, (void*)3) == 0);
	assert(hash.insert(13, (void*)13) == 0);
	assert(hash.insert(23, (void*)23) == 0);
	assert(hash.insert(33, (void*)33) == 0);
	assert(hash.insert(43, (void*)43) == 0);
	assert(hash.insert(53, (void*)53) == 0);
	assert(hash.insert(63, (void*)63) == 0);
	assert(hash.insert(73, (void*)73) == 0);
	assert(hash.insert(83, (void*)83) == 0);
	assert(hash.insert(93, (void*)93) == 0);
	assert(hash.insert(4, (void*)4) == 0);
	assert(hash.insert(14, (void*)14) == 0);
	assert(hash.insert(24, (void*)24) == 0);
	assert(hash.insert(34, (void*)34) == 0);
	assert(hash.insert(44, (void*)44) == 0);
	assert(hash.insert(5, (void*)5) == 0);
	assert(hash.insert(7, (void*)7) == 0);
	assert(hash.insert(8, (void*)8) == 0);

	it = hash.begin();
	assert((long)*it == 3); ++it;
	assert((long)*it == 13); it++;
	assert((long)*it == 23); ++it;
	assert((long)*it == 33); it++;
	assert((long)*it == 43); ++it;
	assert((long)*it == 53); it++;
	assert((long)*it == 63); ++it;
	assert((long)*it == 73); it++;
	assert((long)*it == 83); ++it;
	assert((long)*it == 93); it++;
	assert((long)*it == 4); ++it;
	assert((long)*it == 14); it++;
	assert((long)*it == 24); ++it;
	assert((long)*it == 34); it++;
	assert((long)*it == 44); ++it;
	assert((long)*it == 5); ++it;
	assert((long)*it == 7); it++;
	assert((long)*it == 8); ++it;
	assert(it == hash.end());
	++it; it++; ++it; it++;
	assert(it == hash.end());

	puts("test case 13: ok~");
}

/* 单线程性能测试（通过HASH_THREAD_UNSAFE进行有无锁的性能比较） */
static void test_case_14()
{
	EL_Hash hash;
	hash.init(1000000, 10000000);

	/* 插入满数据 */
	for (int i=0; i<10000000; ++i) {
		assert(hash.insert(i, (void*)(long)i) == 0);
	}

	long c = 0;
	long n;
	for (int i=0; i<10000000; ++i) {
		n = (long)hash.find(i);
		c += n;
	}

	hash.clean();

	puts("test case 14: ok~");
}

static void* thread_insert(void *arg)
{
	EL_Hash *hash = (EL_Hash*)arg;

	for (int i=0; i<2500000; ++i) {
		int n = rand();
		assert(hash->insert(n, (void*)(long)n) >= 0);
	}

	return NULL;
}

static void* thread_erase(void *arg)
{
	EL_Hash *hash = (EL_Hash*)arg;

	for (int i=0; i<10000000; ++i) {
		int n = rand();
		assert(hash->erase(n) >= 0);
	}

	return NULL;
}

static void* thread_find(void *arg)
{
	EL_Hash *hash = (EL_Hash*)arg;

	for (int i=0; i<10000000; ++i) {
		int n = rand();
		int m = 0;
		m = (int)(long)hash->find(n);
		if (m)
			assert(m == n);
	}
	return NULL;
}

static void* thread_traverse(void *arg)
{
	EL_Hash *hash = (EL_Hash*)arg;

	EL_Hash::iterator it = hash->begin();
	for ( ; it != hash->end(); ++it) {
		assert((long)it.key() == (long)it.value());
	}

	return NULL;
}

/* 多线程插入+多线程查找+多线程遍历 */
static void test_case_15()
{
	EL_Hash hash;
	hash.init(1000000, 10000000);

	pthread_t pt_insert[4];
	pthread_t pt_find[8];
	pthread_t pt_traverse[4];

	puts("starting insert");
	for (int i=0; i<4; ++i) {
		pthread_create(&pt_insert[i], NULL, thread_insert, &hash);
	}

	sleep(1);
	puts("starting find");
	for (int i=0; i<8; ++i) {
		pthread_create(&pt_find[i], NULL, thread_find, &hash);
	}

	for (int i=0; i<4; ++i) {
		sleep(1);
		printf("starting traverse %d\n", i+1);
		pthread_create(&pt_traverse[i], NULL, thread_traverse, &hash);
	}

	for (int i=0; i<4; ++i) {
		pthread_join(pt_insert[i], NULL);
	}
	for (int i=0; i<8; ++i) {
		pthread_join(pt_find[i], NULL);
	}
	for (int i=0; i<4; ++i) {
		pthread_join(pt_traverse[i], NULL);
	}

	hash.destroy();

	puts("test case 15: ok~");
}

/* 多线程插入+多线程删除+多线程查找 */
static void test_case_16()
{
	EL_Hash hash;
	hash.init(1000000, 10000000);

	pthread_t pt_insert[4];
	pthread_t pt_find[8];
	pthread_t pt_erase[2];

	puts("starting insert");
	for (int i=0; i<4; ++i) {
		pthread_create(&pt_insert[i], NULL, thread_insert, &hash);
	}

	sleep(1);
	puts("starting find");
	for (int i=0; i<8; ++i) {
		pthread_create(&pt_find[i], NULL, thread_find, &hash);
	}

	for (int i=0; i<2; ++i) {
		sleep(1);
		printf("starting erase %d\n", i+1);
		pthread_create(&pt_erase[i], NULL, thread_erase, &hash);
	}

	for (int i=0; i<4; ++i) {
		pthread_join(pt_insert[i], NULL);
	}
	for (int i=0; i<8; ++i) {
		pthread_join(pt_find[i], NULL);
	}
	for (int i=0; i<2; ++i) {
		pthread_join(pt_erase[i], NULL);
	}

	hash.destroy();

	puts("test case 16: ok~");
}

/* 多线程插入+迭代器删除+多线程遍历+多线程查找 */
static void test_case_17()
{
	EL_Hash hash;
	hash.init(1000000, 10000000);

	pthread_t pt_insert[4];
	pthread_t pt_find[8];
	pthread_t pt_traverse[4];

	puts("starting insert");
	for (int i=0; i<4; ++i) {
		pthread_create(&pt_insert[i], NULL, thread_insert, &hash);
	}

	sleep(1);
	puts("starting find");
	for (int i=0; i<8; ++i) {
		pthread_create(&pt_find[i], NULL, thread_find, &hash);
	}

	for (int i=0; i<4; ++i) {
		sleep(1);
		printf("starting traverse %d\n", i+1);
		pthread_create(&pt_traverse[i], NULL, thread_traverse, &hash);
	}

	sleep(1);
	puts("starting erase by traverse");
	for (EL_Hash::iterator it = hash.begin(); it != hash.end(); ) {
		if (((long)*it)%2) {
			hash.erase(it);
		} else {
			++it;
		}
	}

	for (int i=0; i<4; ++i) {
		pthread_join(pt_insert[i], NULL);
	}
	for (int i=0; i<8; ++i) {
		pthread_join(pt_find[i], NULL);
	}
	for (int i=0; i<4; ++i) {
		pthread_join(pt_traverse[i], NULL);
	}

	hash.destroy();

	puts("test case 17: ok~");
}
#endif	// -----  HASH_VARIABLE_VALUE  -----

/* value为一段buffer时，需要宏定义: HASH_VARIABLE_VALUE */
/* value变长的基础上，需要阻塞式删除，需要宏定义; HASH_EARSE_BLOCK */
#ifdef HASH_VARIABLE_VALUE
/* 单线程-简单初始化、插入、查找、删除 */
static void test_case_20()
{
	EL_Hash hash;
	char buf[1024] = {0};
	const void *value;

	hash.init(1000, 10000, 1024);

	buf[0] = 1;
	assert(hash.insert(1, buf) == 0);
	value = hash.pop(1);
	assert((long)(((char*)value)[0]) == 1);
	assert(hash.push(value) == 0);
	assert(hash.push(value) == -1);

	buf[0] = 2;
	assert(hash.insert(1, buf) == 1);
	value = hash.pop(1);
	assert((long)(((char*)value)[0]) == 2);
	assert(hash.push(value) == 0);
	assert(hash.push(value) == -1);

	assert(hash.erase(1) == 0);
	value = hash.pop(1);
	assert(value == NULL);

	assert(hash.erase(1) == 1);
	value = hash.pop(1);
	assert(value == NULL);

	hash.destroy();
	puts("test case 20: ok~");
}

/* 单线程-多次插入 */
static void test_case_21()
{
	EL_Hash hash;
	char buf[1024] = {0};
	hash.init(10, 100, 1024);

	assert(hash.size() == 0);
	assert(hash.is_empty());
	assert(!hash.is_full());

	for (int i=0; i<100; ++i) {
		*(long*)buf = i;
		assert(hash.insert(i, buf) == 0);
	}

	assert(hash.size() == 100);
	assert(!hash.is_empty());
	assert(hash.is_full());

	for (int i=0; i<100; ++i) {
		char *value = (char*)hash.pop(i);
		assert(*(long*)value == i);
		assert(hash.push(value) == 0);
	}
	assert(hash.insert(100, (void*)(long)100) == -1);

	puts("test case 21: ok~");
}

/* 单线程-多次删除 */
static void test_case_22()
{
	EL_Hash hash;
	char buf[1024] = {0};
	hash.init(10, 100, 1024);

	assert(hash.size() == 0);
	assert(hash.is_empty());
	assert(!hash.is_full());

	for (int i=0; i<100; ++i) {
		*(long*)buf = i;
		assert(hash.insert(i, buf) == 0);
	}

	assert(hash.size() == 100);
	assert(!hash.is_empty());
	assert(hash.is_full());

	for (int i=0; i<100; ++i) {
		char *value = (char*)hash.pop(i);
		assert(*(long*)value == i);
		*(long*)value = (*(long*)value) + 1;
		assert(hash.insert(i, value) == 1);	// 定义了HASH_EARSE_BLOCK这里将会导致死锁
		assert(hash.push(value) == 0);
		assert(hash.push(value) == -1);	// 此时绝对不允许两次push，这将导致极端严重的内存错误
	}
	for (int i=0; i<100; ++i) {
		char *value = (char*)hash.pop(i);
		assert(*(long*)value == i+1);
		assert(hash.push(value) == 0);
		assert(hash.push(value) == -1);
	}

	for (int i=99; i>=0; --i) {
		assert(hash.erase(i) == 0);
		char *value = (char*)hash.pop(i);
		assert(value == NULL);
		assert(hash.push(value) == 1);
		assert(hash.erase(i) == 1);
	}

	assert(hash.size() == 0);
	assert(hash.is_empty());
	assert(!hash.is_full());

	puts("test case 22: ok~");
}

/* 单线程-迭代器 */
static void test_case_23()
{
	EL_Hash hash;
	char buf[1024] = {0};
	hash.init(10, 100, 1024);

	/* 插入满数据 */
	for (int i=0; i<100; ++i) {
		*(long*)buf = i;
		assert(hash.insert(i, buf) == 0);
	}

	/* 单独使用begin不允许持续挂锁 */
	hash.begin();

	/* 测试++n语句（满数据） */
	EL_Hash::iterator it = hash.begin();
	int i = 0;
	for ( ; it != hash.end(); ++it, ++i) {
		int x = i % 10;
		int y = i / 10;
		int n = x*10 + y;
		assert(*(long*)*it == n);
	}
	/* 迭代结束，自动放锁 */
	assert(hash.erase(99) == 0);	// 即使最后的锁不放，删除也不应该导致死锁
	*(long*)buf = 99;
	assert(hash.insert(99, buf) == 0);
	assert(i == 100);
	assert(it == hash.end());
	/* 迭代结束后，确保安全自增 */
	++it; ++it; ++it; ++it;
	assert(it == hash.end());

	/* 测试n++语句（满数据） */
	{
		EL_Hash::iterator tmp = hash.begin();
		it.clean();	// 如果迭代已经结束，那么clean不是必须的
		it = tmp++;
	}
	assert(it == hash.begin());
	i = 0;
	for ( ; it != hash.end(); it++, i++) {
		int x = i % 10;
		int y = i / 10;
		int n = x*10 + y;
		assert(*(long*)*it == n);
	}

	/* 迭代结束，自动放锁 */
	assert(hash.erase(99) == 0);	// 即使最后的锁不放，删除也不应该导致死锁
	*(long*)buf = 99;
	assert(hash.insert(99, buf) == 0);
	assert(i == 100);
	assert(it == hash.end());
	/* 迭代结束后，确保安全自增 */
	it++; it++; it++; it++;
	assert(it == hash.end());

	/* 迭代到一半推出，不允许遗留锁 */
	i = 0;
	for ( EL_Hash::iterator it = hash.begin(); i<10; ++it, ++i) {
		++it;
		it++;
	}
	i = 0;
	/* 反复使用同一变量，必须在使用前清零 */
	it.clean();
	it = hash.begin();
	it.clean();
	it = hash.begin();
	it.clean();

	/* 多次clean是无害的 */
	it.clean();
	it.clean();
	it.clean();

	/* 清除满数据，添加稀疏数据 */
	it = hash.begin();
	for (int i=0; it != hash.end(); ++i) {
		int x = i % 10;
		int y = i / 10;
		int n = x*10 + y;
		assert(*(long*)*it == (long)n);
		assert(hash.erase(it) == 0);
	}
	it.clean();

	*(long*)buf = 3;	assert(hash.insert(3, buf) == 0);
	*(long*)buf = 13;	assert(hash.insert(13, buf) == 0);
	*(long*)buf = 23;	assert(hash.insert(23, buf) == 0);
	*(long*)buf = 33;	assert(hash.insert(33, buf) == 0);
	*(long*)buf = 43;	assert(hash.insert(43, buf) == 0);
	*(long*)buf = 53;	assert(hash.insert(53, buf) == 0);
	*(long*)buf = 63;	assert(hash.insert(63, buf) == 0);
	*(long*)buf = 73;	assert(hash.insert(73, buf) == 0);
	*(long*)buf = 83;	assert(hash.insert(83, buf) == 0);
	*(long*)buf = 93;	assert(hash.insert(93, buf) == 0);
	*(long*)buf = 4;	assert(hash.insert(4, buf) == 0);
	*(long*)buf = 14;	assert(hash.insert(14, buf) == 0);
	*(long*)buf = 24;	assert(hash.insert(24, buf) == 0);
	*(long*)buf = 34;	assert(hash.insert(34, buf) == 0);
	*(long*)buf = 44;	assert(hash.insert(44, buf) == 0);
	*(long*)buf = 5;	assert(hash.insert(5, buf) == 0);
	*(long*)buf = 7;	assert(hash.insert(7, buf) == 0);
	*(long*)buf = 8;	assert(hash.insert(8, buf) == 0);

	it = hash.begin();
	assert(*(long*)*it == 3); ++it;
	assert(*(long*)*it == 13); it++;
	assert(*(long*)*it == 23); ++it;
	assert(*(long*)*it == 33); it++;
	assert(*(long*)*it == 43); ++it;
	assert(*(long*)*it == 53); it++;
	assert(*(long*)*it == 63); ++it;
	assert(*(long*)*it == 73); it++;
	assert(*(long*)*it == 83); ++it;
	assert(*(long*)*it == 93); it++;
	assert(*(long*)*it == 4); ++it;
	assert(*(long*)*it == 14); it++;
	assert(*(long*)*it == 24); ++it;
	assert(*(long*)*it == 34); it++;
	assert(*(long*)*it == 44); ++it;
	assert(*(long*)*it == 5); ++it;
	assert(*(long*)*it == 7); it++;
	assert(*(long*)*it == 8); ++it;
	assert(it == hash.end());
	++it; it++; ++it; it++;
	assert(it == hash.end());

	puts("test case 23: ok~");
}

/* 单线程性能测试（通过HASH_THREAD_UNSAFE进行有无锁的性能比较） */
static void test_case_24()
{
	EL_Hash hash;
	char buf[8] = {0};
	hash.init(1000000, 10000000, 8);

	/* 插入满数据 */
	for (int i=0; i<10000000; ++i) {
		*(long*)buf = i;
		assert(hash.insert(i, buf) == 0);
	}

	long c = 0;
	long n;
	for (int i=0; i<10000000; ++i) {
		char *value = (char*)hash.pop(i);
		n = *(long*)value;
		c += n;
		assert(hash.push(value) == 0);

//		n = (long)hash.find(i);	// 在线程不安全情况下，也可以使用find，这样效率更高
//		c += n;
	}

	hash.clean();

	puts("test case 24: ok~");
}

static void* thread_insert(void *arg)
{
	EL_Hash *hash = (EL_Hash*)arg;

	char buf[8] = {0};

	for (int i=0; i<2500000; ++i) {
		*(long*)buf = rand();
		assert(hash->insert(*(long*)buf, buf) >= 0);
	}

	return NULL;
}

static void* thread_erase(void *arg)
{
	EL_Hash *hash = (EL_Hash*)arg;

	for (int i=0; i<10000000; ++i) {
		int n = rand();
		assert(hash->erase(n) >= 0);
	}

	return NULL;
}

static void* thread_find(void *arg)
{
	EL_Hash *hash = (EL_Hash*)arg;


	for (int i=0; i<10000000; ++i) {
		int n = rand();
		const void *value;
		value = hash->pop(n);
		if (value)
			assert(*(long*)value == n);
		hash->push(value);
	}
	return NULL;
}

static void* thread_traverse(void *arg)
{
	EL_Hash *hash = (EL_Hash*)arg;

	EL_Hash::iterator it = hash->begin();
	for ( ; it != hash->end(); ++it) {
		assert((long)it.key() == *(long*)it.value());
	}

	return NULL;
}

/* 多线程插入+多线程查找+多线程遍历 */
static void test_case_25()
{
	EL_Hash hash;
	hash.init(1000000, 10000000);

	pthread_t pt_insert[4];
	pthread_t pt_find[8];
	pthread_t pt_traverse[4];

	puts("starting insert");
	for (int i=0; i<4; ++i) {
		pthread_create(&pt_insert[i], NULL, thread_insert, &hash);
	}

	sleep(1);
	puts("starting find");
	for (int i=0; i<8; ++i) {
		pthread_create(&pt_find[i], NULL, thread_find, &hash);
	}

	for (int i=0; i<4; ++i) {
		sleep(1);
		printf("starting traverse %d\n", i+1);
		pthread_create(&pt_traverse[i], NULL, thread_traverse, &hash);
	}

	for (int i=0; i<4; ++i) {
		pthread_join(pt_insert[i], NULL);
	}
	for (int i=0; i<8; ++i) {
		pthread_join(pt_find[i], NULL);
	}
	for (int i=0; i<4; ++i) {
		pthread_join(pt_traverse[i], NULL);
	}

	hash.destroy();

	puts("test case 25: ok~");
}

/* 多线程插入+多线程删除+多线程查找 */
static void test_case_26()
{
	EL_Hash hash;
	hash.init(1000000, 10000000);

	pthread_t pt_insert[4];
	pthread_t pt_find[8];
	pthread_t pt_erase[2];

	puts("starting insert");
	for (int i=0; i<4; ++i) {
		pthread_create(&pt_insert[i], NULL, thread_insert, &hash);
	}

	sleep(1);
	puts("starting find");
	for (int i=0; i<8; ++i) {
		pthread_create(&pt_find[i], NULL, thread_find, &hash);
	}

	for (int i=0; i<2; ++i) {
		sleep(1);
		printf("starting erase %d\n", i+1);
		pthread_create(&pt_erase[i], NULL, thread_erase, &hash);
	}

	for (int i=0; i<4; ++i) {
		pthread_join(pt_insert[i], NULL);
	}
	for (int i=0; i<8; ++i) {
		pthread_join(pt_find[i], NULL);
	}
	for (int i=0; i<2; ++i) {
		pthread_join(pt_erase[i], NULL);
	}

	hash.destroy();

	puts("test case 26: ok~");
}

/* 多线程插入+迭代器删除+多线程遍历+多线程查找 */
static void test_case_27()
{
	EL_Hash hash;
	hash.init(1000000, 10000000);

	pthread_t pt_insert[4];
	pthread_t pt_find[8];
	pthread_t pt_traverse[4];

	puts("starting insert");
	for (int i=0; i<4; ++i) {
		pthread_create(&pt_insert[i], NULL, thread_insert, &hash);
	}

	sleep(1);
	puts("starting find");
	for (int i=0; i<8; ++i) {
		pthread_create(&pt_find[i], NULL, thread_find, &hash);
	}

	for (int i=0; i<4; ++i) {
		sleep(1);
		printf("starting traverse %d\n", i+1);
		pthread_create(&pt_traverse[i], NULL, thread_traverse, &hash);
	}

	sleep(1);
	puts("starting erase by traverse");
	for (EL_Hash::iterator it = hash.begin(); it != hash.end(); ) {
		if (((long)*it)%2) {
			hash.erase(it);
		} else {
			++it;
		}
	}

	for (int i=0; i<4; ++i) {
		pthread_join(pt_insert[i], NULL);
	}
	for (int i=0; i<8; ++i) {
		pthread_join(pt_find[i], NULL);
	}
	for (int i=0; i<4; ++i) {
		pthread_join(pt_traverse[i], NULL);
	}

	hash.destroy();

	puts("test case 27: ok~");
}
#endif	// -----  HASH_VARIABLE_VALUE  -----

void test_hash()
{
#ifndef HASH_VARIABLE_VALUE
	test_case_10();
	test_case_11();
	test_case_12();
	test_case_13();
	test_case_14();
	test_case_15();
	test_case_16();
	test_case_17();
#else
	test_case_20();
	test_case_21();
	test_case_22();
	test_case_23();
	test_case_24();
	test_case_25();
	test_case_26();
	test_case_27();
#endif	// -----  HASH_VARIABLE_VALUE  -----
	puts("test hash is finished~");
}

