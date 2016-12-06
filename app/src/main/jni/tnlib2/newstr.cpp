#include "stdafx.h"
#include	<string.h>
#pragma	hdrstop
#include "tndefs.h"

// 文字列の確保、コピー
tchar *newstr( const tchar *str )
{
	if ( !str )
		return 0;
	tchar *p = new tchar[ _tcslen( str ) + 1 ];
	if ( !p )
		return 0;
	return _tcscpy( p, str );
}

// 指定数文字列の確保、コピー
tchar *newstr( const tchar *str, int len )
{
	if ( !str )
		return 0;
	tchar *p = new tchar[ len + 1 ];
	if ( !p )
		return p;
	p[ len ] = '\0';
	return _tcsncpy( p, str, len );
}
