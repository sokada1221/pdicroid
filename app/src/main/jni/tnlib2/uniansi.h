#ifndef __UNIANSI_H
#define	__UNIANSI_H

#ifdef _UNICODE
	#ifdef UNIANSI	// Mixture
		#define	mchar	char
		#define	_M(x)	x
		#define	_F(x)	x##A
		#define	_FA(x)	x##M	// uses alternating function
	#else	// pure UNICODE
		#define	mchar	tchar
		#define	_M(x)	_T(x)
		#define	_F(x)	x
		#define	_FA(x)	x
	#endif
#else	// ANSI
	#define	mchar	char
	#define	_M(x)	x
	#define	_F(x)	x
	#define	_FA(x)	x
#endif

#define	LPMSTR	mchar *
#define	LPCMSTR	const mchar *
#define	MCHAR	mchar

#include "tnstr.h"

////////////////////////////////////////////////
// å›ä∑ê´Çà€éùÇ∑ÇÈÇΩÇﬂÇÃíËã`
////////////////////////////////////////////////
#define	UniToAnsi( cp, str1, str2, size )	WideCharToMultiByte( cp, 0, str1, -1, str2, size, NULL, NULL )
#define	AnsiToUni( cp, str1, str2, size )	MultiByteToWideChar( cp, 0, str1, -1, str2, size )
void UniToAnsiTemp( unsigned cp, const wchar_t *str1, char *str2, int size );
void AnsiToUniTemp( unsigned cp, const char *str1, wchar_t *str2, int size );
void AnsiToUniTemp( unsigned cp, const char *str1, tnstr &str2, int size );
#ifdef UNIANSI
#define	ConvertAnsiToUni( cp, str1, str2, size )	\
	tchar str2[ size+1 ]; \
	if ( !str1 || !str1[0] ){ \
		str2[0] = '\0'; \
	} else { \
		if ( AnsiToUni( cp, str1, str2, size+1 ) == 0 ){ \
			AnsiToUniTemp( cp, str1, str2, size ); \
		} \
	}
#define	ConvertUniToAnsi( cp, str1, str2, size )	\
	char str2[ size+1 ]; \
	if ( !str1 || !str1[0] ){ \
		str2[0] = '\0'; \
	} else { \
		if ( UniToAnsi( cp, str1, str2, size+1 ) == 0 ){ \
			UniToAnsiTemp( cp, str1, str2, size ); \
		} \
	}

#else
#define	ConvertAnsiToUni( cp, str1, str2, size )	const tchar *str2 = str1;
#define	ConvertUniToAnsi( cp, str1, str2, size )	const tchar *str2 = str1;
#endif


//////////////////////////////////////////////////////////
// Alternating functions
//////////////////////////////////////////////////////////
#ifdef UNIANSI
#endif

#endif
