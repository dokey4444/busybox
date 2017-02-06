/*
// =====================================================================================
// 
//       Filename:  elib_hash.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  10/26/2013 07:18:30 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "elib_hash.hpp"

#ifdef LOCK
make it compile error
#endif

#ifdef UNLOCK
make it compile error
#endif

#ifndef HASH_THREAD_UNSAFE
#define LOCK(index, style) lock((index), (style))
#define UNLOCK(index, style) unlock((index), (style))
#else
#define LOCK(index, style) ((void)0)
#define UNLOCK(index, style) ((void)0)
#endif

// =====================================================================================
//        Class:  Iterator
//  Description:  EL_Hash表的迭代器
// =====================================================================================

EL_Hash::Iterator::Iterator()
{
	this->pobj = NULL;
	this->index = 0;
	this->pos = NULL;
}

EL_Hash::Iterator::Iterator(EL_Hash *p)
{
	this->pobj = p;
	this->index = 0;
	this->pos = NULL;
}

EL_Hash::Iterator::Iterator(const EL_Hash::Iterator &it)
{
	*this = it;
}

EL_Hash::Iterator::~Iterator()
{
	clean();
}

EL_Hash::Iterator& EL_Hash::Iterator::operator=(const EL_Hash::Iterator &it)
{
#ifndef HASH_THREAD_UNSAFE
	if (it.pobj) {
		it.pobj->lock(it.index, LOCK_READ);
	}
#endif	// -----  HASH_THREAD_UNSAFE  -----
	this->pobj = it.pobj;
	this->index = it.index;
	this->pos = it.pos;
	return *this;
}

const void* EL_Hash::iterator::operator*()
{
	if (*pos==NULL)
		return NULL;

	return (*pos)->value;
}

EL_Hash::Iterator& EL_Hash::Iterator::operator++()	// ++n
{
	/* 由于遍历会给持续整个桶加读锁，所以不必再为每个节点加引用计数 */
	if (*pos) {
		pos = &(*pos)->next;
	} 

	if (*pos==NULL && index < pobj->max_buckets) {
#ifndef HASH_THREAD_UNSAFE
		pobj->unlock(index, LOCK_READ);
#endif  // -----  HASH_THREAD_UNSAFE  -----
		++index;

#ifndef HASH_THREAD_UNSAFE
		pobj->lock(index, LOCK_READ);
#endif  // -----  HASH_THREAD_UNSAFE  -----
		while (index < pobj->max_buckets) {
			if (pobj->hash_table[index].head) {
				pos = &pobj->hash_table[index].head;
				break;
			} else {
#ifndef HASH_THREAD_UNSAFE
				pobj->unlock(index, LOCK_READ);
#endif  // -----  HASH_THREAD_UNSAFE  -----
				++index;
#ifndef HASH_THREAD_UNSAFE
				pobj->lock(index, LOCK_READ);
#endif  // -----  HASH_THREAD_UNSAFE  -----
			}
		}
	}

	return *this;
}

EL_Hash::Iterator EL_Hash::Iterator::operator++(int)	// n++
{
	EL_Hash::Iterator temp(*this);
	++*this;
	return temp;
}

bool EL_Hash::Iterator::operator==(const EL_Hash::Iterator &it)
{
	if (this->index == it.index && *this->pos == *it.pos)
		return true;
	else
		return false;
}

bool EL_Hash::Iterator::operator!=(const EL_Hash::Iterator &it)
{
	return !operator==(it);
}

bool EL_Hash::Iterator::operator==(const EL_Hash::Iterator &it)const
{
	if (this->index == it.index && *this->pos == *it.pos)
		return true;
	else
		return false;
}

bool EL_Hash::Iterator::operator!=(const EL_Hash::Iterator &it)const
{
	return !operator==(it);
}

ulong EL_Hash::Iterator::key()
{
	if (*pos==NULL)
		return ULONG_MAX;

	return (*pos)->key;
}

const void* EL_Hash::Iterator::value()
{
	if (*pos==NULL)
		return NULL;

	return (*pos)->value;
}

void EL_Hash::Iterator::clean()
{
#ifndef HASH_THREAD_UNSAFE
	if (pobj) {
		pobj->unlock(index, LOCK_READ);
//		printf("iterator clean index: %lu\tlock: %x\n",index , pobj->hash_table[index].locker);
	}
#endif  // -----  HASH_THREAD_UNSAFE  -----
	this->pobj = NULL;
	this->index = 0;
	this->pos = NULL;
}

// =====================================================================================
//        Class:  EL_Hash
//  Description:  拉链法实现散列表
// =====================================================================================

#ifndef HASH_THREAD_UNSAFE
/* 将锁的状态分为：读、插入和删除3种状态 */
const uint EL_Hash::LOCK_MARK[4] = {
	0x00000000,	// 0000, read	// 读没有标志位，完全看引用计数
	0x40000000,	// 0100, insert	// 插入位为次高比特
	0x80000000,	// 1000, erase	// 删除位为最高比特
	0x00000000};	// 0000, void	// 空标志，表示加空锁
/* 对于无锁Hash来讲，读和插入并不冲突，可以并行 */
const uint EL_Hash::LOCK_MASK[4] = {
	0x80000000,	// 1000, read	// 读只与删除互斥
	0xc0000000,	// 1100, insert	// 插入独占但克读
	0x3fffffff,	// 0011, erase	// 删除独占且不可读
	0x00000000};	// 0000, void	// 任何情况下都可以加空锁
#endif  // -----  HASH_THREAD_UNSAFE  -----

EL_Hash::EL_Hash()
{
	node_count = 0;
	hash_table = NULL;
}

EL_Hash::~EL_Hash()
{
	destroy();
}

int EL_Hash::init(size_t bucket_size, size_t key_max, size_t value_size)
{
	if (bucket_size == 0) return -1;
	if (key_max == 0) return -1;
	if (value_size == 0) return -1;

	this->max_buckets = bucket_size;
	this->max_nodes = key_max;
	this->value_size = value_size;

#ifndef HASH_VARIABLE_VALUE
	assert(value_size <= sizeof(void*));
	if (node_mm.init(sizeof(Node)) < 0) {
		err_printf("memory node_mm init failure!\n");
		return -1;
	}
#else
	assert(value_size >= sizeof(void*));
	if (node_mm.init(sizeof(Node) - 1 + value_size) < 0) {
		err_printf("memory node_mm init failure!\n");
		return -1;
	}
#endif	// -----  HASH_VARIABLE_VALUE  -----

	hash_table = new(std::nothrow) Head[max_buckets+1];	// 最后一个作为留给last迭代器用于防止越界
	if (hash_table == NULL) {
		sys_perror("new hash table");
		return -1;
	}
	memset(hash_table, 0, sizeof(Head)*(max_buckets+1));

	node_count = 0;

	last.pobj = this;
	last.index = max_buckets;
	last.pos = &hash_table[max_buckets].head;

	return 0;
}		// -----  end of function init  -----

void EL_Hash::destroy()
{
	if (node_count)
		log_printf("warning: destrpy when there are %lu nodes exist!\n", node_count);
	if (hash_table) {
#ifdef PROJECT_TEST
		for (size_t i=0; i<max_buckets; ++i) {
			if (hash_table[i].locker) {
				log_printf("warning: locker of index %lu is locked: %u!\n", i, hash_table[i].locker);
#ifdef PROJECT_DEBUG
				abort();
#endif	// -----  PROJECT_DEBUG  -----
			}
		}
		if (hash_table[max_buckets].locker) {
			log_printf("warning: locker of end is locked!\n");
		}
#endif	// -----  PROJECT_TEST  -----
		delete[] hash_table;
		hash_table = NULL;
	}

	node_mm.destroy();

	last.pobj = NULL;
}		// -----  end of function destroy  -----

int EL_Hash::clean()
{
	for (iterator it = begin(); it != end(); ) {
		if (erase(it) < 0)
			return -1;
	}

	return 0;
}		// -----  end of function clean  -----

int EL_Hash::insert(ulong key, void *value)
{
	size_t index = key % max_buckets;

	LOCK(index, LOCK_INSERT);

	Node **p = &hash_table[index].head;
	Node *tmp = NULL;
	for ( ; *p; p=&(*p)->next) {
		if ((*p)->key^key)
			continue;

		/* replace same key */
#ifndef HASH_VARIABLE_VALUE
		(*p)->value = value;	/* 值的原地替换不需要加写锁，赋值本身就是原子的 */
#else
#ifdef HASH_THREAD_UNSAFE
		memcpy((*p)->value, value, value_size);
#else
		// 经验：超过100K长度的memcpy平均时间在10ms以上（超过一个的时间片）
		tmp = (Node*)node_mm.alloc();
		if (tmp == NULL) {
			UNLOCK(index, LOCK_INSERT);
			return -1;
		}
		tmp->key = key;
		memcpy(tmp->value, value, value_size);
		tmp->ref_cnt = 0;
		tmp->next = (*p)->next;

		Node *old = *p;
		LOCK(index, LOCK_ERASE);
#ifdef HASH_EARSE_BLOCK
		while ((*p)->ref_cnt != 0);
		*p = tmp;
		UNLOCK(index, LOCK_ERASE);
		node_mm.recycle(old);
#else
		for ( ; ; ) {
			uint cnt = (*p)->ref_cnt;
			if (cnt == 0) {
				*p = tmp;
				UNLOCK(index, LOCK_ERASE);
				node_mm.recycle(old);
				break;
			} else {
				if (atom_cas(&(*p)->ref_cnt, cnt, cnt|DEL_MARK)) {
					*p = tmp;
					UNLOCK(index, LOCK_ERASE);
					break;
				}
			}
		}
#endif	// -----  HASH_EARSE_BLOCK  -----
#endif	// -----  HASH_THREAD_UNSAFE  -----
#endif	// -----  HASH_VARIABLE_VALUE  -----
		UNLOCK(index, LOCK_INSERT);
		return 1;
	}

	if (node_count == max_nodes) {
		UNLOCK(index, LOCK_INSERT);
		return -1;
	}

	/* there is no same key */
	tmp = (Node*)node_mm.alloc();
	if (tmp == NULL) {
		UNLOCK(index, LOCK_INSERT);
		return -1;
	}
	tmp->key = key;
#ifndef HASH_VARIABLE_VALUE
	tmp->value = value;
#else
	memcpy(tmp->value, value, value_size);
	tmp->ref_cnt = 0;
#endif	// -----  HASH_VARIABLE_VALUE  -----
	// 插入到最后
	tmp->next = NULL;
	*p = tmp;

	++node_count;

	UNLOCK(index, LOCK_INSERT);

	return 0;
}		// -----  end of function insert  -----

int EL_Hash::erase(ulong key)
{
	size_t index = key % max_buckets;

	LOCK(index, LOCK_INSERT);

	Node **p = &hash_table[index].head;
	for ( ; *p; p=&(*p)->next) {
		if ((*p)->key!=key)
			continue;

		Node *old = *p;
		LOCK(index, LOCK_ERASE);	// 删除采用写优先的策略
#ifndef HASH_VARIABLE_VALUE
		*p = (*p)->next;
		UNLOCK(index, LOCK_ERASE);
		node_mm.recycle(old);
#else
#ifdef HASH_THREAD_UNSAFE
		*p = (*p)->next;
		node_mm.recycle(old);
#else
#ifdef HASH_EARSE_BLOCK
		while ((*p)->ref_cnt != 0);
		*p = (*p)->next;
		UNLOCK(index, LOCK_ERASE);
		node_mm.recycle(old);
#else
		for ( ; ; ) {
			uint cnt = (*p)->ref_cnt;
			if (cnt == 0) {
				*p = (*p)->next;
				UNLOCK(index, LOCK_ERASE);
				node_mm.recycle(old);
				break;
			} else {
				if (atom_cas(&(*p)->ref_cnt, cnt, cnt|DEL_MARK)) {
					*p = (*p)->next;
					UNLOCK(index, LOCK_ERASE);
					break;
				}
			}
		}
#endif	// -----  HASH_EARSE_BLOCK  -----
#endif	// -----  HASH_THREAD_UNSAFE  -----
#endif	// -----  HASH_VARIABLE_VALUE  -----

		--node_count;

		UNLOCK(index, LOCK_INSERT);
		return 0;
	}

	UNLOCK(index, LOCK_INSERT);

	return 1;
}		// -----  end of function erase  -----

int EL_Hash::erase(iterator &it)
{
	if (it == end()) {
		err_printf("can't erase by end iterator!\n");
		return -1;
	}

	Node *old = *it.pos;

	LOCK(it.index, LOCK_INSERT);
	UNLOCK(it.index, LOCK_READ);
	LOCK(it.index, LOCK_ERASE);
#ifndef HASH_VARIABLE_VALUE
	*it.pos = (*it.pos)->next;
	UNLOCK(it.index, LOCK_ERASE);
	node_mm.recycle(old);
#else
#ifdef HASH_THREAD_UNSAFE
	*it.pos = (*it.pos)->next;
	node_mm.recycle(old);
#else
#ifdef HASH_EARSE_BLOCK
	while ((*it.pos)->ref_cnt != 0);
	*it.pos = (*it.pos)->next;
	UNLOCK(it.index, LOCK_ERASE);
	node_mm.recycle(old);
#else
	for ( ; ; ) {
		uint cnt = (*it.pos)->ref_cnt;
		if (cnt == 0) {
			*it.pos = (*it.pos)->next;
			UNLOCK(it.index, LOCK_ERASE);
			node_mm.recycle(old);
			break;
		} else {
			if (atom_cas(&(*it.pos)->ref_cnt, cnt, cnt|DEL_MARK)) {
				*it.pos = (*it.pos)->next;
				UNLOCK(it.index, LOCK_ERASE);
				break;
			}
		}
	}
#endif	// -----  HASH_EARSE_BLOCK  -----
#endif	// -----  HASH_THREAD_UNSAFE  -----
#endif	// -----  HASH_VARIABLE_VALUE  -----

	--node_count;

	LOCK(it.index, LOCK_READ);
	UNLOCK(it.index, LOCK_INSERT);

	if (*it.pos == NULL) {
		++it;
	}

	return 0;
}		// -----  end of function erase  -----

const void* EL_Hash::find(ulong key)
{
#ifdef HASH_VARIABLE_VALUE
#ifndef HASH_THREAD_UNSAFE
	abort();
#endif  // -----  HASH_THREAD_UNSAFE  -----
#endif	// -----  HASH_VARIABLE_VALUE  -----
	size_t index = key % max_buckets;

	LOCK(index, LOCK_READ);

	Node **p = &hash_table[index].head;
	for ( ; *p; p=&(*p)->next) {
		if ((*p)->key!=key)
			continue;
		const void *ret = (*p)->value;
		UNLOCK(index, LOCK_READ);
		return ret;
	}

	UNLOCK(index, LOCK_READ);

	return NULL;
}		// -----  end of function find  -----
#ifdef HASH_VARIABLE_VALUE
const void* EL_Hash::pop(ulong key)
{
	size_t index = key % max_buckets;

	LOCK(index, LOCK_READ);

	Node **p = &hash_table[index].head;
	for ( ; *p; p=&(*p)->next) {
		if ((*p)->key!=key)
			continue;
		const void *ret = (*p)->value;
#ifndef HASH_THREAD_UNSAFE
		atom_add_and_fetch(&(*p)->ref_cnt, 1);
#endif  // -----  HASH_THREAD_UNSAFE  -----
		UNLOCK(index, LOCK_READ);
//		printf("node: %p,\tref_cnt: %u\n", *p, (*p)->ref_cnt);
		return ret;
	}

	UNLOCK(index, LOCK_READ);

	return NULL;
}		// -----  end of function pop  -----

int EL_Hash::push(const void *value)
{
	if (value == NULL)
		return 1;

#ifndef HASH_THREAD_UNSAFE
	long offset = (long)(&((Node*)0)->value);
	Node *p = (Node*)(((char*)value) - offset);

	if (!(p->ref_cnt&~DEL_MARK))
		return -1;

	uint cnt = atom_sub_and_fetch(&p->ref_cnt, 1);
	if (cnt == DEL_MARK) {
		node_mm.recycle(p);
	}
#endif  // -----  HASH_THREAD_UNSAFE  -----
//	printf("node: %p,\tref_cnt: %u\n", p, p->ref_cnt);

	return 0;
}		// -----  end of function push  -----
#endif	// -----  HASH_VARIABLE_VALUE  -----
EL_Hash::iterator EL_Hash::begin()
{
	EL_Hash::iterator it(this);
	LOCK(0, LOCK_READ);

	it.pos = &hash_table[0].head;
	if (*it.pos == NULL)
		++it;

	return it;
}		// -----  end of function begin  -----

EL_Hash::iterator& EL_Hash::end()
{
	return last;
}		// -----  end of function end  -----

size_t EL_Hash::size()
{
	return node_count;
}		// -----  end of function size  -----

size_t EL_Hash::max_size()
{
	return max_nodes;
}		// -----  end of function max_size  -----

size_t EL_Hash::bucket_count()
{
	return max_buckets;
}		// -----  end of function bucket_count  -----

bool EL_Hash::is_empty()
{
	return node_count == 0;
}		// -----  end of function is_empty  -----

bool EL_Hash::is_full()
{
	return node_count == max_nodes;
}		// -----  end of function is_full  -----

// ==================== PRIVATE METHOD =======================================

#ifndef HASH_THREAD_UNSAFE
void EL_Hash::lock(size_t index, int style)
{
	volatile uint &locker = hash_table[index].locker;
	uint oflag, nflag;
	do {
		/* 自己实现自旋有可能导致解锁线程的饥饿，加锁线程的空转 */
		while (locker & LOCK_MASK[style]);	// spin lock
		oflag = locker;
		nflag = style==LOCK_READ ? oflag+1 : oflag;
	} while (!atom_cas(&locker, oflag, nflag|LOCK_MARK[style]));
//	printf("index: %lu\tlock: %x\n",index , locker);
}		// -----  end of function lock  -----

void EL_Hash::unlock(size_t index, int style)
{
	volatile uint &locker = hash_table[index].locker;
	uint flag;
	if (style == LOCK_READ) {	// 解读锁不需要关心当前状态，流程保证正确性
		atom_sub_and_fetch(&locker, 1);
	} else {	// 解插入锁不能破坏当前的读状态
		do { flag = locker;
		} while (!atom_cas(&locker, flag, flag&~LOCK_MARK[style]));
	}
//	printf("index: %lu\tunlock: %x\n",index , locker);
}		// -----  end of function unlock  -----
#endif	// -----  HASH_THREAD_UNSAFE  -----

#ifdef LOCK
#undef LOCK
#endif

#ifdef UNLOCK
#undef UNLOCK
#endif

