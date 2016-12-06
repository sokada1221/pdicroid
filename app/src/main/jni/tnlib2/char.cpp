#include "tndefs.h"
#include "char.h"
#include "strlib.h"

#include <windows.h>	// Windowsコードがあるため暫定措置

// 必ず呼ばれるもの

__Char::__Char( )
{
	buf = new tchar[ 1 ];
	buf[0] = '\0';
}

__Char::__Char( const tchar *str, int len )
{
	buf = NULL;
    set( str, len );
}

void __Char::Constructor( const tchar *str )
{
	buf = new tchar[ 1 ];
	set( str );
}
__Char::__Char( const tchar c )
{
	buf = new tchar[2];
	buf[0] = c;
	buf[1] = '\0';
}


__Char::~__Char()
{
	if ( buf ){
		delete[] buf;
		buf = 0;
	}
}

// 戻り値はstrの終端NULLのポインタ
tchar *__Char::set( const tchar *str )
{
	return set( str, str ? _tcslen( str ) : 0 );
}

// 戻り値は&str[len]の終端NULLのポインタ
tchar *__Char::set( const tchar *str, int len )
{
	tchar *p = new tchar[ len + 1 ];
	if ( !p )
		return NULL;
	if (len)
		memcpy( p, str, LENTOBYTE(len) );
	p[len] = '\0';
	if (buf){
		delete[] buf;
	}
	buf = p;
	return str ? (tchar*)str + len : NULL;
}
#ifdef UNICODE
bool __Char::set(const char *str)
{
	wchar_t *s = ansi_to_uni(str);
	if (!s)
		return false;
	SetBuf(s);
	return true;
}
#endif
void TNChar::cat( const tchar *str )
{
	cat( str , _tcslen( str ) );
}

void TNChar::cat( const tchar *str, int len )
{
	int l = _tcslen( buf );
	tchar *p = new tchar[ l + len + 1 ];
	if ( !p )
		return;
	memcpy( p, buf, LENTOBYTE(l) );
	delete[] buf;
	buf = p;
	memcpy( buf+l, str, LENTOBYTE(len) );
	buf[l+len] = '\0';
}

tchar *TNChar::get( tchar *str, int len )
{
	_tcsncpy( str, buf, len );
	str[len] = '\0';
	return str;
}

int TNChar::is_clear2( )
{
	tchar *p = buf;
	while ( *p ){
		if ( *p != ' ' )
			return 0;
		p++;
	}
	return 1;
}

void TNChar::trunc( int c )
{
	tchar *p = buf + _tcslen( buf );
	while ( p != buf ){
		if ( *p == c ){
			*p = '\0';
		}
		p--;
	}
}
// char 日本語処理用

int TNChar::jcut( int len )
{
	int l = _tcslen( buf );
	jstrcut( buf, len );
	return ( l > len )?1:0;
}

void TNChar::MoveBuf( TNChar *obj )
{
	delete[] buf;
	buf = obj->buf;
	obj->buf = 0;
	delete obj;
}
void __Char::cat( const tchar *str1, const tchar *str2 )
{
	int l = _tcslen( buf );
	int len1 = _tcslen( str1 );
	int len2 = _tcslen( str2 );
	tchar *p = new tchar[ l + len1 + len2 + 1 ];
	if ( !p )
		return;
	memcpy( p, buf, LENTOBYTE(l) );
	delete[] buf;
	buf = p;
	memcpy( buf+l, str1, LENTOBYTE(len1) );
	memcpy( buf+l+len1, str2, LENTOBYTE(len2) );
	buf[l+len1+len2] = '\0';
}

void __Char::operator = (int val)
{
	delete[] buf;
	tchar *newbuf = new tchar[12];	// int = longの場合
	itos( val, newbuf );
	buf = newbuf;
}

