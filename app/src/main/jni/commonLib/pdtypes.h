#ifndef	__PDTYPES_H
#define	__PDTYPES_H

#include "defs.h"

#ifdef PDICW
typedef tuchar t_charset;

// Ansi文字列処理
// システム依存のコードを使用する場合
inline tchar *A_next( const tchar *s )
{
#if defined(UNIX) && !defined(__ANDROID__)
	return _ismbblead(*(uchar*)s) ? (tchar*)(s+2) : (tchar*)(s+1);
#else	// !UNIX
	return NEXT_CHAR( s );
#endif
}
int A_strcmp( const tchar *s1, const tchar *s2 );
int A_stricmp( const tchar *s1, const tchar *s2 );
int A_strncmp( const tchar *s1, const tchar *s2 );
tchar *A_strstr( const tchar *s1, const tchar *s2 );
tchar *A_strchr( const tchar *s1, int c );
#ifdef __WIN32__
inline unsigned short A_lower( unsigned short us )
	{ return (unsigned short)::CharLower( (tchar *)us ); }
#ifndef _UNICODE
inline unsigned short A_lower( tchar c )
	{ return A_lower( (unsigned short)(tuchar)c ); }
#endif	// !_UNICODE
inline unsigned short A_upper( unsigned short us )
	{ return (unsigned short)::CharUpper( (tchar *)us ); }
inline BOOL A_isalpha( unsigned short us )
	{ return IsCharAlpha( us ); }
#else	// !__WIN32__
#if defined(UNIX)
#if !defined(UNICODE) || defined(__UTF8)
inline unsigned short A_lower( unsigned short us );
#endif
inline unsigned short A_lower( tchar c )
	{ return (uchar)c >= 'A' && (uchar)c <= 'Z' ? c + 0x20 : c; }
unsigned short A_upper( unsigned short us );
inline BOOL A_isalpha( unsigned short us )
	{ return isalpha( us ); }
#else	// !UNIX
inline unsigned short A_lower( unsigned short us )
	{ return (unsigned short)::AnsiLower( (LPTSTR)us ); }
inline unsigned short A_lower( tchar c )
	{ return A_lower( (ushort)(uchar)c ); }
inline unsigned short A_upper( unsigned short us )
	{ return (unsigned short)::AnsiUpper( (LPTSTR)us ); }
#endif	// !UNIX
#endif

// マルチ言語文字列処理
// フォントによって処理を帰る必要がある場合
tchar *M_next( const tchar *s, t_charset );
int M_strcmp( const tchar *s1, const tchar *s2, t_charset );
int M_stricmp( const tchar *s1, const tchar *s2, t_charset );
int M_strncmp( const tchar *s1, const tchar *s2, t_charset );
tchar *M_strstr( const tchar *s1, const tchar *s2, t_charset );
tchar *M_strchr( const tchar *s1, int c, t_charset );
unsigned short M_lower( unsigned short us );
unsigned short M_upper( unsigned short us );

// UNICODEを使用する部分

// シングルバイト文字文字列処理
// 純粋なシングルコードのみを扱う場合
tchar *S_strcmp( const tchar *s1, const tchar *s2 );
inline bool strequ( const tchar *s1, const tchar *s2 )
	{ return _tcscmp( s1, s2 ) == 0; }
inline bool strnequ( const tchar *s1, const tchar *s2, int n )
	{ return _tcsncmp( s1, s2, n ) == 0; }
#if defined(_UNICODE) && !defined(__UTF8)
inline bool strequ( const char *s1, const char *s2 )
	{ return strcmp( s1, s2 ) == 0; }
#endif
#ifdef DIC_BOCU
inline bool strequ( const unsigned char *s1, const unsigned char *s2 )
	{ return strcmp( (const char*)s1, (const char*)s2 ) == 0; }
#endif
tuchar S_lower( tuchar us );
tuchar S_upper( tuchar us );

struct CharSetS {
	t_charset word;
	t_charset japa;
	t_charset pron;
	t_charset exp;
};
#else	// !PDICW
inline bool strequ( const tchar *s1, const tchar *s2 )
	{ return _tcscmp( s1, s2 ) == 0; }
#define	A_lower(x)		_totlower(x)
//#define	A_upper(x)		_totupper(x)
#define	A_isalpha(x)	isalpha(x)
inline tchar *A_next( const tchar *s ) { return ::NEXT_CHAR( s ); }
#endif

#endif	// __PDTYPES_H

