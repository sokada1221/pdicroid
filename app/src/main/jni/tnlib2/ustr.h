//---------------------------------------------------------------------------

#ifndef ustrH
#define ustrH
//---------------------------------------------------------------------------

class __ustr {
protected:
	void *buffer;
	char *newbuffer;
	int length;
	int type;
public:
	__ustr( const char *str );
	__ustr( const char *str, int len );
	__ustr( const wchar_t *str );
	~__ustr();
	operator char *();
	operator unsigned char *()
		{ return (unsigned char*)(char*)(*this); }
	operator wchar_t *();
};

#define	_u(x)	__ustr(x)

#endif

