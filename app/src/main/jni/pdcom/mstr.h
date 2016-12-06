#ifndef __MSTR_H
#define	__MSTR_H

// WideChar <-> ANSI or UTF-8 or BOCU converter

#include "pdconfig.h"

#ifdef NEED_SINGLEBYTE
#define	MSTR_ARG_singlebyte	, bool _singlebyte
#define	MSTR_singlebyte(x)		,x
#else
#define	MSTR_ARG_singlebyte
#define	MSTR_singlebyte(x)
#endif

class __mstr {
protected:
	void *buffer;
	char *newbuffer;
	int length;
	int type;
#ifdef NEED_SINGLEBYTE
	bool singlebyte;
#endif
public:
	__mstr( const char *str MSTR_ARG_singlebyte );
	__mstr( const char *str, int len MSTR_ARG_singlebyte );
	__mstr( const wchar_t *str MSTR_ARG_singlebyte );
	~__mstr();
	operator char *();
	operator unsigned char *()
		{ return (unsigned char*)(char*)(*this); }
	operator wchar_t *();
};

#if /*(MIXDIC && !defined(DIC_UTF8)) || */ defined(MIXMJ) || defined(MIXJAPA)
#define	USE_TNMIXCHAR
class TNMixChar {
protected:
	// 前提：
	// cbufとwbufが存在する場合は必ず内容は一致する
	// chが存在する場合はcbuf = wbuf = NULLである
	char *cbuf;
	wchar_t *wbuf;
#ifdef NEED_SINGLEBYTE
	bool singlebyte;
#endif
	tnstr *ch;
public:
	TNMixChar();
	TNMixChar( const char *str MSTR_ARG_singlebyte );
	TNMixChar( const char *str, int len MSTR_ARG_singlebyte );
	TNMixChar( const wchar_t *str MSTR_ARG_singlebyte );
	~TNMixChar();
	void cat( TNMixChar &o );
	void cat( const char *str );
	void cat( const char *str, int len );			//lenまで連結
	void cat( const char *str1, const wchar_t *str2 );	// ２つの文字列を連結
	void cat( const wchar_t *str );
	void cat( const wchar_t *str, int len );			//lenまで連結
	void cat( const wchar_t *str1, const wchar_t *str2 );	// ２つの文字列を連結
	int jcut( int len );				//cutされた場合は 1が返る
	wchar_t &operator []( int i );
	char *set( const char *str );
	char *set( const char *str, int len );			//lenまでのコピー
	wchar_t *set( const wchar_t *str );
	wchar_t *set( const wchar_t *str, int len );			//lenまでのコピー
	void set( TNMixChar & );
	void clear();
	const wchar_t *operator = ( const wchar_t *str )
		{ set( str ); return wbuf; }
	const char *operator = ( const char *str )
		{ set( str ); return cbuf; }
	operator char *();
	operator unsigned char *()
		{ return (unsigned char*)(char*)(*this); }
	operator wchar_t *();
	void SetBuf( char *newbuf );
	void SetBuf( wchar_t *newbuf );
	int operator == ( TNMixChar &o )
		{ return compare( o ); }
	bool operator != ( TNMixChar &o )
		{ return compare( o )!=0; }
	int compare( TNMixChar &o );
//	TNChar &GetChar();
protected:
	void ChangeToW();
	void ChangeToC();
	void ReleaseCh();
};
#else	// !MIXDIC
#define	TNMixChar	TNChar
#endif	// !MIXDIC

#endif	// __MSTR_H

