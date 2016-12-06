//---------------------------------------------------------------------------

#ifndef CharTH
#define CharTH
//---------------------------------------------------------------------------

#include <string.h>

inline int _strlen(const char *s)
	{ return strlen(s); }
inline int _strlen(const wchar_t *s)
	{ return wcslen(s); }

// CharT //
template <class T>
class CharT {
protected:
	T *buf;
	static T str_null[1];
public:
	CharT();
	virtual ~CharT();
	operator const T*() const
		{return buf ? buf : str_null;}
	T *c_str()
		{ return buf ? buf : str_null; }
	void SetBuf( T *newbuf )
		{ if (buf) delete[] buf; buf = newbuf; }
	T *set( const T *str )
		{ return set( str, _strlen( str ) ); }
	T *set( const T *str, int len );			//lenまでのコピー
	CharT<T> &operator = ( T *str )
		{set( str ); return *this;}
};

template <class T>
T CharT<T>::str_null[1] = {0};

template <class T>
CharT<T>::CharT()
{
	buf = NULL;
}
template <class T>
CharT<T>::~CharT()
{
	if (buf)
		delete[] buf;
}
template <class T>
T *CharT<T>::set( const T *str, int len )			//lenまでのコピー
{
	T *p = new T[ len + 1 ];
	if ( !p )
		return NULL;
	if ( buf ){
		delete[] buf;
	}
	buf = p;
	memcpy( buf, str, len*sizeof(T) );
	buf[len] = '\0';
	return (T*)str + len;
}
// CChar //
class CChar : public CharT<char> {
typedef CharT<char> super;
public:
	CChar &operator = ( char *str )
		{ super::operator = (str); return *this; }
};

class WChar : public CharT<wchar_t> {
typedef CharT<wchar_t> super;
public:
	WChar &operator = ( wchar_t *str )
		{ super::operator = (str); return *this; }
};

#endif
