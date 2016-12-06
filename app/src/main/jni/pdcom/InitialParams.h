#ifndef __InitialParams_h
#define	__InitialParams_h

// Initial State //
#define	INIT_VERSIONINFO	0x0001	// show version info
#define	INIT_NODICGROUP		0x0002
#define	INIT_OPENDICTIONARY	0x0004
#define	INIT_VIEWPOPUP		0x0008
#define	INIT_NOMAINMENU		0x0010
#define	INIT_ALLSEARCH		0x0020

class TStartupSemaphore {
protected:
	HANDLE hSem;
public:
	TStartupSemaphore(const tchar *name)
	{
		hSem = CreateSemaphore(NULL, 1, 1, name);
	}
	~TStartupSemaphore()
	{
		if (hSem){
			CloseHandle(hSem);
		}
	}
	bool Wait(int waittime)
	{
		return WaitForSingleObject(hSem, waittime) == WAIT_OBJECT_0;
	}
	void Release()
	{
		ReleaseSemaphore(hSem, 1, NULL);
	}
};

class TInitialParams {
public:
//	bool OpenPSWindow;
	int State;
	tnstr PopupFile;
	tnstr Dictionary;
	tnstr SearchWord;
	int ShowState;
	tnstr cmdGroupName;
	int GroupIndex;
protected:
	TStartupSemaphore *StartupSem;
	bool StartupOk;
public:
	TInitialParams();
	~TInitialParams();
	void SetState(int flag)
		{ State |= flag; }
	bool WaitStartup();
	void ReleaseStartup();
};

#endif	/* __InitialParams_h */

