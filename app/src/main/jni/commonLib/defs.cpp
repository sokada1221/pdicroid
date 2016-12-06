#include "tnlib.h"
#pragma hdsrtop
#include "defs.h"

#if MUTEX_DEBUG || !USE_MUTEX_ATTR
void TMutex::Lock()
{
#if USE_MUTEX
#if MUTEX_DEBUG
	if (pthread_mutex_trylock(&_mutex)==EBUSY){
		printf("Deadlock!!");
	}
#else
	pthread_mutex_lock(&CounterMutex);
	if (pthread_mutex_trylock(&_mutex)==EBUSY){
		if (LockThread != pthread_self()){
			// ‘¼‚Ìthread‚ªlock‚µ‚Ä‚¢‚é
			pthread_mutex_unlock(&CounterMutex);
			pthread_mutex_lock(&_mutex);	// lock‘Ò‚¿
			pthread_mutex_lock(&CounterMutex);
			Counter++;
			LockThread = pthread_self();
			pthread_mutex_unlock(&CounterMutex);
			return;
		}
	}
	// “¯‚¶thread‚È‚çCounterMutex‚ÍCounter++ŒãAunlock‚·‚é‚¾‚¯‚Å‚¢‚¢
	// Å‰‚Ìlock‚à“¯—l
	Counter++;
	LockThread = pthread_self();
	pthread_mutex_unlock(&CounterMutex);
#endif
#endif
}
#endif

#if !USE_MUTEX_ATTR
void TMutex::Unlock()
{
#if USE_MUTEX
	pthread_mutex_lock(&CounterMutex);
	pthread_mutex_unlock(&_mutex);
	if (--Counter==0){
		LockThread = 0;
	}
	pthread_mutex_unlock(&CounterMutex);
#endif
}
#endif

