/*
// =====================================================================================
// 
//       Filename:  elib_asm.hpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  12/30/2013 01:36:30 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "elib_util.hpp"

static inline long bsf(ulong n)
{
	__asm__(
			"bsf %1, %0;"
			:"=r"(n)
			:"r"(n)
			:"%eax"
	       );

	return n;
}

static inline long bsr(ulong n)
{
	__asm__(
			"bsr %1, %0;"
			:"=r"(n)
			:"r"(n)
			:"%eax"
	       );

	return n;
}

