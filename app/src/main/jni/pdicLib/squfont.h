#ifndef	__SQUFONT_H
#define	__SQUFONT_H

#include "wsqudef.h"
#include "pdconfig.h"
#include "tnfont.h"

// NOSQUCOLOR
// 色分けを使用しない - 色を全く使用しないわけではない(charcolorとbackcolorは使用する)
// NOCOLOR
// 色はまったく使用しない

#define	MAX_CONTROL_HEIGHT	32

#ifdef NOCOLOR
#undef NOSQUCOLOR
#define	NOSQUCOLOR
#endif

class TRegKey;

extern TFontAttr nfont;	// 共通のフォント

#if SQUFONT
extern TFontAttr wfont;	// 英単語フォント
#if USE_DT2
extern TFontAttr wfontASC;	// 英単語英字フォント
extern TFontAttr nfontASC;	// 共通英字フォント
#endif
#ifdef NEWDIC2
extern TFontAttr pfont;	// 発音記号フォント
#endif

extern TFontAttr jfont;	// 日本語訳フォント
extern TFontAttr efont;	// 用例フォント

#ifdef EPWING
extern TFontAttr gfont;	// EPWINGフォント
#endif

#else
#define	wfont	nfont
#define	pfont	nfont
#define	jfont	nfont
#define	efont	nfont
#define	gfont	nfont
#endif

extern COLORREF LinkColors[ N_SECTION+1+1 ];

//extern TFontAttr *fonts[ N_SECTION+1 ];
//extern TFontAttr *fontsASC[ N_SECTION+1 ];

extern int useuniqfont;

struct FontHandles {
#if SQUFONT
	HFONT hnFont;
//	HFONT hnFontASC;
	HFONT hwFont;
//	HFONT hwFontASC;
	HFONT hpFont;
	HFONT hjFont;
    HFONT heFont;
    HFONT hgFont;
#endif
	FontHandles();
	~FontHandles();
	void Create( int flagNormalSize );
	void Delete();

#if SQUFONT
    #define	__SETFONT( hwnd, hfont ) SendMessage( hwnd, WM_SETFONT, (WPARAM)hfont, TRUE )
#else
	#define	__SETFONT( hwnd, hfont )
#endif

	void SetWord( HWND h )
		{ __SETFONT( h, hwFont ); }
	void SetNormal( HWND h )
		{ __SETFONT( h, hnFont ); }
	void SetPron( HWND h )
		{ __SETFONT( h, hpFont ); }
	void SetJapa( HWND h )
		{ __SETFONT( h, hjFont ); }
	void SetExp( HWND h )
		{ __SETFONT( h, heFont ); }
	void SetEPWing( HWND h )
		{ __SETFONT( h, hgFont ); }
#if SQUFONT
	#define	__return_font(name)		return name
#else
	#define	__return_font(name)		return NULL
#endif
	HFONT GetNormal() const
		{ __return_font(hnFont); }
	HFONT GetWord() const
		{ __return_font(hwFont); }
	HFONT GetPron() const
		{ __return_font(hpFont); }
	HFONT GetJapa() const
		{ __return_font(hjFont); }
	HFONT GetExp() const
		{ __return_font(heFont); }
};


// フォントハンドル
extern TNFONT *hnFont;
#if SQUFONT
extern TNFONT *hnFont, *hwFont, *hpFont, *hjFont, *heFont, *hgFont;
extern TNFONT *hnFontASC, *hwFontASC, *hpFontASC;
#else
#define	hwFont	hnFont
#define	hpFont	hnFont
#define	hjFont	hnFont
#define	heFont	hnFont
#define	hgFont	hnFont
#endif

extern int nFontsOpened;		// hFontsが有効であるかどうかの参照回数(OpenTextFonts()の回数）

extern int fNormalSize;

#if USE_DEFFONTSIZE
inline bool IsNormalSize( int flag )
	{ return (fNormalSize & flag) ? true : false; }
#else
inline bool IsNormalSize( int flag )
	{ return false; }
#endif

// for draw the squere
TFontAttr *GetFontAttrFromSN(int sn);
void _CreateTextFonts( HDC hdc );
void _DeleteTextFonts( );
HDC _GetCurrentHDC( );
void _SetCurrentHDC( HDC hdc );
HFONT CreatePFont( );
HFONT CreateOneFont( int type, bool fNormalSize );

#ifndef NOSQUCOLOR
inline COLORREF GET_BACKCOLOR( COLORREF color )
	{ return color == (COLORREF)-1 ? GetSysColor(COLOR_WINDOW) : color; }
inline COLORREF GET_FORECOLOR( COLORREF color )
	{ return color == (COLORREF)-1 ? GetSysColor(COLOR_WINDOWTEXT) : color; }
#else
inline COLORREF GET_BACKCOLOR( COLORREF /*color*/ )
	{ return GetSysColor(COLOR_WINDOW); }
inline COLORREF GET_FORECOLOR( COLORREF /*color*/ )
	{ return GetSysColor(COLOR_WINDOWTEXT); }
#endif

void SaveProfileFont( TRegKey *key, int type, TFontAttr &lf, COLORREF color, COLORREF linkcolor, TFontAttr *lfASC );
void DeleteProfileFont( TRegKey *key, int type );
void LoadProfileFont( TRegKey *key, int type, TFontAttr &lf, COLORREF &color, COLORREF &linkcolor, TFontAttr *lfASC );

#if defined(__VCL0_H__) || defined(INC_VCL)
void LoadProfileFont( int type, TFont *f, COLORREF &linkcolor );
void SaveProfileFont( int type, TFont *f, COLORREF linkcolor );
void AssignFont(int sqm_flag, TFont *font, TWinControl *control, int maxheight=-1);
void AssignFont(TFontAttr &fa, TFont *font, TWinControl *control, bool useNormalSize, int maxheight=-1);
#endif

int GetCY( int no );

#if USE_SPECIALCHAR
// 特殊文字入力関連 //
extern tnstr splangname[ 4 ];	// 特殊文字入力言語名
#endif

void MakeEnph( class EnphTextVec &et, class THyperLinks &hls, int item );

// for MIXDIC
// 最後に使用したfontのsinglebyte flag
#if MIXDIC && SQUFONT
extern bool wSingleByte;
extern bool jSingleByte;
extern bool eSingleByte;
extern bool pSingleByte;
#endif

namespace squfont {
extern int HtmlEnabled;
extern bool CanDispDicName;
};

#endif	// __SQUFONT_H

