#include "tnlib.h"
#pragma hdrstop

// 0xFFxxといういやらしい半角コードを0x00xxの通常の半角コードに直す
// only for UNICODE
void Zen2Han( const wchar_t *src, wchar_t *dest )
{
	while ( *src ){
		if ( *src >= 0xFF01 && *src < 0xFF5E ){
			*dest = *src - 0xFF01 + '!';
		} else
        switch ( *src )
        {
        	case 0x201d:
            	*dest = '"';
                break;
            case 0x2019:
            	*dest = '\'';
                break;
            default:
        		*dest = *src;
                break;
        }
		dest++;
        src++;
	}
    *dest = 0;
}
void Zen2Han( wchar_t *str )
{
	Zen2Han( str, str );
}
tchar *Zen2HanNew( const tchar *str, int len )
{
	if ( len == -1 )
    	len = _tcslen( str );
	wchar_t *buf = new wchar_t[ len+1 ];
    Zen2Han( str, buf );
    return buf;
}

// Normalizeしたtextを返す
tnstr GetWindowTextStrEx( HWND hwnd )
{
	tnstr str = GetWindowTextStr( hwnd );
	Zen2Han( str.c_str() );
    return str;
}

