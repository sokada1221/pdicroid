#include "stdafx.h"
#include "tndefs.h"
#pragma hdrstop
#ifdef _Windows
#include <tchar.h>
#endif
#include <string.h>
#include <stdlib.h>
#include "strlib.h"

#ifndef _UNICODE
#include <mbstring.h>	// for _mbsbtype
#endif

const wchar_t *skipspc( const wchar_t *str )
{
	while ( *str && (*str) <= ' ' ) str++;
	return str;
}
const char *skipspc(const char *str)
{
	while ( *str && ((unsigned char)*str) <= (unsigned char)' ' ) str++;
	return str;
}

const char *skipto(const char *str, char c)
{
	while ( *str && *str!=c ) str++;
    return (char*)str;
}

const wchar_t *skipto(const wchar_t *str, wchar_t c)
{
	while ( *str && *str!=c ) str++;
    return (wchar_t*)str;
}

/*======================================================================//
//	atox()
//----------------------------------------------------------------------//
// Description:
//	１６進数文字列を数値に変換する
// Arguments:
//
// Return Value:
//
//======================================================================*/
int atox( const char *str, const char **next, int defval )
{
	unsigned val = 0;
	const char *_str = str;
	for (;;)
	{
		unsigned char c = (unsigned char)toupper( *str );
		if ( c >= '0' && c <= '9' ){
			val = (val<<4) + (c - '0');
		} else if ( c >= 'A' && c <= 'F' ){
			val = (val<<4) + ( c - 'A' + 10 );
		} else {
			break;
		}
		str++;
	}
	if ( next ) *next = (char*)str;
	if ( _str == str ) return defval;
	return val;
}
/*======================================================================//
//	atod()
//----------------------------------------------------------------------//
// Description:
//	１０進数文字列を数値に変換する
//	0x[0-9a-fA-F]+ : 16進数
//	[0-9]+ : 10進数
// Arguments:
//
// Return Value:
//
//======================================================================*/
int atod( const char *str, const char **next, int defval )
{
	unsigned val = 0;
	const char *_str = str;
	for (;;)
	{
		unsigned char c = (unsigned char)*str;
		if ( c >= '0' && c <= '9' ){
			val = (val<<4) + (c - '0');
		} else {
			break;
		}
		str++;
	}
	if ( next ) *next = (char*)str;
	if ( _str == str ) return defval;
	return val;
}
/*======================================================================//
//	atov()
//----------------------------------------------------------------------//
// Description:
//	数値文字列を数値に変換する
//	0x[0-9a-fA-F]+ : 16進数
//	[0-9]+ : 10進数
// Arguments:
//
// Return Value:
//
//======================================================================*/
int atov( const char *str, const char **next, int defval )
{
	if (str[0]=='0' && str[1]=='x')
		return atox( str+2, next, defval );
	return atod( str, next, defval );
}
/*======================================================================//
//	itos()
//----------------------------------------------------------------------//
// Description:
//	signed 数値を10進数文字列に
// Arguments:
//
// Return Value:
//	コピーした文字列の終端アドレス
//======================================================================*/
char *itosA( int value, char *buf )
{
	int v = value;
	int n;	// 桁数
	if ( v < 0 ){
		buf[0] = '-';
		n = 1;
		value = -value;
	} else {
		n = 0;
	}
	for(;v!=0;){
		v /= 10;
		n++;
	}
	if ( n == 0 ){
		*buf++ = '0';
		*buf = '\0';
		return buf;
	}
	char *dp = buf + n;
	*dp-- = '\0';
	for(;value!=0;){
		*dp-- = (char)((value % 10) + '0');
		value /= 10;
	}
	return buf + n;
}

wchar_t *itosW( int value, wchar_t *buf )
{
	int v = value;
	int n;	// 桁数
	if ( v < 0 ){
		buf[0] = '-';
		n = 1;
		value = -value;
	} else {
		n = 0;
	}
	for(;v!=0;){
		v /= 10;
		n++;
	}
	if ( n == 0 ){
		*buf++ = '0';
		*buf = '\0';
		return buf;
	}
	wchar_t *dp = buf + n;
	*dp-- = '\0';
	for(;value!=0;){
		*dp-- = (wchar_t)((value % 10) + '0');
		value /= 10;
	}
	return buf + n;
}

tnstr itos(int value)
{
	tchar buf[21];
	itos(value, buf);
	return tnstr(buf);
}

tchar *jstrcut( tchar *str, size_t len )
{
	if ( _tcslen( str ) > len ){
#ifndef _UNICODE
		if ( _mbsbtype( (tuchar*)str, len-1 ) == _MBC_LEAD ){
			str[len-1] = '\0';
		} else
#endif
		{
			str[len] = '\0';
		}
	}
	return str;
}


// global variable //
tchar *strptr = NULL;

tchar *csvstr( tchar *str )
{
	static tchar *sstr = NULL;

	if ( str )
		sstr = str;
	if ( !*sstr )
		return (tchar*)_T("");

	while ( *sstr ){
		if ( *sstr != ' ' || *sstr != '\t' ){
			break;
		}
		sstr = NEXT_CHAR( sstr );
	}

	tchar *rstr;
	if ( *sstr == '"' ){
		sstr++;
		rstr = sstr;
		while ( *sstr ){
			if ( *sstr == '"' ){
				if ( *(sstr+1) == '"' && *(sstr+2) != ',' && !*(sstr+2) ){
					memmove( sstr, sstr+1, _tcsbyte( sstr ) );
				} else {
					*sstr++ = '\0';
					if ( *sstr == ',' ) sstr++;
					break;
				}
			}
			sstr = NEXT_CHAR( sstr );
		}
	} else {
		rstr = sstr;
		while ( *sstr ){
			if ( *sstr == ',' ){
				*sstr++ = '\0';
				break;
			}
			sstr = NEXT_CHAR( sstr );
		}
	}
	return rstr;
}

// 終端付き_tcsncpy
void strncpy2( tchar *dest, const tchar *src, int maxlen )
{
	_tcsncpy( dest, src, maxlen );
	dest[ maxlen ] = '\0';
}

// Str -> CSV
tchar *strcsv( tchar *dp, const tchar *src )
{
	*dp++ = '"';

	while (*src){
		if ( *src == '"' ){
			*dp++ = '"';
		}
		const tchar *_src = NEXT_CHAR( src );
		while ( _src != src ) *dp++ = *src++;
	}
	*dp++ = '"';
	*dp++ = ',';
	return dp;
}

// Val -> CSV
tchar *valcsv( tchar *dp, int val )
{
#ifdef WINCE
	wsprintf( dp, _T("%d"), val );
#else
	_itot( val, dp, 10 );
#endif
	dp += _tcslen( dp );
	*dp++ = ',';
	return dp;
}

#if 0	//TODO: mapを使うと例外が使えない error →compile optionに例外を追加する必要有り
//TODO: 最適化していない
//TODO: sepa前後の空白処理は未定義
int split(tnstr_map &m, const tchar *s, tchar sepa, tchar pair_sepa)
{
	tchar tmp[2];
	tnstr_vec vec;
	tmp[0] = sepa;
	tmp[1] = '\0';
	vec.split(s, tmp);

	tmp[0] = pair_sepa;
	foreach_tnstr_vec(vec, it){
		tnstr_vec pair;
		pair.split(it->c_str(), tmp);
		if (pair.size()>0){
			m[pair[0]] = pair.size()==1?NULL:pair[1].c_str();
		}
	}
	return m.size();
}
#endif

tchar *getword( tchar *str )
{
	tuchar c = (tuchar)*str;
	if ( c == '_' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z' ) ){
		do {
			c = (tuchar)*++str;
		} while ( c == '_' || (c>='A' && c<='Z') || (c>='a' && c<='z') || (c>='0'&&c<='9') );
		if ( c ){
			return str+1;
		} else {
			return str;
		}
	}
	return NULL;
}

void cutcomment( tchar *str, const tchar *delim )
{
	tchar *tail = NULL;
	while ( *str ){
		if ( !_tcscmp( str, delim ) ){
			if ( tail ){
				*tail = '\0';
			} else {
				*str = '\0';
			}
			return;
		}
		if ( (tuchar)*str <= ' ' ){
			if ( !tail ) tail = str;
		} else tail = NULL;
		str = NEXT_CHAR( str );
	}
	if ( tail ) *tail = '\0';
}
#ifdef _Windows
wchar_t *ansi_to_uni(const char *str)
{
	int l = strlen(str);
	wchar_t *wbuf = new wchar_t[l+1];
	if (!wbuf)
		return NULL;
	int r = MultiByteToWideChar(CP_ACP, 0, str, l, wbuf, l);
	wbuf[r] = '\0';
	return wbuf;
}
char *uni_to_ansi(const wchar_t *str)
{
	int l = wcslen(str);
	char *buf = new char[l+1];
	if (!buf)
		return NULL;
	WideCharToMultiByte(CP_ACP, 0, str, l, buf, l, NULL, NULL);
	return buf;
}

// for C++Builder bug
tchar __totupper(tchar ch)
{
	DWORD c = ch;
	return (tchar)CharUpper((LPTSTR)c);
}

// RTLにbugあるため (wcsicmpは使用禁止！）
int __wcsicmp(const wchar_t *str1, const wchar_t *str2)
{
	wchar_t c1, c2;

	while ((c1 = (wchar_t)__totupper(*str1)) == (c2 = (wchar_t)__totupper(*str2)) && c1 != '\0')
	{
		str1++;
		str2++;
	}
	return (c1 - c2);
}

int __wcsnicmp(const wchar_t *str1, const wchar_t *str2, int n)
{
	wchar_t c1=0, c2=0;

	while (n>0 && ((c1 = (wchar_t)__totupper(*str1)) == (c2 = (wchar_t)__totupper(*str2)) && c1 != '\0'))
	{
		str1++;
		str2++;
		n--;
	}
	return (c1 - c2);
}
#endif

