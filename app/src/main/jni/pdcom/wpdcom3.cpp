#include "tnlib.h"
#pragma hdrstop
#include "wpdcom.h"
#include "txr.h"

//
// Application Depends codes
// ex.uses .txr

int MsgBox(HWND hwnd, int text, int caption, UINT type)
{
	__assert(text);
	return MsgBox(hwnd, _LT(text), caption ? _LT(caption) : NULL, type);
}
int MsgBox(int text, int caption, UINT type)
{
	__assert(text);
	return MsgBox(_LT(text), caption ? _LT(caption) : NULL, type);
}

