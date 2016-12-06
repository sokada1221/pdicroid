#ifndef __GBLSTR_H
#define	__GBLSTR_H

#include <string.h>
#include "tndefs.h"

template <class T>
class GlobalStringT {
protected:
	T *buf;
protected:
	void Constructor( const T *str, int len );
public:
	GlobalStringT();
	~GlobalStringT();
	GlobalStringT( const T *str, int len )
		{ Constructor( str, len ); }
	GlobalStringT( int len );
	operator const T *()
		{ return buf; }
	T *GetBuf()
		{ return buf; }
	void set( const T *str, int len );
	void cat( int buflen, const T *str, int len );
	T *c_str()
		{ return buf; }
	bool IsEmpty() const
		{ return buf[0] == '\0'; }
	bool Exist() const
		{ return buf[0] ? true : false; }
	void clear()
		{ buf[0] = '\0'; }
};

#ifdef WINCE
#define	gblALLOCATE( size )	(T*)LocalAlloc( LMEM_FIXED, size )
#define	gblFREE( handle )	LocalFree( handle )
#else
#define	gblALLOCATE( size )	(T*)GlobalAlloc( GMEM_FIXED, size )
#define	gblFREE( handle )	GlobalFree( handle )
#endif

template <class T>
GlobalStringT<T>::GlobalStringT()
{
	buf = gblALLOCATE( sizeof(T) );
	buf[0] = '\0';
}
template <class T>
GlobalStringT<T>::GlobalStringT( int len )
{
	buf = gblALLOCATE( (len + 1)*sizeof(T) );
	buf[0] = '\0';
}
template <class T>
void GlobalStringT<T>::Constructor( const T *str, int len )
{
	buf = gblALLOCATE( (len+1)*sizeof(T) );
	memcpy( buf, str, len );
}
template <class T>
GlobalStringT<T>::~GlobalStringT( )
{
	if ( gblFREE( (HLOCAL)buf ) )
	{
		// âï˙ÉGÉâÅ[î≠ê∂ÇµÇ»Ç¢ÇÕÇ∏
//		DBW("~GlobalString");
		MessageBeep( 0 );
	}
}
template <class T>
void GlobalStringT<T>::set( const T *str, int len )
{
	T *p = gblALLOCATE( (len+1)*sizeof(T) );
	if ( !p ) return;
	memcpy( p, str, len*sizeof(T) );
	gblFREE( buf );
	buf = p;
	buf[len] = '\0';
}
template <class T>
void GlobalStringT<T>::cat( int buflen, const T *str, int len )
{
	T *p = gblALLOCATE( (buflen + len + 1)*sizeof(T) );
	if ( !p )
		return;
	memcpy( p, buf, buflen*sizeof(T) );
	gblFREE( buf );
	buf = p;
	memcpy( buf+buflen, str, len*sizeof(T) );
	buf[buflen+len] = '\0';
}

class GlobalString : public GlobalStringT<tchar> {
typedef GlobalStringT<tchar> inherited;
public:
	GlobalString(){}
	GlobalString( const tchar *s ){ Constructor( s, _tcslen(s) ); }
	GlobalString( const tchar *s, int l):inherited(s,l){}
	GlobalString( GlobalString &o )
		{ Constructor( o.buf, _tcslen(o.buf) ); }
	GlobalString( int len ):inherited(len){}
	void set( const tchar *str )
		{ inherited::set( str, _tcslen(str) ); }
	void set( const tchar *str, int len )
		{ inherited::set( str, len ); }
	void cat( const tchar *str )
		{ inherited::cat( _tcslen(buf), str, _tcslen(str) ); }
	void cat( const tchar *str, int len )
		{ inherited::cat( _tcslen(buf), str, len ); }
	bool operator == ( const tchar *str )
		{ return _tcscmp( buf, str ) == 0; }
	bool operator != ( const tchar *str )
		{ return _tcscmp( buf, str ) != 0; }
	GlobalString &operator += ( const tchar *str )
		{ inherited::cat( _tcslen(buf), str, _tcslen(str) ); return *this; }
	GlobalString &operator = ( const tchar *str )
		{ inherited::set( str, _tcslen(str) ); return *this; }
};

#ifdef _UNICODE
class GlobalStringA : public GlobalStringT<char> {
typedef GlobalStringT<char> inherited;
public:
	GlobalStringA(){}
	GlobalStringA( const char *s ){ Constructor( s, strlen(s) ); }
	GlobalStringA( const char *s, int l):inherited(s,l){}
	GlobalStringA( int len ):inherited(len){}
	GlobalStringA( GlobalStringA &o )
		{ Constructor( o.buf, strlen(o.buf) ); }
	void set( const char *str )
		{ inherited::set( str, strlen(str) ); }
	void cat( const char *str )
		{ inherited::cat( strlen(buf), str, strlen(str) ); }
	void cat( const char *str, int len )
		{ inherited::cat( strlen(buf), str, len ); }
#if 0
	bool operator == ( const char *str )
		{ return strcmp( buf, str ) == 0; }
	bool operator != ( const char *str )
		{ return strcmp( buf, str ) != 0; }
#endif
	GlobalStringA &operator += ( const char *str )
		{ inherited::cat( strlen(buf), str, strlen(str) ); return *this; }
	GlobalStringA &operator = ( const char *str )
		{ inherited::set( str, strlen(str) ); return *this; }
};
#else
#define	GlobalStringA	GlobalString
#endif

#endif

