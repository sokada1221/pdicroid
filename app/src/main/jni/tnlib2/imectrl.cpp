#include	<windows.h>
#pragma	hdrstop
#include	<ime.h>
#include	"imectrl.h"

// IME ON/OFF‚Ì§Œä
void IMEOpen( HWND hwnd, BOOL f )
{
	if ( !hwnd )
		return;
#ifdef WIN32
	HIMC hIMC = ImmGetContext( hwnd );
	ImmSetOpenStatus( hIMC, f );    // IME OPEN
	ImmReleaseContext( hwnd, hIMC );
#else
	HGLOBAL hime = GlobalAlloc( GHND | GMEM_SHARE, sizeof( IMESTRUCT ) );
	LPIMESTRUCT ime = (LPIMESTRUCT)GlobalLock( hime );
	ime->fnc = IME_SETOPEN;
	ime->wParam = f;
	GlobalUnlock( hime );
	LPARAM lParam = (UINT)hime;
#ifdef WIN32
	SendIMEMessageEx( hwnd, lParam );
#else
	SendIMEMessage( hwnd, lParam );
#endif
	GlobalFree( hime );
#endif
}

BOOL IsIMEOpened( HWND hwnd )
{
	if ( !hwnd )
		return FALSE;
#ifdef WIN32
	BOOL r;
	HIMC hIMC = ImmGetContext( hwnd );
	r = ImmGetOpenStatus( hIMC );    // IME OPEN
	ImmReleaseContext( hwnd, hIMC );
#else
	HGLOBAL hime = GlobalAlloc( GHND | GMEM_SHARE, sizeof( IMESTRUCT ) );
	LPIMESTRUCT ime = (LPIMESTRUCT)GlobalLock( hime );
	ime->fnc = IME_GETOPEN;
	ime->wParam = FALSE;
	GlobalUnlock( hime );
	LPARAM lParam = (WORD)hime;
#ifdef WIN32
	SendIMEMessageEx( hwnd, lParam );
#else
	SendIMEMessage( hwnd, lParam );
#endif
	ime = (LPIMESTRUCT)GlobalLock( hime );
	BOOL r = ime->wParam;
	GlobalUnlock( hime );
	GlobalFree( hime );
#endif
	return r;
}
