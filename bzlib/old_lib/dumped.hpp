/*
// =====================================================================================
// 
//       Filename:  elib_lock.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  10/26/2013 04:10:52 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "elib_lock.hpp"

// =====================================================================================
//        Class:  EL_RW_Lock()
//  Description:  璇诲啓閿佺被
// =====================================================================================
class EL_RW_Lock
{
public:
	// ====================  INTERFACE     =======================================
	EL_RW_Lock(bool is_writer_wins = true);
	~EL_RW_Lock();

	void lock_r();
	void lock_w();
	void unlock();
	int trylock_r();
	int trylock_w();

private:
	// ====================  DATA MEMBERS  =======================================
	unsigned sign_mask;
	unsigned sign_lock;
	pthread_mutex_t locker;
};		// -----  end of class EL_RW_Lock  -----

// =====================================================================================
//        Class:  EL_RW_Lock
//        data structure: | w_using(1bit) | r_stop(1bit) | r_count(6bit + 3byte) |
// =====================================================================================
EL_RW_Lock::EL_RW_Lock(bool is_writer_wins)
{
	sign_mask = is_writer_wins == true ? 0xc0000000 : 0x80000000;	// has r_stop or not
	sign_lock = 0;
	pthread_mutex_init(&locker, NULL);
}

EL_RW_Lock::~EL_RW_Lock()
{
	pthread_mutex_destroy(&locker);
}

void EL_RW_Lock::lock_r()
{
	for ( ; ; ) {
		pthread_mutex_lock(&locker);

		if (!(sign_lock & sign_mask)) {	// no w_using, no r_stop(if there is)
			++sign_lock;
			pthread_mutex_unlock(&locker);
			break;
		}
		pthread_mutex_unlock(&locker);
		usleep(1000);	// use cond replace sleep when it's necessary
	}
}

void EL_RW_Lock::lock_w()
{
	for ( ; ; ) {
		pthread_mutex_lock(&locker);

		if (!(sign_lock & 0xbfffffff)) {	// no w_using, no r_count
			sign_lock |= sign_mask;
			pthread_mutex_unlock(&locker);
			break;
		}
		sign_lock |= sign_mask & 0x7fffffff;	// set r_stop to sign_lock(if there is)
		pthread_mutex_unlock(&locker);
		usleep(1000);	// use cond replace sleep when it's necessary
	}
}

void EL_RW_Lock::unlock()
{
	pthread_mutex_lock(&locker);

	if ((sign_lock & 0xc0000000) == sign_mask)	// unlock w_lock
		sign_lock = 0;
	else
		--sign_lock;

	pthread_mutex_unlock(&locker);
}

int EL_RW_Lock::trylock_r()
{
	if (pthread_mutex_trylock(&locker) < 0)
		return -1;

	if (sign_lock & sign_mask) {
		pthread_mutex_unlock(&locker);
		return -1;
	}
	// no w_using, no r_stop(if there is)
	++sign_lock;
	pthread_mutex_unlock(&locker);
	return 0;

}

int EL_RW_Lock::trylock_w()
{
	if (pthread_mutex_trylock(&locker) < 0)
		return -1;

	if (sign_lock & 0xbfffffff) {
		// :TRICKY:07/16/2013 10:14:05 AM:elwin: "is_writer_wins" flag is useless in trylock_w()
//		sign_lock |= sign_mask & 0x7fffffff;	// set r_stop to sign_lock(if there is)
		pthread_mutex_unlock(&locker);
		return -1;
	}
	// no w_using, no r_count
	sign_lock |= sign_mask;
	pthread_mutex_unlock(&locker);
	return 0;
}

