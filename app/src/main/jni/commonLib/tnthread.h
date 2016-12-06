#ifndef __tnthread_h
#define	__tnthread_h

typedef int (*tnthread_method_t)(class tnthread *_this, int param);

#define	tnthread_method(name, cls)	static int cb##name(tnthread *thread, int param) { return ((cls*)thread)->name(param); } \
									int name(int param)
#define	tnthread_sync(name, param)	Synchronize(cb##name,param)

#include "tnassert.h"

#ifdef UNIX
class tnthread {
protected:
	pthread_t thread;
	bool ThreadCreated;
	void *ReturnValue;
	bool Suspended;
	bool Terminated;
	bool Finished;
	tnthread_method_t Method;	//TODO: 機能せず
public:
	bool FreeOnTerminate;	//TODO: 機能せず
	//static HWND hwndSync;
	//static int msgSync;
public:
	tnthread(bool createsuspend)
	{
		//__assert(hwndSync!=NULL);
		ThreadCreated = false;
		ReturnValue = 0;
		Suspended = createsuspend;
		Terminated = false;
		Finished = false;
		FreeOnTerminate = false;
		if (!createsuspend){
			StartThread();
		}
	}
	virtual ~tnthread()
	{
		if (!Finished && !Suspended){
			Terminate();
			WaitFor();
		}
		pthread_detach(thread);	//TODO: FreeOnTerminateでも大丈夫？
	}
	void StartThread()
	{
		if (pthread_create(&thread, NULL, thread_func, this)==0){
			// thread ok
			ThreadCreated = true;
		}
	}
#if 0
	static void Init(HWND hwnd, int msg)
		{ hwndSync = hwnd; msgSync = msg; }
#endif
	//Note:
	// pthreadにsuspendはない？
	// そのためResume()はdummy (pthread_create()したらすぐに実行開始)
	// これを承知した呼び出し側は_Resume()を使用する。
	void _Resume()
	{
		//ResumeThread(hThread);
		Suspended = false;
	}
#if 1
	void Suspend()
	{
		pthread_kill(thread, SIGSTOP);	//Note: この関数を戻ったとき、実際にsuspend状態になっているとは限らない
										// それを保証するにはSIGUSR1のhandlerを登録してそのhandlerでsem_postする必要がある
		Suspended = true;
	}
#endif
	void Terminate()
	{
		Terminated = true;
	}
	bool IsFinished() const
	{
		return Finished;
	}
	int WaitFor()
	{
		pthread_join(thread, &ReturnValue);
		return (int)ReturnValue;
	}
#if 0
	static int SyncMethod(WPARAM _param, LPARAM _this)
	{
		return ((tnthread*)_this)->Method((tnthread*)_this, _param);
	}
#endif
protected:
	virtual void Execute() = 0;
#if 0
	int Synchronize(tnthread_method_t func, int param)
	{
		if (hwndSync){
			Method = func;
			return SendMessage(hwndSync, msgSync, param, (LPARAM)this);
		}
		return 0;
	}
#endif

	static void *thread_func(void *arg)
	{
		((tnthread*)arg)->Execute();
		return (void*)((tnthread*)arg)->ReturnValue;
	}
};
#else	// !UNIX
class tnthread {
protected:
	HANDLE hThread;
	DWORD ThreadId;
	int ReturnValue;
	bool Suspended;
	bool Terminated;
	bool Finished;
	tnthread_method_t Method;
public:
	bool FreeOnTerminate;
	static HWND hwndSync;
	static int msgSync;
public:
	tnthread(bool createsuspend)
	{
		__assert(hwndSync!=NULL);
		ReturnValue = 0;
		Suspended = createsuspend;
		Terminated = false;
		Finished = false;
		FreeOnTerminate = false;
		hThread = CreateThread(NULL, 0, cbEntry, this, createsuspend ? CREATE_SUSPENDED:0, &ThreadId);
	}
	virtual ~tnthread()
	{
		if (!Finished && !Suspended){
			Terminate();
			WaitFor();
		}
		if (hThread){
			CloseHandle(hThread);
		}
	}
	inline void StartThread()
	{
		Resume();
	}
	static void Init(HWND hwnd, int msg)
		{ hwndSync = hwnd; msgSync = msg; }
	inline void _Resume(){ Resume(); }
	void Resume()
	{
		ResumeThread(hThread);
		Suspended = false;
	}
	void Suspend()
	{
		SuspendThread(hThread);
		Suspended = true;
	}
	void Terminate()
	{
		Terminated = true;
	}
	bool IsFinished() const
	{
		return Finished;
	}
	int WaitFor()
	{
		WaitForSingleObject(hThread, INFINITE);
		DWORD r;
		GetExitCodeThread(hThread, &r);
		return (int)r;
	}
	static int SyncMethod(WPARAM _param, LPARAM _this)
	{
		return ((tnthread*)_this)->Method((tnthread*)_this, _param);
	}
protected:
	virtual void Execute() = 0;
	int Synchronize(tnthread_method_t func, int param)
	{
		if (hwndSync){
			Method = func;
			return SendMessage(hwndSync, msgSync, param, (LPARAM)this);
		}
		return 0;
	}

	static DWORD WINAPI cbEntry(LPVOID lpParam)
	{
		return ((tnthread*)lpParam)->Entry();
	}
	DWORD Entry();
};
#endif	// !UNIX

#endif	// __tnthread_h
