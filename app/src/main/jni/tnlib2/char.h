#ifndef	__TNCHAR_H
#define	__TNCHAR_H

#include <string.h>
#include "tndefs.h"

class __Char {
protected:
	tchar *buf;
private:
	void Constructor( const tchar *str );
public:
	__Char( );
	__Char( const tchar *str )
		{ Constructor( str ); }
	__Char( const tchar *str, int len );
	__Char( __Char &o )
		{ Constructor( o.buf ); }
	__Char( const tchar c );
#ifdef	_WINDOWS
//	__Char( class TEdit *edit );
#endif
	~__Char();
	tchar *set( const tchar *str );
	tchar *set( const tchar *str, int len );			//lenまでのコピー
	void cat( const tchar *str );
	void cat( const tchar *str, int len );			//lenまで連結
	void cat( const tchar *str1, const tchar *str2 );	// ２つの文字列を廉潔
#if !__DLL__
//		tchar *get( tchar *str )
//			{ return lstrcpy( str, buf ); }
#endif
	tchar *get( tchar *str, int len );
	int length() const
		{ return __tcslen(buf); }
	int Length() const
		{ return length(); }
	int GetLength() const
		{ return length(); }
	operator const tchar *()	const {return buf;}
	operator const tuchar *() { return (const tuchar*)buf; }
	operator const void *() { return (const void*)buf; }
	tchar *c_str()	{ return buf; }
	void clear( void )		{buf[0] = '\0';}
	int is_clear( void )	{ return buf[0] == '\0'; }
	bool IsEmpty()			{ return buf[0] == '\0'; }
	void trunc( int c );	// 後ろの余分なcを削る
	int is_clear2( void );		// スペースのみでは初期化されていると判断
	int jcut( int len );				//cutされた場合は 1が返る
	tchar &operator []( int i )	{return buf[i];}
	const tchar *operator = ( const tchar *str )	{set( str ); return buf;}
	void operator = (int val);
	__Char &operator = ( __Char &obj ) { this->set( (const tchar *)obj ); return *this; }
#ifdef UNICODE
	bool setA(const char *str);
#endif
#if defined(UNICODE) && !defined(__UTF8)
	bool set(const char *str);
#endif
	const tchar *operator += ( const tchar *str )	{cat( str ); return buf;}
	bool operator == ( const tchar *s )
		{ return __tcscmp(buf,s) == 0; }
	__Char &operator + ( __Char &obj )
		{ cat(obj); return *this; }
//		__Char &operator + ( const tchar *s )
//			{ cat(s); return *this; }
	int operator == ( const __Char &o )
#if defined(UNICODE) && !defined(__UTF8)
		{ return !wcscmp( buf, o.buf ); }
#else
		{ return !strcmp( buf, o.buf ); }
#endif
	int operator != ( const __Char &o )
#if defined(UNICODE) && !defined(__UTF8)
		{ return wcscmp( buf, o.buf ); }
#else
		{ return strcmp( buf, o.buf ); }
#endif
//		operator const __Char * ( const tchar *str )	{return new __Char( str );}
	tchar *get_buf( )
		{ return buf; }

	void __SetBuf__( tchar *newbuf )
	{
		delete[] buf;
		buf = newbuf;
	}
	void SetBuf( tchar *newbuf )
	{
		delete[] buf;
		buf = newbuf;
	}
	tchar *SetBufLength(int len)
	{
		delete[] buf;
		return buf = new tchar[len+1];
	}
	tchar *Discard()
		{ tchar *b = buf; buf = (tchar*)0; return b; }
	void ReplaceBuf( tchar *newbuf )
			{ buf = newbuf; }
	void MoveBuf( __Char *obj );

// Windows用追加ルーチン
#ifdef	_WINDOWS
#ifdef	WIN16NP
	void set( const tchar far *str );
	void set( const tchar far *str, int len );
#endif
//	void set( class TEdit *edit );
	int ansicut( int len );
	void toupper( );
#endif
};


// 固定長__Char
class FixChar {
protected:
	tchar *buf;
public:
	int maxlen;
private:
	void Constructor( const tchar *str, int _maxlen );
public:
	FixChar( int _maxlen );
	FixChar( const tchar *str, int maxlen );
	~FixChar();
	void padchar( int c );		// 残りをlenまで c 文字でうめる
	void set( const tchar *str );
	void set( const tchar *str, int len );
	operator tchar *()	{return buf;}
	void clear( void )		{buf[0] = '\0';}
	int get_maxlen( ) { return maxlen; }
	tchar &operator []( int i )	{return buf[i];}
	tchar *get_buf()	{ return buf; }
};



//__Charとしては扱えないので注意
class StreamChar : public __Char {
protected:
	const tchar *readptr;
public:
	StreamChar(){readptr = buf;}
	StreamChar( const tchar *str ):__Char( str ){readptr = buf;}
	void reset( void )	{readptr = buf;}
	void set_readptr( const tchar *ptr )	{readptr = ptr;}
	const tchar *operator ++()	{return readptr++;}
	const tchar *operator --()	{return readptr--;}
	operator const tchar *()	{return readptr;}
	const tchar *operator = ( const tchar *ptr )	{return readptr = ptr;}
};

#define TNChar	__Char

#endif	// __TNCHAR_H

