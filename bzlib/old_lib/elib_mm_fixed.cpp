/*
// =====================================================================================
// 
//       Filename:  elib_mm_fixed.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  12/27/2013 05:02:55 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "elib_util.hpp"
#include "elib_mm_fixed.hpp"
#include "elib_bitmap.hpp"

EL_MM_Fixed::EL_MM_Fixed()
{
	block_size = 0;
	segment_size = 0;
	segments_size = 0;
	resident = 1;

	bm_level = 0;
	for (int i=0; i<4; ++i) {
		alloc_bm[i] = NULL;
	}
	for (int i=0; i<4; ++i) {
		recycle_bm[i] = NULL;
	}

	node_list = NULL;

	cur_index = 0;
	cur_header = NULL;
	cur_offset = 0;
}

EL_MM_Fixed::~EL_MM_Fixed()
{
	destroy();
}

int EL_MM_Fixed::init(uint32_t block_size, uint8_t segment_size_pow, uint8_t segments_size_pow, int32_t resident)
{
	if (block_size < (long)sizeof(void*)) {	// for recycle
		puts("block_size can save a integer at least");
		return -1;
	}

	if (segment_size_pow > 32) {	// less than 4GB per segment
		puts("segment_size_pow must be less than 32");
		return -1;
	}

	if (segments_size_pow > 24) {	// less than 16M segments(4 levels of bitmap pyramid)
		puts("segments_size_pow must be less than 24");
		return -1;
	}

	if (segment_size_pow == 32) {
		this->segment_size = ~0x0;
	} else {
		this->segment_size = 1 << segment_size_pow;
	}

	if (block_size > segment_size) {
		puts("segment can't contain a block");
		return -1;
	}
	this->block_size = block_size;

	this->segments_size = 1 << segments_size_pow;

	if (resident == 0 || resident > this->segments_size) {
		puts("resident is too small or too big");
		return -1;
	}
	this->resident = resident;

	node_list = (struct head*) malloc(segments_size * sizeof(struct head));
	if (node_list == NULL) {
		perror("malloc node_list");
		return -1;
	}

	int size;
	if (segments_size < 64) {			// one level(64 segments)
		size = 1;				// 8B
		bm_level = 1;
	} else if (segments_size < 64*64) {		// two level(4K segments)
		size = 1 + 64;				// 8B + 512B = 520B
		bm_level = 2;
	} else if (segments_size < 64*64*64) {		// three level(256K segments)
		size = 1 + 64 + 64*64;			// 520B + 32KB = 32.5KB
		bm_level = 3;
	} else if (segments_size < 64*64*64*64) {	// four level(16M segments)
		size = 1 + 64 + 64*64 + 64*64*64;	// 32.5KB + 2MB = 2.3MB
		bm_level = 4;
	} else {	// more than 16M segments	// four level(16M segments)
		size = 1 + 64 + 64*64 + 64*64*64;	// 32.5KB + 2MB = 2.3MB
		bm_level = 4;
	}

	/* create recycle bitmap pyramid */
	uint64_t *p = (uint64_t*) malloc(size * sizeof(uint64_t));
	if (p == NULL) {
		perror("malloc bitmap");
		return -1;
	}
	memset(p, 0, size * sizeof(uint64_t));

	alloc_bm[0] = p;
	alloc_bm[1] = p + 1;
	alloc_bm[2] = p + 1 + 64;
	alloc_bm[3] = p + 1 + 64 + 64*64;

	/* create alloc bitmap pyramid */
	p = (uint64_t*) malloc(size * sizeof(uint64_t));
	if (p == NULL) {
		perror("malloc bitmap");
		return -1;
	}
	memset(p, 0, size * sizeof(uint64_t));

	recycle_bm[0] = p;
	recycle_bm[1] = p + 1;
	recycle_bm[2] = p + 1 + 64;
	recycle_bm[3] = p + 1 + 64 + 64*64;

	/* create initialize space appointed by resident */
	for (int i=0; i<resident; ++i) {
		node_list[i].header = malloc(segment_size);
		if (node_list[i].header == NULL) {
			perror("malloc one segment within resident");
			return -1;
		}
		node_list[i].count = 0;
		node_list[i].recycle = NULL;
	}

	cur_index = get_index_to_alloc();
	cur_header = (char*)node_list[0].header;
	cur_offset = 0;

	max_node_count = (int)(segment_size / (sizeof(int) + block_size));
	max_segs_count = resident;

	return 0;
}		// -----  end of function init  -----

void EL_MM_Fixed::destroy()
{
	if (node_list == NULL)
		return;

	/* free alloc_bm */
	if (alloc_bm[0]) {
		free(alloc_bm[0]);
	}
	for (int i=0; i<4; ++i) {
		alloc_bm[i] = NULL;
	}

	/* free recycle_bm */
	if (recycle_bm[0]) {
		free(recycle_bm[0]);
	}
	for (int i=0; i<4; ++i) {
		recycle_bm[i] = NULL;
	}

	/* free each node in node_list */
	for (long i=0; i<max_segs_count; ++i) {
		if (node_list[i].header) {
			free(node_list[i].header);
			node_list[i].header = NULL;
		}
	}

	/* free node_list */
	free(node_list);
	node_list = NULL;
}		// -----  end of function destroy  -----

void* EL_MM_Fixed::alloc()
{
	union node *p = NULL;

	locker.lock();

	int index = get_index_to_reuse();
	if (index >= 0) {	/* reuse from recycle */

		p = node_list[index].recycle;
		node_list[index].recycle = node_list[index].recycle->next;
		--node_list[index].count;

		if (node_list[index].count == 0) {
			set_recycle_empty(index);
		}

		p->index = index;
	} else {	/* use new space */
		if (cur_offset + block_size > segment_size) {
			cur_index = get_index_to_alloc();

			if (cur_index < 0) {
				puts("all segments are in used");
				locker.unlock();
				return NULL;
			}

			if (cur_index >= resident) {
				if (cur_index == max_segs_count) {
					max_segs_count = cur_index + 1;
				}

				cur_header = (char*)malloc(segment_size);
				if (cur_header == NULL) {
					perror("malloc a new segment");
					locker.unlock();
					return NULL;
				}
				node_list[cur_index].header = cur_header;
				node_list[cur_index].count = 0;
				node_list[cur_index].recycle = NULL;
			} else {
				cur_header = (char*)node_list[cur_index].header;
			}

			cur_offset = 0;
		}

		p = (union node*)(cur_header + cur_offset);
		cur_offset += block_size + 4;
		p->index = cur_index;
	}

	locker.unlock();

	return p->space;
}		// -----  end of function alloc  -----

void EL_MM_Fixed::recycle(void *p)
{
	union node *pnode = (union node*)(((char*)p) - ((size_t) &(((union node*)0)->space)));
	int32_t index = pnode->index;

	locker.lock();

	pnode->next = node_list[index].recycle;
	node_list[index].recycle = pnode;
	++node_list[index].count;

	set_recycle(index);

	if(index >= resident && node_list[index].count == max_node_count) {
		free(node_list[index].header);
		node_list[index].header = NULL;
		node_list[index].count = 0;
		node_list[index].recycle = NULL;

		set_recycle_full(index);
 // :TODO:02/09/2014 04:54:41 PM:elwin: handle max_segs_count, but now is ok
	}

	locker.unlock();
}		// -----  end of function recycle  -----

// ==================== PRIVATE METHOD =======================================

int32_t EL_MM_Fixed::get_index_to_alloc()
{
	int32_t index = 0;
	int8_t offset = 0;

	if (*alloc_bm[0] == (uint64_t)~0x0)
		return -1;

	for (int i=0; i<bm_level; ++i) {
		index = (index << 6) | offset;
		offset =  (int8_t)EL_Bitmap::scan_forward_zero(*(alloc_bm[i] + index));
	}

	if ((index<<6)+offset == segments_size)
		return -1;

	EL_Bitmap::set<uint64_t>(*(alloc_bm[bm_level-1] + index), offset);

	int i = bm_level-1;
	if (*(alloc_bm[i] + index) == ~0x0UL) {
		int32_t idx = index;
		int8_t off;
		do {
			--i;
			off = idx & 0x3f;
			idx >>= 6;
			EL_Bitmap::set<uint64_t>(*(alloc_bm[i] + idx), off);
		} while (i>0 && *(alloc_bm[i] + idx) == ~0x0UL);
	}

	return (index << 6) | offset;
}		// -----  end of function get_index_to_alloc  -----

int32_t EL_MM_Fixed::get_index_to_reuse()
{
	int32_t index = 0;
	int8_t offset = 0;

	if (*recycle_bm[0] == 0x0)
		return -1;

	for (int i=0; i<bm_level; ++i) {
		index = (index << 6) | offset;
		offset =  (int8_t)EL_Bitmap::scan_forward(*(recycle_bm[i] + index));
	}

	return (index << 6) | offset;
}		// -----  end of function get_index_to_reuse  -----

void EL_MM_Fixed::set_recycle(int32_t index)
{
	int32_t idx = index >> 6;
	int8_t off = index & 0x3f;

	for (int i=bm_level-1; i>=0; --i) {
		EL_Bitmap::set<uint64_t>(*(recycle_bm[i] + idx), off);
		off = idx & 0x3f;
		idx >>= 6;
	}
}		// -----  end of function set_recycle  -----

void EL_MM_Fixed::set_recycle_empty(int32_t index)
{
	int32_t idx = index >> 6;
	int8_t off = index & 0x3f;

	for (int i=bm_level-1; i>=0; --i) {
		EL_Bitmap::unset<uint64_t>(*(recycle_bm[i] + idx), off);
		if (*(recycle_bm[i] + idx))
			break;
		off = idx & 0x3f;
		idx >>= 6;
	}
}		// -----  end of function set_recycle_empty  -----

void EL_MM_Fixed::set_recycle_full(int32_t index)
{
	int32_t idx;
	int8_t off;

	/* clear alloc_bm */
	idx = index >> 6;
	off = index & 0x3f;
	for (int i=bm_level-1; i>=0; --i) {
		EL_Bitmap::unset<uint64_t>(*(alloc_bm[i] + idx), off);
		off = idx & 0x3f;
		idx >>= 6;
	}

	/* clear recycle_bm */
	idx = index >> 6;
	off = index & 0x3f;
	for (int i=bm_level-1; i>=0; --i) {
		EL_Bitmap::unset<uint64_t>(*(recycle_bm[i] + idx), off);
		if (*(recycle_bm[i] + idx))
			break;
		off = idx & 0x3f;
		idx >>= 6;
	}
}		// -----  end of function set_recycle_full  -----

