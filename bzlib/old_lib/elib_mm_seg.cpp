/*
// =====================================================================================
// 
//       Filename:  elib_mm_seg.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  09/02/2013 05:03:30 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "elib_mm_seg.hpp"

int EL_MM_Seg::init(size_t seg_size_pow, size_t seg_max_pow)
{
	if (seg_size_pow > 1<<30) {
		err_printf("seg_size_power out of bound(30)\n");
		return -1;
	}

	if (seg_max_pow > 1<<20) {
		err_printf("seg_max_power out of bound(20)\n");
		return -1;
	}

	if (mem_list) {
		err_printf("can't init twice\n");
		return -1;
	}

	seg_size = 1 << seg_size_pow;
	seg_max = 1 << seg_max_pow;
	current = 0;
	
	mem_list = new(std::nothrow) struct mem_seg_node[seg_max];
	if (mem_list == NULL) {
		sys_perror("mem_list");
		return -1;
	}

	memset(mem_list, 0, sizeof(struct mem_seg_node) * seg_max);

	mem_list[0].head = new(std::nothrow) char[seg_size];
	if (mem_list[0].head == NULL) {
		sys_perror("mem_list[0]");
		return -1;
	}

	return 0;
}

void* EL_MM_Seg::alloc(size_t size)
{
	if (mem_list == NULL) {
		err_printf("please init first\n");
		return NULL;
	}

	locker.lock();
	if (current + 1 == seg_max) {
		err_printf("out of segment max\n");
		locker.unlock();
		return NULL;
	}

	if (mem_list[current].offset + size > seg_size) {
		if (size > seg_size) {
			err_printf("alloc size bigger than segmeng size\n");
			locker.unlock();
			return NULL;
		}

		++current;

		if (mem_list[current].head == NULL) {
			mem_list[current].head = new(std::nothrow) char[seg_size];
			if (mem_list[current].head == NULL) {
				sys_perror("new segment");
				locker.unlock();
				return NULL;
			}
		}
		mem_list[current].offset = 0;
	}

	char *p = mem_list[current].head + mem_list[current].offset;
	mem_list[current].offset += size;

	locker.unlock();
	return (void*)p;
}

void EL_MM_Seg::clear()
{
	locker.lock();

	++current;

	if (mem_list)
		mem_list[0].offset = 0;

	for(size_t i=1; i<current; ++i) {
		if (mem_list[i].head) {
			mem_list[i].offset = 0;
		}
	}

	current = 0;

	locker.unlock();
}

void EL_MM_Seg::destroy()
{
	clear();

	for (size_t i=0; i<seg_max; ++i) {
		if (mem_list && mem_list[i].head) {
			delete[] mem_list[i].head;
			mem_list[i].head = NULL;
			mem_list[i].offset = 0;
		}
	}

	if (mem_list) {
		delete[] mem_list;
		mem_list = NULL;
	}
}

