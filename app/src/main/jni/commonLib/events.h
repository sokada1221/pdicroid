#ifndef __events_h
#define	__events_h

#if defined(UNIX)
#include "defs.h"
class TEvent {
	TMutex Mutex;
	pthread_cond_t cond;
	bool sig;
public:
	TEvent()
	{
		sig = false;
		pthread_cond_init( &cond, NULL );
	}
	virtual ~TEvent()
	{
		pthread_cond_destroy(&cond);
	}
	void Wait(int msec=-1)
	{
		Mutex.Lock();
		while (!sig){
			if (msec!=-1){
				timespec to;
				to.tv_sec = time(NULL) + msec/1000;
				to.tv_nsec = (msec%1000) * 1000;
				int err = pthread_cond_timedwait(&cond, Mutex.mutex(), &to);
				if (err==ETIMEDOUT)
					break;
			} else {
				pthread_cond_wait(&cond, Mutex.mutex());
			}
		}
		Mutex.Unlock();
	}
	void Wake()
	{
		Mutex.Lock();
		sig = true;
		Mutex.Unlock();
	}
};
#else
class TEvent {
protected:
	HANDLE Handle;
public:
	int Counter;	// generic counter
	int Param;		// generic parameter
public:
	TEvent()
	{
		Handle = CreateEvent(NULL, FALSE, FALSE, NULL);
		Counter = 0;
		Param = 0;
	}
	virtual ~TEvent()
	{
		CloseHandle(Handle);
	}
	bool Wait(int msec=INFINITE)
	{
		return WaitForSingleObject(Handle, msec) == WAIT_OBJECT_0;
	}
	void Wake()
	{
		SetEvent(Handle);
	}
};
#endif

#endif	// __events_h
