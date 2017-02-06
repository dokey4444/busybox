/*
// =====================================================================================
// 
//       Filename:  elib_lock.hpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  10/26/2013 04:10:34 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#ifndef  _ELIB_LOCK_H_
#define  _ELIB_LOCK_H_

#include "elib_util.hpp"

// =====================================================================================
//        Class:  EL_Spin_Lock
//  Description:  互斥锁类
// =====================================================================================
class EL_Spin_Lock
{
public:
	// ====================  INTERFACE     =======================================
	EL_Spin_Lock(int type = PTHREAD_PROCESS_PRIVATE)
	{
		pthread_spin_init(&locker, type);
	}

	~EL_Spin_Lock()
	{
		pthread_spin_destroy(&locker);
	}

	int lock()
	{
		return pthread_spin_lock(&locker);
	}

	int unlock()
	{
		return pthread_spin_unlock(&locker);
	}

	int trylock()
	{
		return pthread_spin_trylock(&locker);
	}

private:
	// ====================  DATA MEMBERS  =======================================
	pthread_spinlock_t locker;

};		// -----  end of class EL_Spin_Lock  -----

// =====================================================================================
//        Class:  EL_Mutex_Lock
//  Description:  互斥锁类
// =====================================================================================
class EL_Mutex_Lock
{
public:
	// ====================  INTERFACE     =======================================
	EL_Mutex_Lock(int type = PTHREAD_MUTEX_NORMAL)
	{
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, type);

		pthread_mutex_init(&locker, &attr);
	}

	~EL_Mutex_Lock()
	{
		pthread_mutex_destroy(&locker);
		pthread_mutexattr_destroy(&attr);
	}

	int lock()
	{
		return pthread_mutex_lock(&locker);
	}

	int unlock()
	{
		return pthread_mutex_unlock(&locker);
	}

	int trylock()
	{
		return pthread_mutex_trylock(&locker);
	}

private:
	// ====================  DATA MEMBERS  =======================================
	pthread_mutex_t locker;
	pthread_mutexattr_t attr;

};		// -----  end of class EL_Mutex_Lock  -----

// =====================================================================================
//        Class:  EL_RW_Lock()
//  Description:  读写锁类(writer wins)
// =====================================================================================
class EL_RW_Lock
{
public:
	// ====================  INTERFACE     =======================================
	EL_RW_Lock()
	{
		pthread_rwlock_init(&locker, NULL);
	}

	~EL_RW_Lock()
	{
		pthread_rwlock_destroy(&locker);
	}

	int lock_r()
	{
		return pthread_rwlock_rdlock(&locker);
	}

	int lock_w()
	{
		return pthread_rwlock_wrlock(&locker);
	}

	int unlock()
	{
		return pthread_rwlock_unlock(&locker);
	}

	int trylock_r()
	{
		return pthread_rwlock_tryrdlock(&locker);
	}

	int trylock_w()
	{
		return pthread_rwlock_trywrlock(&locker);
	}

private:
	// ====================  DATA MEMBERS  =======================================
	pthread_rwlock_t locker;
};		// -----  end of class EL_RW_Lock  -----

#endif   // ----- #ifndef _ELIB_LOCK_H_  -----

