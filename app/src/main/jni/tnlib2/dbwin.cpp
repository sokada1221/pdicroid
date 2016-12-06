#include "stdafx.h"
#include	<windows.h>
#include	"bc.h"
#pragma	hdrstop
#include "tndefs.h"

//	DBWin用デバッグモジュール

#if !defined(WIN32)
void DBW( const char *format, ... )
{
	tchar buf[ 1024 ];
	wvsprintf( buf, format, ... );
#if 1	// DBWinバージョン
	OutputDebugString( buf );
#else

#endif
}
#else	// WIN32
#if 0	// ComWin32
extern "C" {
//LONG FAR PASCAL Puts( LPSTR lpszString );	// ComWin32
typedef LONG FAR PASCAL (*FNPuts)( LPSTR lpszString );	// ComWin32
};

void DBW( const tchar *format, ... )
{
	static HINSTANCE hComWin32 = NULL;
	static FNPuts Puts = NULL;
	if ( !hComWin32 ){
		hComWin32 = LoadLibrary( "comwin32.dll" );
		if ( !hComWin32 )
			return;
		Puts = (FNPuts)GetProcAddress( hComWin32, "Puts" );
	}
#if 1
	va_list ap;
	va_start( ap, format );
	tchar buf[ 2048 ];
	wvsprintf( buf, format, ap );
//	OutputDebugString( buf );
	Puts( buf );	// ComWin32版
	va_end( ap );
#else
	tchar buf[ 512 ];
	wvsprintf( buf, format, ... );
//	OutputDebugString( buf );
	Puts( buf );	// ComWin32版
#endif
}
#else	// DbgMsg or OutputDebugString
#ifndef WINCE
static HWND hWin = NULL;
static const char *clsname = "TDbgMsgForm";
static const char *winname_wrong = "Debug Messanger";
static const char *winname = "Debug Messenger";
#ifdef _UNICODE
static const tchar *clsnameW = _T("TDbgMsgForm");
static const tchar *winnameW_wrong = _T("Debug Messanger");
static const tchar *winnameW = _T("Debug Messenger");
#else
#define	clsnameW	clsname
#define	winnameW_wrong	winname_wrong
#define	winnameW	winname
#endif
#endif
#if 0
static int FindWin( )
{
	return hWin ? 1 : 0;
}
#endif
#ifdef WINCE
void DBW( const char *format, ... )
{}	// dummy
#else
// ANSI version
void DBW( const char *format, ... )
{
	if ( !hWin ){
		hWin = FindWindowA( clsname, winname );
		if ( !hWin ){
			hWin = FindWindowA( clsname, winname_wrong );
			if ( !hWin ) return;
		}
	}
	va_list ap;
	va_start( ap, format );
	char buf[ 2048 ];
	wvsprintfA( buf, format, ap );
	COPYDATASTRUCT cds;
	cds.dwData = 1;	// Indicate String
	cds.cbData = strlen(buf);
	cds.lpData = buf;
	SendMessageA( hWin, WM_COPYDATA, (WPARAM)GetActiveWindow(), (LPARAM)&cds );
	va_end( ap );
}
#endif
#ifdef _UNICODE
void DBW( const tchar *format, ... )
{
#ifndef WINCE
	if ( !hWin ){
		hWin = FindWindow( clsnameW, winnameW );
		if ( !hWin ){
			hWin = FindWindow( clsnameW, winnameW_wrong );
			if ( !hWin ) return;
		}
	}
#endif
	va_list ap;
	va_start( ap, format );
	tchar buf[ 2048 ];
	wvsprintf( buf, format, ap );
#ifdef WINCE
	_tcscat( buf, _T("\n") );
	OutputDebugString( buf );
#else
	COPYDATASTRUCT cds;
	cds.dwData = 1;	// Indicate String
	cds.cbData = lstrlen(buf);
	cds.lpData = buf;
	SendMessage( hWin, WM_COPYDATA, (WPARAM)GetActiveWindow(), (LPARAM)&cds );
#endif
	va_end( ap );
}
#endif
#ifndef WINCE
void D8( const void *data, int len )
{
	if ( !hWin ){
		hWin = FindWindow( clsnameW, winnameW );
		if ( !hWin ){
			hWin = FindWindow( clsnameW, winnameW_wrong );
			if ( !hWin ) return;
		}
	}
	COPYDATASTRUCT cds;
	cds.dwData = 2;	// Indicate byte dump
	cds.cbData = len;
	cds.lpData = (void*)data;
	SendMessage( hWin, WM_COPYDATA, (WPARAM)GetActiveWindow(), (LPARAM)&cds );
}
#endif	// !WINCE
#endif	// !ComWin32
#endif	// WIN32

