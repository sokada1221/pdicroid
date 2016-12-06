#include <windows.h>
#pragma hdrstop
#include "tnthread.h"

#ifdef UNIX
void tnthread::Entry()
{
	try {
		Execute();
	} catch(...) {}
	bool freethread = FreeOnTerminate;
	int ret = ReturnValue;
	Finished = true;
	if (freethread) delete this;
//	return (DWORD)ret;
}
#else	// !UNIX

HWND tnthread::hwndSync = NULL;
int tnthread::msgSync = 0;

DWORD tnthread::Entry()
{
	try {
		Execute();
	} catch(...) {}
	bool freethread = FreeOnTerminate;
	int ret = ReturnValue;
	Finished = true;
	if (freethread) delete this;
	return (DWORD)ret;
}

#endif
