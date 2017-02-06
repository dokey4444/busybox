/*
// =====================================================================================
// 
//       Filename:  elib_bitmap.hpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  12/30/2013 11:00:38 AM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#ifndef  _ELIB_BITMAP_H_
#define  _ELIB_BITMAP_H_

#include "elib_asm.hpp"

// =====================================================================================
//        Class:  EL_Bitmap
//  Description:  
// =====================================================================================
class EL_Bitmap
{
public:
	// ====================  LIFECYCLE     =======================================
	EL_Bitmap()
	{
	}

	~EL_Bitmap()
	{
		if (bitmap)
			destroy();
	}

	// ====================  INTERFACE     =======================================
	
	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  init
	//  Description:  初始化位图
	//   Parameters:  size: 位图一共由几个bit组成
	//                outer_space: 如果该参数被设置，则位图使用外部内存空间
	//                outer_space必须提供能够容纳指定比特数的内存空间，且必须为整字节空间
	//  ReturnValue:  成功返回0，失败返回-1
	// =====================================================================================
	*/
	int init(long size, void *outer_space = NULL)
	{
		this->size = (size+63L) >> 6L;

		if (outer_space) {
			outer = true;
			bitmap = (ulong*)outer_space;
		} else {
			outer = false;
			bitmap = new ulong[this->size];
			if (bitmap == NULL)
				return -1;
		}

		int n = (int)(size % 64);
		if (n) {
			tail_mask = 0UL;
			for (int i=0; i<n; ++i) {
				tail_mask |= 1UL<<i;
			}
		} else {
			tail_mask = ~0UL;
		}

		return 0;
	}		// -----  end of function init  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  destroy
	//  Description:  销毁当前位图
	// =====================================================================================
	*/
	void destroy()
	{
		if (outer == false && bitmap) {
			delete[] bitmap;
		}
		bitmap = NULL;
		outer = false;
		tail_mask = 0UL;
		size = 0;
	}		// -----  end of function destroy()  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  set
	//  Description:  将指定比特设置为1
	//   Parameters:  指定需要设置的比特位
	//  ReturnValue:  null
	// =====================================================================================
	*/
	void set(long location)
	{
		long idx = location >> 6L;
		ulong off = location & 0x3fUL;

		set<ulong>(bitmap[idx], off);
	}		// -----  end of function set  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  unset
	//  Description:  将指定比特设置为0
	//   Parameters:  指定需要设置的比特位
	//  ReturnValue:  null
	// =====================================================================================
	*/
	void unset(long location)
	{
		long idx = location >> 6L;
		ulong off = location & 0x3fUL;

		unset<ulong>(bitmap[idx], off);
	}		// -----  end of function unset()  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  get
	//  Description:  查看指定比特的状态
	//   Parameters:  指定需要查看的比特位
	//  ReturnValue:  0为false，1为true
	// =====================================================================================
	*/
	bool get(long location)
	{
		long idx = location >> 6L;
		ulong off = location & 0x3fUL;

		return bitmap[idx] & (1L<<off) ? true : false;
	}		// -----  end of function get  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  scan_forward
	//  Description:  从低字节向高字节搜索第一个1的位置
	//   Parameters:  null
	//  ReturnValue:  第一个1在位图中的位置
	// =====================================================================================
	*/
	long scan_forward()
	{
		long n = 0;
		for ( long i=0; i<size; ++i) {
			if (bitmap[i] == 0UL) {
				n += 64L;
			} else {
				n += bsf(bitmap[i]);
				return n;
			}
		}

		return -1;
	}		// -----  end of function scan_forward  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  scan_reverse
	//  Description:  从高字节向低字节搜索第一个1的位置
	//   Parameters:  null
	//  ReturnValue:  最后一个1在位图中的位置
	// =====================================================================================
	*/
	long scan_reverse()
	{
		long n = 0;

		for (long i=size-1; i>=0; --i) {
			if (bitmap[i] != 0UL) {
				n = 64L * i;
				n += bsr(bitmap[i]);
				return n;
			}
		}

		return -1;
	}		// -----  end of function scan_reverse  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  scan_forward_zero
	//  Description:  从低字节向高字节搜索第一个0的位置
	//   Parameters:  null
	//  ReturnValue:  第一个0在位图中的位置
	// =====================================================================================
	*/
	long scan_forward_zero()
	{
		long n = 0;
		for (long i=0; i<size-1; ++i) {
			if (bitmap[i] == ~0UL) {
				n += 64L;
			} else {
				n += bsf(~bitmap[i]);
				return n;
			}
		}

		if (bitmap[size-1] != (~0UL & tail_mask)) {
			n += bsf(~bitmap[size-1] & tail_mask);
			return n;
		}

		return -1;
	}		// -----  end of function scan_forward_zero  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  scan_reverse_zero
	//  Description:  从高字节向低字节搜索第一个0的位置
	//   Parameters:  null
	//  ReturnValue:  最后一个0在位图中的位置
	// =====================================================================================
	*/
	long scan_reverse_zero()
	{
		long n = 0;
		long i = size - 1;

		if (bitmap[i] != (~0UL & tail_mask)) {
			n = 64L * i;
			n += bsr(~bitmap[i] & tail_mask);
			return n;
		}
		--i;

		for ( ; i>=0; --i) {
			if (bitmap[i] != ~0UL) {
				n = 64L * i;
				n += bsr(~bitmap[i]);
				return n;
			}
		}

		return -1;
	}		// -----  end of function scan_reverse_zero  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  clean
	//  Description:  将位图的所有位全部置0
	// =====================================================================================
	*/
	void clean()
	{
		memset(bitmap, 0, size<<3);
	}		// -----  end of function clean  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  fill
	//  Description:  将位图的所有位全部置0
	// =====================================================================================
	*/
	void fill()
	{
		memset(bitmap, 0xff, size<<3L);
		bitmap[size-1] &= tail_mask;
	}		// -----  end of function fill  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  head
	//  Description:  返回bitmap数组首地址
	// =====================================================================================
	*/
	void* head()
	{
		return (void*)bitmap;
	}		// -----  end of function head  -----
	
	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  scan_forward
	//  Description:  从低字节向高字节搜索第一个为1的比特位
	//                警告：不要使用有符号类型变量做强制类型转换传参，除非你非常清楚自己在做什么
	// =====================================================================================
	*/
	static long scan_forward(ulong n)
	{
		return bsf(n);
	}		// -----  end of static function scan_forward  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  scan_forward
	//  Description:  从低字节向高字节搜索第一个为1的比特位
	//                警告：不要使用有符号类型变量做强制类型转换传参，除非你非常清楚自己在做什么
	// =====================================================================================
	*/
	static long scan_reverse(ulong n)
	{
		return bsr(n);
	}		// -----  end of static function scan_reverse  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  scan_reverse
	//  Description:  从高字节向低字节搜索第一个为1的比特位
	//                警告：不要使用有符号类型变量做强制类型转换传参，除非你非常清楚自己在做什么
	// =====================================================================================
	*/
	static long scan_forward_zero(ulong n)
	{
		return bsf(~n);
	}		// -----  end of static function scan_forward_zero  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  scan_reverse_zero
	//  Description:  从高字节向低字节搜索第一个为0的比特位
	//                警告：不要使用有符号类型变量做强制类型转换传参，除非你非常清楚自己在做什么
	// =====================================================================================
	*/
	static long scan_reverse_zero(ulong n)
	{
		return bsr(~n);
	}		// -----  end of static function scan_reverse_zero  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  set
	//  Description:  为指定数据类型的指定位设置为1
	//   Parameters:  n: 变量的引用
	//                location: 需要置位的bit在变量中的位置
	//  ReturnValue:  null
	// =====================================================================================
	*/
	template <typename T>
	static void set(T &n, long location)
	{
		n |= ( (T)0x1 )<<location;
	}		// -----  end of static function set  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  unset
	//  Description:  为指定数据类型的指定位设置为0
	//   Parameters:  n: 变量的引用
	//                location: 需要置位的bit在变量中的位置
	//  ReturnValue:  null
	// =====================================================================================
	*/
	template <typename T>
	static void unset(T &n, long location)
	{
		n &= ~(( (T)0x1 )<<location);
	}		// -----  end of static function unset  -----

	// ====================  OPERATORS     =======================================
	bool operator[](long location)
	{
		return get(location);
	}		// -----  end of function operator[]  -----

private:
	// ==================== PRIVATE METHOD =======================================

	// ====================  DATA MEMBERS  =======================================
	bool outer;
	ulong *bitmap;
	long size;
	ulong tail_mask;

};		// -----  end of class EL_Bitmap  -----


#endif   // ----- #ifndef _ELIB_BITMAP_H_  -----
