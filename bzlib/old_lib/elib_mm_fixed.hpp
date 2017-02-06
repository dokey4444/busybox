/*
// =====================================================================================
// 
//       Filename:  elib_mm_fixed.hpp
// 
//    Description:  定长内存分配（与elib_mm_block的区别为本分配器可以真正释放内存）
// 
//        Version:  1.0
//        Created:  12/27/2013 04:59:42 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#ifndef  _ELIB_MM_FIXED_H_
#define  _ELIB_MM_FIXED_H_

#include "elib_lock.hpp"

// =====================================================================================
//        Class:  EL_MM_Fixed
//  Description:  
// =====================================================================================
class EL_MM_Fixed
{
public:
	// ====================  LIFECYCLE     =======================================
	EL_MM_Fixed();
	~EL_MM_Fixed();

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  init
	//  Description:  初始化内存管理器
	//   Parameters:  block_size: 定长块的大小
	//                one_segment_pow: 段大小的幂次，每个block都是从segment中获取
	//                                 当一个segment用完之后会申请新的segment用于block的分配
	//                                 幂次不得大于32
	//                segment_size_pow: 最大段个数的幂次，用于表示最多只能管理多少个段
	//                                  幂次不得大于32
	//                resident: 驻留集大小，表示管理器中最少保留多少个段不释放，最少为1
	//  ReturnValue:  成功返回0，失败返回-1
	// =====================================================================================
	*/
	int init(uint32_t block_size, uint8_t segment_size_pow = 22, uint8_t segments_size_pow = 18, int32_t resident = 1);

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  destroy
	//  Description:  销毁对象，释放内存
	// =====================================================================================
	*/
	void destroy();

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  alloc
	//  Description:  获取定长分配的内存，失败返回NULL
	// =====================================================================================
	*/
	void* alloc();

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  recycle
	//  Description:  回收已分配的内存
	// =====================================================================================
	*/
	void recycle(void *p);

private:
// public:	// delete it when all finished
	// ==================== PRIVATE METHOD =======================================

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  get_index_to_alloc
	//  Description:  获取当前可用于分配的新内存的node_list下标，分无可分，返回-1
	// =====================================================================================
	*/
	int32_t get_index_to_alloc();

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  get_index_to_reuse
	//  Description:  获取当前可用于重复使用的内存的node_list下标，无可用内存返回-1
	// =====================================================================================
	*/
	int32_t get_index_to_reuse();

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  set_recycle
	//  Description:  设置当前index已经被回收
	// =====================================================================================
	*/
	void set_recycle(int32_t index);

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  set_recycle_empty
	//  Description:  标记当前下标位置的内存已经全部用完，下次再申请时，需要另寻新的位置
	// =====================================================================================
	*/
	void set_recycle_empty(int32_t index);

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  set_recycle_full
	//  Description:  标记当前下标位置的内存全部归还，可释放空间，并允许该下标被再分配
	// =====================================================================================
	*/
	void set_recycle_full(int32_t index);

	// ====================  DATA MEMBERS  =======================================
	uint32_t block_size;	/* 8B ~ segment_size-4B */
	uint32_t segment_size;	/* 1MB ~ 4GB-1B */
	int32_t segments_size;	/* 1K ~ 16M */
	int32_t resident;	/* 1 ~ segments_size */

	int8_t bm_level;
	uint64_t *recycle_bm[4];	// 4 floors of bitmap pyramid for record recycle
	uint64_t *alloc_bm[4];	// 4 floors of bitmap pyramid for record alloc

	union node {
		union node *next;	// for recycle
		struct {
			int32_t index;	// for recycle
			byte space[1];	// for user
		};
	};

#pragma pack(push)
#pragma pack(4)
	struct head {
		void *header;
		int32_t count;
		union node *recycle;
	};
#pragma pack(pop)

	struct head *node_list;	// recycle list

	char *cur_header;	// record current header
	int32_t  cur_index;	// for record the max index in used
	uint32_t  cur_offset;	// offset of current space for alloc

	int32_t max_node_count;	// max nodes per segment
	int32_t max_segs_count;	// max segments in used

	EL_Mutex_Lock locker;
};		// -----  end of class EL_MM_Fixed  -----

#endif   // ----- #ifndef _ELIB_MM_FIXED_H_  -----
