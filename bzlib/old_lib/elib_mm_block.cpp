/*
// =====================================================================================
// 
//       Filename:  elib_mm_block.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  09/02/2013 05:03:50 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "elib_mm_block.hpp"

EL_MM_Block::EL_MM_Block()
{
	m_block_size = 0;
	recycle_pointer = NULL;
}

EL_MM_Block::~EL_MM_Block()
{
	destroy();
}

int EL_MM_Block::init(size_t block_size, size_t seg_size_pow, size_t seg_max_pow)
{
	if (block_size < sizeof(void*)) {
		err_printf("block size must contain at least a pointer\n");
		return -1;
	}
	m_block_size = block_size;

	if ((unsigned)1 << seg_size_pow < block_size) {
		err_printf("segmeng size must contain at least a block\n");
		return -1;
	}

	if (recycle_pointer) {
		err_printf("illegal init, someone in used\n");
		return -1;
	}
	recycle_pointer = NULL;

	if (seg_size_pow < 3)
		seg_size_pow = 3;	// at least 8 byte

	return EL_MM_Seg::init(seg_size_pow, seg_max_pow);
}

void* EL_MM_Block::alloc()
{
	while (recycle_pointer) {
		locker.lock();
		if (!recycle_pointer) {
			locker.unlock();
			continue;
		}

		void *p = recycle_pointer;
		recycle_pointer = *(void**)recycle_pointer;

		locker.unlock();
		return p;
	}

	/* 程序运行到这里，有可能上面已经有内存被回收，不过继续开新内存不会造成逻辑错误 */

	return EL_MM_Seg::alloc(m_block_size);
}

void EL_MM_Block::recycle(void* pointer)
{
	locker.lock();
	*(void**)pointer = recycle_pointer;
	recycle_pointer = pointer;
	locker.unlock();
}

size_t EL_MM_Block::block_size()
{
	return m_block_size;
}

void EL_MM_Block::clear()
{
	recycle_pointer = NULL;

	EL_MM_Seg::clear();
}

void EL_MM_Block::destroy()
{
	clear();

	EL_MM_Seg::destroy();
}
