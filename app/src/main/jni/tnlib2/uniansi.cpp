#include "tnlib.h"
#pragma hdrstop
#include "uniansi.h"

void AnsiToUniTemp( UINT cp, const char *str1, wchar_t *str2, int size )
{
	int len = AnsiToUni( cp, str1, str2, 0 );
	wchar_t *tmp = new wchar_t[ len+1 ];
	AnsiToUni( cp, str1, tmp, len+1 );
	wcsncpy( str2, tmp, size );
    if ( size < len )
		str2[size] = '\0';
	delete[] tmp;
}
void AnsiToUniTemp( UINT cp, const char *str1, tnstr &str2, int size )
{
	int len = AnsiToUni( cp, str1, NULL, 0 );
	wchar_t *tmp = new wchar_t[ len+1 ];
	AnsiToUni( cp, str1, tmp, len+1 );
    if ( size < len )
		tmp[size] = '\0';
	str2.set( tmp );
	delete[] tmp;
}
void UniToAnsiTemp( UINT cp, const wchar_t *str1, char *str2, int size )
{
	int len = UniToAnsi( cp, str1, str2, 0 );
	char *tmp = new char[ len+1 ];
	UniToAnsi( cp, str1, tmp, len+1 );
	strncpy( str2, tmp, len );
    if ( size < len )
		str2[size] = '\0';
	delete[] tmp;
}

