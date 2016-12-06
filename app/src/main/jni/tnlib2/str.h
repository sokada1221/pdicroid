#ifndef	__STR_H
#define	__STR_H

#include	"tnstr.h"

char *ubtospc(char *str);

class Str {
private:
	int maxlen;				//現在のところ無意味
protected:
	char *buf;
public:
	Str(int _maxlen);
	~Str();
	void set(const char *str);
	int len( void );
	char &operator [](int c)	{return buf[c];}
	operator const char *()			{return buf;}
};

class Path : public Str {
	char *fullpath;				//フルパス名のバッファ
								//次のmake()まで生きている
public:
	Path();
	~Path();
	const char *make(const char *filename);		//フルパス名を作成
};

//char *はChar型にして、文字列長さに対する処理を強化する必要あり
void addmd(char *filename);
char *strgetfile(char *buf, const char *str);
char *makeuniqname(char *filename);
int checkext(const char *filename);
int getnum(int &num, const char *&str);
char *getstrword( const char *buf, tnstr &word );			//英字単語の取得
char *getstrnumword( const char *buf, tnstr &word );			//英字で始まる英数字文字列の取得

// 新規 1995.12.29
// スペースとは半角スペースとタブ
//tchar *SkipSpc( const tchar *str );	// strにスペースがあればスキップする
//tchar *SkipChar( const tchar *str );
//tchar *SkipNum( const tchar *str );	// 数値のみスキップ(先頭の+/-を認識)
tchar *SkipTo( const tchar *str, int c );	// cの文字までスキップする(ポインターはcへ、cが無い場合は終端'\0'へ)

//#define	LD_CHAR(c, p)	{c = (uchar)*p++; if(iskanji(c)) c = ((ushort)c << 8) + (uchar)*p++;}
//#define	ST_CHAR(c, p)	{if ((ushort)c > 0x100) *(uchar*)p++ = c >> 8; *(uchar*)p++ = c;}

#endif	// __STR_H
