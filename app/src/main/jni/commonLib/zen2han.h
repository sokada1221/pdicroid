#ifndef	__ZEN2HAN_H
#define	__ZEN2HAN_H

#ifndef __JTYPE_H
#include "jtype.h"
#endif


#ifdef _UNICODE
#define	zen2han( c )	(c)
#else
unsigned short zen2han( unsigned short c );
#endif


#if 0
// 純粋に漢字コードであるかどうか
inline int is_kanji( unsigned short c )
{
	if ( c >= 0x889f && c <= 0xeaa4 )
		return 1;
	return 0;
}
#endif

// 記号であるかどうか(半角スペースを含む）
inline int is_symbol( unsigned short c )
{
	if ( ( c >= ' ' && c < '0' ) || ( c > '9' && c < 'A' ) || ( c > 'Z' && c < 'a' ) || ( c > 'z' && c < 0x80 )
#ifdef _UNICODE
	// zenkaku symbol
	|| (c>=0xFF01 && c<=0xFF0F) || (c>=0xFF1A && c<=0xFF20) || (c>=0xFF3B && c<=0xFF40) || (c>=0xFF5B && c<=0xFF65) || c>=0xFFE0
#endif
	){
		return 1;
	}
	return 0;
}

//がのにを　の判別
inline int is_ganoniwo( unsigned short c )
{
	if (c == CODE_GA || c == CODE_NO || c == CODE_NI || c == CODE_WO ){
		return 1;
	}
	return 0;
}

//～…‥の判別
inline int is_nami(unsigned short c)
{
	if (c == CODE_NAMI1 || c == CODE_NAMI2 || c == CODE_NAMI3 ){
		return 1;
	}
	return 0;
}


#endif	// __ZEN2HAN_H
