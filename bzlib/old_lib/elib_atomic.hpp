/*
// =====================================================================================
// 
//       Filename:  elib_atomic.hpp
// 
//    Description:  对原子操作进行封装
//                  以下非常用功能没有在封装中提供相应功能
//                  __sync_nand_and_fetch
// 
//        Version:  1.0
//        Created:  11/17/2014 03:22:04 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "elib_util.hpp"

#ifndef  _ELIB_ATOMIC_
#define  _ELIB_ATOMIC_

// =====================================================================================
//        Class:  EL_Atomic
//  Description:  
// =====================================================================================
class EL_Atomic
{
public:
	// ====================  LIFECYCLE     =======================================
	EL_Atomic() : m_value(0) {}
	~EL_Atomic() {}

	// ====================  INTERFACE     =======================================

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  value
	//  Description:   
	//   Parameters:  
	//  ReturnValue:  
	// =====================================================================================
	*/
	int value()
	{
#if __GNUC__ == 4
		return m_value;
#else
#endif
	}		// -----  end of function value  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  set
	//  Description:   
	//   Parameters:  
	//  ReturnValue:  
	// =====================================================================================
	*/
	void set(int value)
	{
		*this = value;
	}		// -----  end of function set  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  cas
	//  Description:   
	//   Parameters:  
	//  ReturnValue:  
	// =====================================================================================
	*/
	bool cas_set(int old_value, int new_value)
	{
#if __GNUC__ == 4
		return sync_cas_bool(&m_value, old_value, new_value);
#else
#endif
	}		// -----  end of function cas  -----

	// ====================  OPERATORS     =======================================

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  operator= (set int)
	//  Description:   
	//   Parameters:  
	//  ReturnValue:  
	// =====================================================================================
	*/
	void operator=(int value)
	{
#if __GNUC__ == 4
		m_value = value;
#else
#endif
	}		// -----  end of function operator=  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  operator+=
	//  Description:   
	//   Parameters:  
	//  ReturnValue:  
	// =====================================================================================
	*/
	int operator+=(int n)
	{
#if __GNUC__ == 4
		return sync_add_and_fetch(&m_value, n);
#else
#endif
	}		// -----  end of function operator+=  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  operator-=
	//  Description:   
	//   Parameters:  
	//  ReturnValue:  
	// =====================================================================================
	*/
	int operator-=(int n)
	{
#if __GNUC__ == 4
		return sync_sub_and_fetch(&m_value, n);
#else
#endif
	}		// -----  end of function operator-=  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  operator++
	//  Description:   
	//   Parameters:  
	//  ReturnValue:  
	// =====================================================================================
	*/
	int operator++()
	{
#if __GNUC__ == 4
		return sync_add_and_fetch(&m_value, 1);
#else
#endif
	}		// -----  end of function operator++  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  operator++
	//  Description:   
	//   Parameters:  
	//  ReturnValue:  
	// =====================================================================================
	*/
	int operator++(int)
	{
#if __GNUC__ == 4
		return sync_fetch_and_add(&m_value, 1);
#else
#endif
	}		// -----  end of function operator++  -----
	
	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  operator--
	//  Description:   
	//   Parameters:  
	//  ReturnValue:  
	// =====================================================================================
	*/
	int operator--()
	{
#if __GNUC__ == 4
		return sync_sub_and_fetch(&m_value, 1);
#else
#endif
	}		// -----  end of function operator--  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  operator--
	//  Description:   
	//   Parameters:  
	//  ReturnValue:  
	// =====================================================================================
	*/
	int operator--(int)
	{
#if __GNUC__ == 4
		return sync_fetch_and_sub(&m_value, 1);
#else
#endif
	}		// -----  end of function operator--  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  operator&=
	//  Description:   
	//   Parameters:  
	//  ReturnValue:  
	// =====================================================================================
	*/
	int operator&=(uint n)
	{
#if __GNUC__ == 4
		return sync_and_and_fetch(&m_value, n);
#else
#endif
	}		// -----  end of function operator&=  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  operator|=
	//  Description:   
	//   Parameters:  
	//  ReturnValue:  
	// =====================================================================================
	*/
	int operator|=(uint n)
	{
#if __GNUC__ == 4
		return sync_or_and_fetch(&m_value, n);
#else
#endif
	}		// -----  end of function operator|=  -----

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  operator^=
	//  Description:   
	//   Parameters:  
	//  ReturnValue:  
	// =====================================================================================
	*/
	int operator^=(uint n)
	{
#if __GNUC__ == 4
		return sync_xor_and_fetch(&m_value, n);
#else
#endif
	}		// -----  end of function operator^=  -----

private:
	// ==================== PRIVATE METHOD =======================================

	// ====================  DATA MEMBERS  =======================================
#if __GNUC__ == 4	// 使用GCC提供的__sync内置函数族实现原子操作
	int m_value;
	int m_old;
#else			// 使用Linux提供的atomic变量实现原子操作
	atomic_t m_value;
	atomic_t m_old;
#endif

};		// -----  end of class EL_Atomic  -----

#endif   // ----- #ifndef _ELIB_ATOMIC_  -----

