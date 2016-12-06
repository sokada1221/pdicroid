#ifndef __UNITYPES_H
#define	__UNITYPES_H

void Zen2Han( const wchar_t *src, wchar_t *dest );
void Zen2Han( wchar_t *str );
tchar *Zen2HanNew( const tchar *str, int len );
#ifdef WINCE
tnstr GetWindowTextStrEx( HWND hwnd );
#else
#define	GetWindowTextStrEx	GetWindowTextStr
#endif

#endif

