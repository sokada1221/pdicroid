#ifndef __tnhtmlhelp_h
#define	__tnhtmlhelp_h

#include "tndefs.h"

void TNHtmlHelpInit();
void TNHtmlHelpUninit();
HWND TNHtmlHelp(HWND hwnd, const tchar *path, int cmd, int data);
void TNHtmlHelpFree();
void TNHtmlHelpDefault(bool usehwnd);
BOOL WINAPI _TNHtmlHelp(HWND hwnd, LPCTSTR path, UINT cmd, DWORD data);
BOOL WINAPI _TNHtmlHelpNoHwnd(HWND hwnd, LPCTSTR path, UINT cmd, DWORD data);

#endif	/* __tnhtmlhelp_h */
