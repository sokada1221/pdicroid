#ifndef	__VBUFFER_H
#define	__VBUFFER_H

//	自動可変長タイプバッファ
//	文字列として扱わないので、最後にNULLはつかない－注意！！

class VarBuffer {
	protected:
		tchar *buff;
		int slotsize;
		int slotnum;
		int len;
	protected:
		tchar *increase( );
	public:
		VarBuffer( int slotsize=256 );
		~VarBuffer( );
		int add( const tchar *str );			// NULL-terminated string
		int add( const tchar *dat, int len );	// バイナリ対応
		int add( tchar c );
		tchar operator []( int i )
			{ return buff[i]; }
		operator const tchar *()
			{ return buff; }
		tchar *c_str()	{ return buff; }
};


#endif	// __VBUFFER_H
