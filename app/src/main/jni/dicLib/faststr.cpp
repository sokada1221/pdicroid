#include	"tnlib.h"
#pragma	hdrstop
#include "pdconfig.h"
#include	"pdtypes.h"
#define	__INCLUDE_FASTSTR
#include	"faststr.h"

#if MIXDIC && defined(_MSC_VER)
int jfstrnicmp( const char *s1, const char *s2, int len )
{
	int l1 = min(strlen(s1),len);
	int l2 = min(strlen(s2),len);
	if ( l1 < l2 ) return -1;
	int r;
	while(l2){
		if ( _ismbblead(*s1) ){
			r = GET_HWORD(s1) - GET_HWORD(s2);
			if ( r != 0 ) return r;
			l2 -= 2;
		} else {
			r = *s1 - *s2;
			if ( r != 0 ) return r;
			l2--;
		}
	}
	return 0;
}
// strnicmp‚Ì‘ã‘ÖŠÖ”(MSC‚É‚Í‚È‚¢H)
int fstrnicmp( const char *s1, const char *s2, int len )
{
	int l1 = min(strlen(s1),len);
	int l2 = min(strlen(s2),len);
	if ( l1 < l2 ) return -1;
	int r;
	while(l2){
		r = *s1 - *s2;
		if ( r != 0 ) return r;
		l2--;
	}
	return 0;
}
#endif	// MIXDIC && defined(_MSC_VER)

