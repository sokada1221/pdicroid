#include "tnlib.h"
#pragma	hdrstop

void LowerToUpper( tchar *str )
{
	tuchar c = (tchar)CharUpper( (LPTSTR)MAKELONG( (tuchar)str[0], 0 ) );
	if (c==str[0]){
		CharUpper( str );
	} else {
		str[0] = c;
	}
}

