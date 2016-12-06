#ifndef __tnhelp_h
#define	__tnhelp_h

#include "tndefs.h"

typedef BOOL (WINAPI *FNWinHelp)(HWND hwnd, LPCTSTR path, UINT cmd, DWORD id);

extern FNWinHelp TNWinHelp;

#endif	/* __tnhelp_h */
