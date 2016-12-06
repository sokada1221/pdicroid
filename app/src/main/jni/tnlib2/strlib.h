#ifndef __STRLIB_H
#define	__STRLIB_H

#include "tnstr.h"

//
//	文字列解析など、汎用性の高い関数ライブラリ
//

// 標準ライブラリ準拠関数 //
void strncpy2( tchar *dest, const tchar *src, int maxlen );

// 基本的な関数
const wchar_t *skipspc( const wchar_t *str );			// \t, \sをskip
inline wchar_t *skipspc(wchar_t *str)
	{ return (wchar_t*)skipspc(str); }
const char *skipspc(const char *str);
inline char *skipspc(char *str)
	{ return (char*)skipspc(str); }
const char *skipto(const char *str, tchar c);
inline char *skipto(char *str, char c) { return (char*)skipto((const char*)str, c); }
const wchar_t *skipto(const wchar_t *str, tchar c);
inline wchar_t *skipto(wchar_t *str, wchar_t c) { return (wchar_t*)skipto((const wchar_t*)str, c); }
void cutcomment( tchar *str, const tchar *delim );	// コメントを削除する

int atox( const char *str, const char **next, int defval );
int atod( const char *str, const char **next, int defval );
int atov( const char *str, const char **next, int defval );


char *itosA( int value, char *buf );
wchar_t *itosW( int value, wchar_t *buf );
#ifdef UNICODE
#define	itos	itosW
#else
#define	itos	itosA
#endif

tnstr itos(int value);

tchar *jstrcut( tchar *str, size_t len );

// CSV関連 //
tchar *csvstr( tchar *str );					// CSV -> strings(一番最初にCSV文字列を渡し、２回目以降はNULLを渡す)
tchar *strcsv( tchar *dp, const tchar *src );	// strings -> CSV
tchar *valcsv( tchar *dp, int val );			// value -> CSV

// Utilities //
int split(tnstr_map &m, const tchar *s, tchar sepa=',', tchar pair_sepa='=');	// A=a,B=b,... => map

// strptrを使用する関数
tchar *getword( tchar *str );	// [_A-Za-z][_A-Za-z0-9]*の文字列である場合は、最後の文字+1のポインターを返す
							// 先頭がword以外の文字列であると、NULLを返す

wchar_t *ansi_to_uni(const char *str);
char *uni_to_ansi(const wchar_t *str);

extern tchar *strptr;	// 次の文字列位置

#endif	// __STRLIB_H

