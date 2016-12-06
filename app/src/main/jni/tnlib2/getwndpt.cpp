#include	<windows.h>
#pragma	hdrstop
#include "tnwinx.h"

tnstr GetWindowTextStr( HWND hwnd )
{
	int l = GetWindowTextLength( hwnd );
	tchar *temp = new tchar[ l + 1 ];
	if (!temp)
		return _t("");
	temp[0] = '\0';
	GetWindowText( hwnd, temp, l+1 );
	tnstr ret;
	ret.setBuf(temp);
	return ret;
}

