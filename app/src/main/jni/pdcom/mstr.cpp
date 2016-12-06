#include "tnlib.h"
#pragma hdrstop
#include "mstr.h"
#include "utf.h"
//#include "dicmix.h"

__mstr::__mstr( const char *str MSTR_ARG_singlebyte )
{
	buffer = (void*)str;
	length = strlen(str);
	type = 1;
	newbuffer = NULL;
#ifdef NEED_SINGLEBYTE
	singlebyte = _singlebyte;
#endif
}
__mstr::__mstr( const char *str, int len MSTR_ARG_singlebyte )
{
	buffer = (void*)str;
	length = len;
	type = 1;
	newbuffer = NULL;
#ifdef NEED_SINGLEBYTE
	singlebyte = _singlebyte;
#endif
}
__mstr::__mstr( const wchar_t *str MSTR_ARG_singlebyte )
{
	buffer = (void*)str;
	length = wcslen(str);
	type = 2;
	newbuffer = NULL;
#ifdef NEED_SINGLEBYTE
	singlebyte = _singlebyte;
#endif
}
__mstr::~__mstr()
{
	if ( newbuffer ) delete[] newbuffer;
}
#if !defined(UNIX) && !defined(DIC_UTF8)
__mstr::operator char *()
{
	if ( type == 1 ) return (char*)buffer;
	if ( newbuffer ) return (char*)newbuffer;

	// Unicode to Ansi

#ifdef NEED_SINGLEBYTE
	if (singlebyte){
		*(uchar**)&newbuffer = new uchar[ length + 1 ];
		uchar *dp = (uchar*)newbuffer;
		wchar_t *sp = (wchar_t*)buffer;
		for(;*sp;){
#if 0	// for debug
			if (*sp >= 0x100){
				// illegal charactor
			}
#endif
			*dp++ = (uchar)*sp++;
		}
		*dp = '\0';
	} else
#endif	// NEED_SINGLEBYTE
	{
#ifdef DIC_UTF8
		newbuffer = new char[ (length<<2) + 1 ];
		int size = UTF16toUTF8( (wchar_t*)buffer, length, newbuffer );
#else
		newbuffer = new char[ length*2 + 1 ];
		int size = _WideCharToMultiByte( CP_ACP, 0, (wchar_t*)buffer, length, newbuffer, length*2, NULL, NULL );
#endif
		newbuffer[size] = '\0';
	}
	return (char*)newbuffer;
}
__mstr::operator wchar_t *()
{
	if ( type == 2 ) return (wchar_t*)buffer;
	if ( newbuffer ) return (wchar_t*)newbuffer;

	// Ansi to Unicode

#ifdef NEED_SINGLEBYTE
	if (singlebyte){
		*(wchar_t**)&newbuffer = new wchar_t[ length*2 + 1 ];
		uchar *sp = (uchar*)buffer;
		wchar_t *dp = (wchar_t*)newbuffer;
		for(;*sp;){
			*dp++ = *sp++;
		}
		*dp = '\0';
	} else
#endif	// NEED_SINGLEBYTE
	{
		*(wchar_t**)&newbuffer = new wchar_t[ length*2 + 1 ];
#ifdef DIC_UTF8
		UTF8toUTF16( (char*)buffer, length, (wchar_t*)newbuffer );
#else
		int size = _MultiByteToWideChar( CP_ACP, 0, (char*)buffer, length, (wchar_t*)newbuffer, length*2 );
		((wchar_t*)newbuffer)[size] = '\0';
#endif
	}
	return (wchar_t*)newbuffer;
}
#endif

#ifdef USE_TNMIXCHAR
TNMixChar::TNMixChar()
{
	cbuf = NULL;
	wbuf = NULL;
	ch = NULL;
#ifdef NEED_SINGLEBYTE
	singlebyte = false;
#endif
}
TNMixChar::TNMixChar( const char *str MSTR_ARG_singlebyte )
{
	cbuf = NULL;
	wbuf = NULL;
	ch = NULL;
#ifdef NEED_SINGLEBYTE
	singlebyte = _singlebyte;
#endif
	set( str );
}
TNMixChar::TNMixChar( const char *str, int len MSTR_ARG_singlebyte )
{
	cbuf = NULL;
	wbuf = NULL;
	ch = NULL;
#ifdef NEED_SINGLEBYTE
	singlebyte = _singlebyte;
#endif
	set( str, len );
}
TNMixChar::TNMixChar( const wchar_t *str MSTR_ARG_singlebyte )
{
	cbuf = NULL;
	wbuf = NULL;
	ch = NULL;
#ifdef NEED_SINGLEBYTE
	singlebyte = _singlebyte;
#endif
	set( str );
}
TNMixChar::~TNMixChar()
{
	if ( cbuf )
		delete[] cbuf;
	if ( wbuf )
		delete[] wbuf;
	if ( ch )
		delete ch;
}
void TNMixChar::cat( TNMixChar &o )
{
	if ( ch ){
		ReleaseCh();
	}
	if ( o.ch ){
		ReleaseCh();
	}
	if ( cbuf ){
		cat( (const char*)o );
		return;
	}
	if ( wbuf ){
		cat( (const wchar_t*)o );
		return;
	}
	if ( o.cbuf ){
		cat( o.cbuf, strlen(o.cbuf) );
		return;
	}
	if ( o.wbuf ){
		cat( o.wbuf, wcslen(o.wbuf) );
		return;
	}
	// no data
}
void TNMixChar::cat( const char *str )
{
	cat( str , strlen( str ) );
}
void TNMixChar::cat( const char *str, int len )
{
	if ( ch ){
		ReleaseCh();
	}
	if ( !cbuf ){
		if ( !wbuf ){
			// no data
			set( str, len );
			return;
		}
		ChangeToC();
	}
	int l = strlen( cbuf );
	char *p = new char[ l + len + 1 ];
	if ( !p )
		return;
	memcpy( p, cbuf, l );
	delete[] cbuf;

	cbuf = p;
	memcpy( cbuf+l, str, len );
	cbuf[l+len] = '\0';
	if ( wbuf ){
		delete[] wbuf;
		wbuf = NULL;
	}
}
#if 0
void TNMixChar::cat( const char *str1, const wchar_t *str2 )
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
#endif
void TNMixChar::cat( const wchar_t *str )
{
	cat( str, wcslen(str) );
}
void TNMixChar::cat( const wchar_t *str, int len )
{
	if ( ch ){
		ReleaseCh();
	}
	if ( !wbuf ){
		if ( !cbuf ){
			// no data
			set( str, len );
			return;
		}
		ChangeToW();
	}
	int l = wcslen( wbuf );
	wchar_t *p = new wchar_t[ l + len + 1 ];
	if ( !p )
		return;
	memcpy( p, wbuf, l<<1 );
	delete[] wbuf;
	wbuf = p;
	memcpy( wbuf+l, str, len<<1 );
	wbuf[l+len] = '\0';
	if ( cbuf ){
		delete[] cbuf;
		cbuf = NULL;
	}
}
void TNMixChar::cat( const wchar_t *str1, const wchar_t *str2 )
{
	if ( ch ){
		ReleaseCh();
	}
	int l;
	if ( !wbuf ){
		if ( !cbuf ){
			// no data
			l = 0;
			goto j1;
		}
		ChangeToW();
	}
	l = wcslen( wbuf );
j1:;
	int len1 = wcslen( str1 );
	int len2 = wcslen( str2 );
	wchar_t *p = new wchar_t[ l + len1 + len2 + 1 ];
	if ( !p )
		return;
	if ( l > 0 ){
		memcpy( p, wbuf, l<<1 );
	}
	if ( wbuf ){
		delete[] wbuf;
	}
	wbuf = p;
	memcpy( wbuf+l, str1, len1<<1 );
	memcpy( wbuf+l+len1, str2, len2<<1 );
	wbuf[l+len1+len2] = '\0';
	if ( cbuf ){
		delete[] cbuf;
		cbuf = NULL;
	}
}
int TNMixChar::jcut( int len )
{
	if ( ch ){
		ReleaseCh();
	}
	int l;
	if ( wbuf ){
		l = wcslen(wbuf);
		wbuf[len] = '\0';
		return l>len;
	}
	if ( cbuf ){
		l = strlen( cbuf );
		_mstrcut( cbuf, len );
		return ( l > len )?1:0;
	}
	return 0;
}
wchar_t &TNMixChar::operator []( int i )
{
	if ( ch ){
		ReleaseCh();
	}
	if ( wbuf )
		return wbuf[i];
	if ( !cbuf )
		return ("")[0];
	ChangeToW();
	return wbuf[i];
}
char *TNMixChar::set( const char *str )
{
	return set( str, strlen( str ) );
}
char *TNMixChar::set( const char *str, int len )
{
	if ( ch ){
		ReleaseCh();
	}
	char *p = new char[ len + 1 ];
	if ( !p )
		return NULL;
	if ( cbuf ){
		delete[] cbuf;
	}
	if ( wbuf ){
		delete[] wbuf;
		wbuf = NULL;
	}
	cbuf = p;
	memcpy( cbuf, str, len );
	cbuf[len] = '\0';
	return (char*)str + len;
}
wchar_t *TNMixChar::set( const wchar_t *str )
{
	return set( str, wcslen( str ) );
}
wchar_t *TNMixChar::set( const wchar_t *str, int len )
{
	if ( ch ){
		ReleaseCh();
	}
	wchar_t *p = new wchar_t[ len + 1 ];
	if ( !p )
		return NULL;
	if ( wbuf ){
		delete[] wbuf;
	}
	if ( cbuf ){
		delete[] cbuf;
		cbuf = NULL;
	}
	wbuf = p;
	memcpy( wbuf, str, (len+1)<<1 );
	wbuf[len] = '\0';
	return (wchar_t*)str + len;
}
void TNMixChar::set( TNMixChar &o )
{
	if ( ch ){
		ReleaseCh();
	}
	if ( cbuf ){
		delete[] cbuf;
		cbuf = NULL;
	}
	if ( wbuf ){
		delete[] wbuf;
		wbuf = NULL;
	}
	int len;
	if ( o.cbuf ){
		len = strlen(o.cbuf);
		cbuf = new char[ len + 1 ];
		memcpy( cbuf, o.cbuf, len+1 );
	}
	if ( o.wbuf ){
		len = wcslen(o.wbuf);
		wbuf = new wchar_t[ len + 1 ];
		memcpy( wbuf, o.wbuf, LENTOBYTE(len+1) );
	}
#ifdef NEED_SINGLEBYTE
	singlebyte = o.singlebyte;
#endif
}
void TNMixChar::clear()
{
	if ( cbuf ){
		cbuf[0] = '\0';
	}
	if ( wbuf ){
		wbuf[0] = '\0';
	}
	if ( ch ){
		delete ch;
		ch = NULL;
	}
}
TNMixChar::operator char *()
{
	if ( ch ){
		ReleaseCh();
	}
	if ( cbuf )
		return cbuf;
	if ( !wbuf )
		return "";
	ChangeToC();
	return cbuf;
}
TNMixChar::operator wchar_t *()
{
	if ( ch ){
		ReleaseCh();
	}
	if ( wbuf )
		return wbuf;
	if ( !cbuf )
		return TEXT("");
	ChangeToW();
	return wbuf;
}
void TNMixChar::ChangeToC()
{
	int len = wcslen(wbuf);
#ifdef NEED_SINGLEBYTE
	if (singlebyte){
		cbuf = new char[ len + 1 ];
		uchar *dp = (uchar*)cbuf;
		wchar_t *sp = wbuf;
		for(;*sp;){
#if 0	// for debug
			if (*sp >= 0x100){
				// illegal charactor
			}
#endif
			*dp++ = (uchar)*sp++;
		}
		*dp = '\0';
	} else
#endif	// NEED_SINGLEBYTE
	{
#ifdef DIC_UTF8
		cbuf = new char[ len<<2 + 1 ];
		int size = UTF16toUTF8( wbuf, len, cbuf );
#else
		cbuf = new char[ len*2 + 1 ];
		int size = _WideCharToMultiByte( CP_ACP, 0, wbuf, len, cbuf, len*2, NULL, NULL );
#endif
		cbuf[size] = '\0';
	}
}
void TNMixChar::ChangeToW()
{
	int len = strlen(cbuf);
#ifdef NEED_SINGLEBYTE
	if (singlebyte){
		wbuf = new wchar_t[ len*2 + 1 ];
		uchar *sp = (uchar*)cbuf;
		wchar_t *dp = wbuf;
		for(;*sp;){
			*dp++ = *sp++;
		}
		*dp = '\0';
	} else
#endif	// NEED_SINGLEBYTE
	{
		wbuf = new wchar_t[ len*2 + 1 ];
#ifdef DIC_UTF8
		UTF8toUTF16( (char*)cbuf, len, (wchar_t*)wbuf );
#else
		int size = _MultiByteToWideChar( CP_ACP, 0, (char*)cbuf, len, (wchar_t*)wbuf, len*2 );
		((wchar_t*)wbuf)[size] = '\0';
#endif
	}
}
void TNMixChar::SetBuf( wchar_t *newbuf )
{
	if ( ch ){
		ReleaseCh();
	}
	if ( wbuf ){
		delete[] wbuf;
	}
	wbuf = newbuf;
	if ( cbuf ){
		delete[] cbuf;
		cbuf = NULL;
	}
}
int TNMixChar::compare( TNMixChar &o )
{
	if ( ch ){
		ReleaseCh();
	}
	if ( o.ch ){
		o.ReleaseCh();
	}
	if ( cbuf ){
		return strcmp( cbuf, o ) == 0;
	}
	if ( wbuf ){
		return wcscmp( wbuf, o ) == 0;
	}
	if ( o.cbuf ){
		return strcmp( "", o.cbuf ) == 0;
	}
	if ( o.wbuf ){
		return wcscmp( L"", o.wbuf ) == 0;
	}
	return 1;
}
#if 0
void TNMixChar::ReleaseCh()
{
	tchar *b = ch->discard();
#ifdef _UNICODE
	wbuf = b;
#else
	cbuf = b;
#endif
	delete ch;
	ch = NULL;
}
TNChar &TNMixChar::GetChar()
{
	if ( ch )
		return *ch;
	ch = new TNChar;
#ifdef _UNICODE
	if ( cbuf ){
		ChangeToW();
		delete[] cbuf;
		cbuf = NULL;
		goto j1;
	}
	if ( wbuf ){
j1:;
		ch->SetBuf( wbuf );
		wbuf = NULL;
	}
#else
	if ( wbuf ){
		ChangeToC();
		delete[] wbuf;
		wbuf = NULL;
		goto j1;
	}
	if ( cbuf ){
j1:;
		ch->SetBuf( cbuf );
		cbuf = NULL;
	}
#endif
	return *ch;
}
#endif

#endif	// USE_TNMIXCHAR

