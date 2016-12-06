#include "stdafx.h"
#ifdef _Windows
#include <windows.h>
#endif
#include <stdio.h>
#include "tndefs.h"
#include "tnassert.h"

#if 0	// old version
void __assertion_error(const char *file, int line)
{
	char buf[100];
	wsprintfA(buf, "ASSERTION ERROR: %s %d", file, line);
	MessageBoxA(NULL, buf, "", MB_OK);
}
#endif

void __assert_message(const tchar *format, ...)
{
#ifdef _Windows
	va_list ap;
	va_start(ap, format);
	tchar buf[1024];
	_vsntprintf(buf, sizeof(buf), format, ap);
	MessageBox(NULL, buf, _T("Assertion Failure"), MB_OK);
	va_end(ap);
#else
	//TODO: Ç«Ç§Ç∑ÇÈÅH@Android
#endif
}

