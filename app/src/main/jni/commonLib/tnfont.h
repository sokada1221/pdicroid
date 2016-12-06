#ifndef __tnfont_h
#define	__tnfont_h

#include "pdconfig.h"

#ifdef __ANDROID__
#include "tnfont_android.h"
#endif

#define	USE_DT4		1
#define	USE_HREF	1

#define	ILLCHAR_AS_PERIOD	1	// 不正コードは . で代用
#define	ILLCHAR				'.'	// 代用文字

#define	TFA_BOLD		0x01
#define	TFA_ITALIC		0x02
#define	TFA_UNDERLINE	0x04
#define	TFA_STRIKEOUT	0x08

class TFontAttr {
public:
	union {
		long AValue;
		struct {
			char Attr;
			char Size;
			char AddSize;
			char CharSet;
		};
	};
	COLORREF Color;
	COLORREF BgColor;	// Draw4 internal use.
	tnstr Name;
public:
	TFontAttr()
	{
		AValue = 0;
		Color = 0;
		BgColor = 0;
	}
	TFontAttr(const TFontAttr &fa);
	~TFontAttr(){}
	void SetAttr(char attr)
		{ Attr |= attr; }
	void SetAttr(char attr, bool f)
		{ if (f) Attr |= attr; else Attr &= ~attr; }
	void ResetAttr(char attr)
		{ Attr &= ~attr; }
	void SetBold(bool f)
		{ SetAttr(TFA_BOLD, f); }
	void SetItalic(bool f)
		{ SetAttr(TFA_ITALIC, f); }
	void SetUnderline(bool f)
		{ SetAttr(TFA_UNDERLINE, f); }
	void SetStrikeOut(bool f)
		{ SetAttr(TFA_STRIKEOUT, f); }

	bool IsBold() const
		{ return Attr & TFA_BOLD ? true : false; }
	bool IsItalic() const
		{ return Attr & TFA_ITALIC ? true : false; }
	bool IsUnderline() const
		{ return Attr & TFA_UNDERLINE ? true : false; }
	bool IsStrikeOut() const
		{ return Attr & TFA_STRIKEOUT ? true : false; }

	void SetSize(char size)
		{ Size = size; }
	static int GetHtmlFontSizee(int sizeIndex);
	int GetHtmlFontSize(int sizeIndex);
	void SetHtmlFontSize(int size);
	void SetAddSize(int add);
	void SetColor(COLORREF color)
		{ Color = color; }

	HFONT CreateFont(bool normalsize=false) const;
	HFONT CreateFontNS() const
		{ return CreateFont(true); }
	void Get(LOGFONT &lf) const;
	void Set(const LOGFONT &lf);

	void AssignFontTag(const TFontAttr &fa);
	
	// operators //
	TFontAttr &operator = (const TFontAttr &fa);
	bool operator == (const TFontAttr &fa) const;
	bool operator != (const TFontAttr &fa) const;
	bool operator < (const TFontAttr &fa) const;
};

#if USE_DT2 && !USE_DT4
#define	ONLY_DT2	1
#else
#define	ONLY_DT2	0
#endif

class TNFONT {
public:
	TFontAttr FontAttr;
	HFONT hFont;
#ifndef _UNICODE
	HFONT hAscFont;
	bool fSingleByte;
#endif
	HDC hdc;
	int cy;
	int cxNum;		// the width of the number.
	int tmHeight;
#if ONLY_DT2
	int width[ 256 ];
#else
	int *width;	// dummy
	int SpcWidth;	// space width
	int IllWidth;	// illegal char width
#endif
#ifndef _UNICODE
	int *widthAsc;
	int cyAsc;
	int tmHeightAsc;
#endif
	COLORREF LinkColor;

public:
//	TNFONT( HDC hdc, HFONT _hFont, HFONT _hAscFont=NULL, COLORREF _LinkColor);
	TNFONT( HDC hdc, const TFontAttr &fa, COLORREF _LinkColor
#ifndef _UNICODE
		, const TFontAttr *faAsc=NULL
#endif
		);
	~TNFONT();
#if USE_DT2
	HFONT Select( )
		{ return (HFONT)::SelectObject( hdc, hFont ); }
	HFONT SelectEx(bool reverse=false);
#ifndef _UNICODE
	HFONT SelectAsc( )
		{ return (HFONT)::SelectObject( hdc, hAscFont ); }
	HFONT SelectAscEx(bool reverse=false);
#endif
	void SelectColor(bool reverse=false);
	operator HFONT ()
		{ return hFont; }
#else
	void Select( ){}
	void SelectEx(bool reverse=false){}
	inline HFONT SelectAscEx(bool reverse=false){ return NULL; }
	void SelectAsc( ){}
#endif
	operator HDC ()
		{ return hdc; }
	COLORREF SelectLinkColor( )
		{ return ::SetTextColor( hdc, LinkColor ); }

	int GetSpcWidth() const
		{ return SpcWidth; }
	int GetIllWidth() const
		{ return IllWidth; }
protected:
	void Get( HDC hdc, HFONT hFont, int &cy, int &tmHeight, int *width );
};

#endif	/* __tnfont_h */

