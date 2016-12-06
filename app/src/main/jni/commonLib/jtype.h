#ifndef	__JTYPE_H
#define	__JTYPE_H

// tuschar	: unicodeではwchar_t、sjisではushort
#ifdef UNICODE
typedef wchar_t tuschar;
#else
typedef unsigned short tuschar;
#endif

// コード対応表は http://www.microsoft.com/globaldev/reference/dbcs/932.htm を参照

#define	ucCODE_NAMI1  	0xFF5E	// ～
#define	ucCODE_NAMI2	0x2026	// …
#define	ucCODE_NAMI3	0x2025	// ‥

#define	ucCODE_SPACE	0x3000	//　
#define	ucCODE_0		0xFF10	// ０
#define	ucCODE_1		0xFF11	// １
#define	ucCODE_9		0xFF19	// ９

#define	mbCODE_NAMI1  	0x8160	// ～
#define	mbCODE_NAMI2	0x8163	// …
#define	mbCODE_NAMI3	0x8164	// ‥

#define	mbCODE_SPACE	0x8140	//　
#define	mbCODE_0		0x824F	// ０
#define	mbCODE_1		0x8250	// １
#define	mbCODE_9		0x8258	// ９

#ifdef _UNICODE

#define	CODE_NAMI1  	ucCODE_NAMI1
#define	CODE_NAMI2		ucCODE_NAMI2
#define	CODE_NAMI3		ucCODE_NAMI3

#define	CODE_GA			0x304C	// が
#define	CODE_NO			0x306E	// の
#define	CODE_NI			0x306B	// に
#define	CODE_WO			0x3092	// を

#define	CODE_COMMA1		0x3001	// 、
#define	CODE_COMMA2		0x30FB	// ・
#define	CODE_COMMA3		0x3002	// 。
#define	CODE_BRACKET1	0x300C	// 「
#define	CODE_BRACKET2	0x300D	// 」
#define	CODE_JPROLONG	0x30FC	// ー

#define	CODE_SEMICOLON	0xFF1B	// ；
#define	CODE_COMMA		0xFF0C	// ，
#define	CODE_APOSTROPHE	0x2019	// ’
#define	CODE_QUOTATION	0x201D	// ”

#define	CODE_SPACE		ucCODE_SPACE
#define	CODE_PERIOD		0xFF0E	// ．

#define	CODE_LBR1		0xFF08	//（
#define	CODE_LBR2		0x3014	// 〔
#define	CODE_LBR3		0xFF3B	// ［
#define	CODE_LBR4		0xFF5B	// ｛
#define	CODE_LBR5		0x3008	// 〈
#define	CODE_LBR6		0x300A	// 《
#define	CODE_LBR7		0x300C	// 「
#define	CODE_LBR8		0x300E	// 『
#define	CODE_LBR9		0x3010	// 【
#define	CODE_LBR10		0xFF1C	// ＜

#define	CODE_RBR1		0xFF09	// ）
#define	CODE_RBR2		0x3015	// 〕
#define	CODE_RBR3		0xFF3D	// ］
#define	CODE_RBR4		0xFF5D	// ｝
#define	CODE_RBR5		0x3009	// 〉
#define	CODE_RBR6		0x300B	// 》
#define	CODE_RBR7		0x300D	// 」
#define	CODE_RBR8		0x300F	// 』
#define	CODE_RBR9		0x3011	// 】
#define	CODE_RBR10		0xFF1E	// ＞

#define	CODE_L_A		0xFF21	// Ａ
#define	CODE_L_Z		0xFF3A	// Ｚ

#define	CODE_S_A		0xFF41	// ａ
#define	CODE_S_Z		0xFF5A	// ｚ

#define	CODE_0			ucCODE_0
#define	CODE_1			ucCODE_1
#define	CODE_9			ucCODE_9

#define	CODE_BLACK_SQUARE	0x25A0	// ■
#define	CODE_BLACK_DIAMOND	0x25C6	// ◆
#define	CODE_BLACK_CIRCLE	0x25CF	// ●

#else	// !_UNICODE

#define	CODE_NAMI1  	mbCODE_NAMI1
#define	CODE_NAMI2		mbCODE_NAMI2
#define	CODE_NAMI3		mbCODE_NAMI3

#define	CODE_GA			0x82aa	// が
#define	CODE_NO			0x82cc	// の
#define	CODE_NI			0x82c9	// に
#define	CODE_WO			0x82f0	// を

#define	CODE_COMMA1		0x8141	// 、
#define	CODE_COMMA2		0x8145	// ・
#define	CODE_COMMA3		0x8142	// 。
#define	CODE_BRACKET1	0x8175	// 「
#define	CODE_BRACKET2	0x8176	// 」

#define	CODE_SEMICOLON	0x8147	// ；
#define	CODE_COMMA		0x8143	// ，
#define	CODE_APOSTROPHE	0x9188	// ’

#define	CODE_SPACE		mbCODE_SPACE
#define	CODE_PERIOD		0x8144	// ．

#define	CODE_LBR1		0x8169	//（
#define	CODE_LBR2		0x816b	// 〔
#define	CODE_LBR3		0x816d	// ［
#define	CODE_LBR4		0x816f	// ｛
#define	CODE_LBR5		0x8171	// 〈
#define	CODE_LBR6		0x8173	// 《
#define	CODE_LBR7		0x8175	// 「
#define	CODE_LBR8		0x8177	// 『
#define	CODE_LBR9		0x8179	// 【
#define	CODE_LBR10		0x8183	// ＜

#define	CODE_RBR1		0x816a	// ）
#define	CODE_RBR2		0x816c	// 〕
#define	CODE_RBR3		0x816e	// ］
#define	CODE_RBR4		0x8170	// ｝
#define	CODE_RBR5		0x8172	// 〉
#define	CODE_RBR6		0x8174	// 》
#define	CODE_RBR7		0x8176	// 」
#define	CODE_RBR8		0x8178	// 』
#define	CODE_RBR9		0x817a	// 】
#define	CODE_RBR10		0x8184	// ＞

#define	CODE_L_A		0x8260	// Ａ
#define	CODE_L_Z		0x8279	// Ｚ

#define	CODE_S_A		0x8281	// ａ
#define	CODE_S_Z		0x829a	// ｚ

#define	CODE_0			mbCODE_0
#define	CODE_1			mbCODE_1
#define	CODE_9			mbCODE_9

#define	CODE_BLACK_DIAMOND	0x819F	// ◆
#define	CODE_BLACK_CIRCLE	0x819C	// ●
#define	CODE_BLACK_SQUARE	0x81A1	// ■

#endif	// !_UNICODE

#define	BOCU_SPACE		0x0020	//  
#define	BOCU_COMMA		0x007C	// ,
#define	BOCU_SHARP		0x0073	// #


#define	DLM_NONE	0
#define	DLM_EX		1
#define	DLM_COMMA	','

#include "tndefs.h"

#define	ucLD_CHAR(c,p)	c = *p++;
#define	ucST_CHAR(c,p)	*p++ = c;
#define	ucMOV_CHAR(p,q)	*p++ = *q++;
#define	ucIS_ENDCHAR(c)	(!(c))

#define	mbTYPE_CHAR		unsigned short
#define	mbLD_CHAR(c, p)	{c = (unsigned char)*(*(unsigned char**)&p)++; if(_ismbblead(c)) c = (unsigned short)(((unsigned short)c << 8) + (unsigned char)*(*(unsigned char**)&p)++);}
#define	mbST_CHAR(c, p)	{if ((unsigned short)c > 0x100) *(*(unsigned char**)&p)++ = (unsigned char)(c >> 8); *(*(unsigned char**)&p)++ = (unsigned char)c;}
#define	mbMOV_CHAR(p, q)	{ unsigned char c = *(*(unsigned char**)&q)++ = *(*(unsigned char**)&p)++; if ( _ismbblead( c ) ) *(*(unsigned char**)&q)++ = *(*(unsigned char**)&p)++; }
#define	mbIS_ENDCHAR(c)	(!((unsigned char)(c)))

#ifdef _UNICODE
#ifdef __UTF8
#define	TYPE_CHAR		unsigned long
#else
#define	TYPE_CHAR		wchar_t
#endif
#define	LD_CHAR(c,p)	ucLD_CHAR(c,p)
#define	ST_CHAR(c,p)	ucST_CHAR(c,p)
#define	MOV_CHAR(p,q)	ucMOV_CHAR(p,q)
#define	IS_ENDCHAR(c)	ucIS_ENDCHAR(c)
#else
#define	TYPE_CHAR		mbTYPE_CHAR
#define	LD_CHAR(c, p)	mbLD_CHAR(c,p)
#define	ST_CHAR(c, p)	mbST_CHAR(c,p)
#define	MOV_CHAR(p, q)	mbMOV_CHAR(p,q)
#define	IS_ENDCHAR(c)	mbIS_ENDCHAR(c)
#endif

bool islbr(unsigned short c);
bool isrbr(unsigned short c);
bool islbr2( unsigned short c );
bool isalphanum( int c );
bool isdelim(unsigned short c);
bool isdelim2(unsigned short c);
int jkstricmp( const tchar *str1, const tchar *str2 );
tchar *issuffix( const tchar *str );
#ifdef _UNICODE
#define	mbIsWordChar	IsWordChar
#else
bool mbIsWordChar( unsigned short c );
#endif

inline bool iskatakana( unsigned short c )
{
#ifdef _UNICODE
	return c >= _T('ァ') && c <= _T('ヶ');
#else
	return c >= 0x8340 && c <= 0x8396;
#endif
}
inline bool ishiragana( unsigned short c )
{
	return
#ifdef _UNICODE
		( c >= 0x3040 && c <= 0x309F )
#else
		( c >= 0x82a0 && c <= 0x82f1 )
#endif
	;
}
inline bool IsWordChar( tuchar c )
{
#ifdef _UNICODE
#ifdef _Windows
	WORD ct;
	GetStringTypeW(CT_CTYPE1,&c,1,&ct);
	return (ct & (C1_ALPHA|C1_DIGIT)) || c=='-' || c==CODE_JPROLONG || c=='\'' || c=='_';
#else
	return isalphanum( c ) || c == '-' || c==CODE_JPROLONG || c == '\'' || c=='_';
#endif
#else	// !_UNICODE
	return isalphanum( c ) || c == '-' || c == '\'' || c >= 0xc0;
#endif
}

#ifdef _UNICODE
#define	LD_MCHAR(c, p, f)	LD_CHAR(c,p)
#define	ST_MCHAR(c, p, f)	ST_CHAR(c,p)
#define	MOV_MCHAR(p, q, f)	MOV_CHAR(p,q)
#define	_IsDBCSLeadByte(x)	(false)
#else
// シングルバイトにも対応
// f がTRUEのときはシングルバイト
#define	LD_MCHAR(c, p, f)	{c = (unsigned char)*(*(unsigned char**)&p)++; if( !f && _ismbblead(c)) c = (unsigned short)(((unsigned short)c << 8) + (unsigned char)*(*(unsigned char**)&p)++);}
#define	ST_MCHAR(c, p, f)	{if ( !f && (unsigned short)c > 0x100) *(*(unsigned char**)&p)++ = (unsigned char)(c >> 8); *(*(unsigned char**)&p)++ = (unsigned char)c;}
#define	MOV_MCHAR(p, q, f)	{ unsigned char c = *(*(unsigned char**)&q)++ = *(*(unsigned char**)&p)++; if ( !f && _ismbblead( c ) ) *(*(unsigned char**)&q)++ = *(*(unsigned char**)&p)++; }
#define	_IsDBCSLeadByte(x)	IsDBCSLeadByte(x)
#endif

bool elimchar( tchar *p );	// 不正コードの除去
bool elimcharspc( tchar *p );	// 不正コードの除去+spcで埋める

void RemoveStr( tchar *str, const tchar *pattern1, const tchar *pattern2, int fKanaEnd );

#ifdef __tnstr_h
int jcut(tnstr &str, int len);
#endif

bool IsRegularChar(tchar c);

const tchar *NextDelim(const tchar *s);

#endif	// __JTYPE_H

