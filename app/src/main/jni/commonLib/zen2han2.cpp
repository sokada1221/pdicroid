#include	"tnlib.h"
#pragma	hdrstop
#include	"zen2han2.h"
#include	"defs.h"

#ifdef	_WINDOWS
void Zen2Han( const tchar *str, tchar *dp )
{
	tchar *np;
	while ( *str ){
		np = CharNext( str );
		if ( STR_DIFF( np, str ) == 2 ){
			ushort c = zen2han( (ushort)((ushort)( (ushort)*str << 8 ) + (tuchar)*(str+1)) );
			if ( c > 0x100 ){
				*dp++ = (tchar)(c >> 8);
			}
			*dp++ = (tchar)(c & 0xff);
			str = np;
		} else {
			while ( str != np ){
				*dp++ = *str++;
			}
		}
	}
	*dp = '\0';
}
#endif

