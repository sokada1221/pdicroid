#include "tnlib.h"
#pragma	hdrstop
#include "jtype.h"

// 不正コードの除去
bool elimchar( tchar *p )
{
	bool changed = false;

	while ( *p ){
		if ( ((tuchar)*p) < ' ' ){
			tuchar *q = p+1;
			while ( ((tuchar)*q) < ' ' && *q) q++;
			memmove( p, q, _tcsbyte1( q ) );
			changed = true;
		}
		p++;
	}
	return changed;
}

// 不正コードの空白置換
bool elimcharspc( tchar *p )
{
	bool changed = false;

	while ( *p ){
		if ( ((tuchar)*p) < ' ' ){
			*(tuchar*)p = ' ';
			changed = true;
		}
		p++;
	}
	return changed;
}
