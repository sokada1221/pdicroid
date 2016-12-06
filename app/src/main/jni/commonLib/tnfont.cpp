#include "tnlib.h"
#pragma hdrstop
#include "tnfont.h"

TFontAttr::TFontAttr(const TFontAttr &fa)
	:Name(fa.Name)
{
	AValue = fa.AValue;
	Color = fa.Color;
	BgColor = fa.BgColor;
}

#if 0
static int HtmlSizeTable[] = { -9, -12, -14, -16, -22, -29, -45 };
int TFontAttr::GetHtmlFontSize(int sizeIndex)
{
	if (sizeIndex<1)
		sizeIndex = 1;
	else
	if (sizeIndex>7)
		sizeIndex = 7;
	return HtmlSizeTable[sizeIndex-1];
}
#endif

void TFontAttr::SetHtmlFontSize(int sizeIndex)
{
	// PDICでは本当の絶対値ではなく、相対値に置き換えられる。
#if 0
	Size = GetHtmlFontSize(sizeIndex);
	AddSize = 0;
#else
	if (sizeIndex<1)
		sizeIndex = 1;
	else
	if (sizeIndex>7)
		sizeIndex = 7;
	AddSize = sizeIndex-3;
#endif
}
void TFontAttr::SetAddSize(int addIndex)
{
	if (addIndex<-2)
		addIndex = -2;
	else
	if (addIndex>4)
		addIndex = 4;
	AddSize = addIndex;
}

HFONT TFontAttr::CreateFont(bool normalsize) const
{
	LOGFONT lf;
	Get(lf);
	if (AddSize){
		int h = lf.lfHeight>0?lf.lfHeight:-lf.lfHeight;
		for (int i=0;i<(AddSize>0?AddSize:-AddSize);i++){
			if (AddSize>0)
				h = h*12/10;
			else
				h = h*10/12;
		}
		if (h==0){
			h = 1;
		}
		lf.lfHeight = lf.lfHeight>0?h:-h;
	}
	if (normalsize)
		lf.lfHeight = 0;
	return CreateFontIndirect(&lf);
}

void TFontAttr::Get(LOGFONT &lf) const
{
	memset(&lf, 0, sizeof(lf));
	lf.lfHeight = Size;
	if (Attr & TFA_BOLD)
		lf.lfWeight = FW_BOLD;
	if (Attr & TFA_ITALIC)
		lf.lfItalic = TRUE;
	if (Attr & TFA_UNDERLINE)
		lf.lfUnderline = TRUE;
	if (Attr & TFA_STRIKEOUT)
		lf.lfStrikeOut = TRUE;
	lf.lfCharSet = CharSet;
	_tcsncpy(lf.lfFaceName, Name, tsizeof(lf.lfFaceName));
}

void TFontAttr::Set(const LOGFONT &lf)
{
	Size = lf.lfHeight;
	Attr = (lf.lfWeight>=FW_BOLD?TFA_BOLD:0)
		 | (lf.lfItalic?TFA_ITALIC:0)
		 | (lf.lfUnderline?TFA_UNDERLINE:0)
		 | (lf.lfStrikeOut?TFA_STRIKEOUT:0);
	CharSet = lf.lfCharSet;
	Name = lf.lfFaceName;
}

// HTML tag用assign
void TFontAttr::AssignFontTag(const TFontAttr &fa)
{
	Color = fa.Color;
	BgColor = fa.BgColor;
	Size = fa.Size;
	AddSize = fa.AddSize;
	CharSet = fa.CharSet;
	Name = fa.Name;
}

TFontAttr &TFontAttr::operator = (const TFontAttr &fa)
{
	AValue = fa.AValue;
	Color = fa.Color;
	BgColor = fa.BgColor;
	Name = fa.Name;
	return *this;
}

bool TFontAttr::operator == (const TFontAttr &fa) const
{
	return AValue==fa.AValue && Color==fa.Color && BgColor==fa.BgColor && Name==fa.Name;
}

bool TFontAttr::operator != (const TFontAttr &fa) const
{
	return AValue!=fa.AValue || Color!=fa.Color || BgColor!=fa.BgColor || Name!=fa.Name;
}

bool TFontAttr::operator < (const TFontAttr &fa) const
{
	if (AValue!=fa.AValue)
		return AValue<fa.AValue;
	if (Color!=fa.Color)
		return Color<fa.Color;
	if (BgColor!=fa.BgColor)
		return BgColor<fa.BgColor;
	return _tcscmp(Name, fa.Name)<0;
}

//
// TNFONT
//
TNFONT::TNFONT( HDC _hdc, const TFontAttr &fa, COLORREF _LinkColor
#ifndef _UNICODE
, const TFontAttr *faAsc
#endif
 )
	:FontAttr(fa)
{
	hdc = _hdc;
	hFont = fa.CreateFont();
	Get(hdc, hFont, cy, tmHeight, width);
#ifndef _UNICODE
	if (faAsc){
		hAscFont = faAsc->CreateFont();
#if ONLY_DT2
		widthAsc = new int[ 256 ];
#endif
		Get( hdc, hAscFont, cyAsc, tmHeightAsc, widthAsc );
	} else {
		hAscFont = NULL;
		widthAsc = NULL;
		cyAsc = 0;
	}
#endif
	LinkColor = _LinkColor;
}
TNFONT::~TNFONT()
{
	{
		HFONT hs = (HFONT)GetStockObject( SYSTEM_FONT );
		if ( hFont ){
			SelectObject( hdc, hs );
			DeleteObject( hFont );
		}
#ifndef _UNICODE
		if ( hAscFont ){
			SelectObject( hdc, hs );
			DeleteObject( hAscFont );
		}
#endif
	}
#if ONLY_DT2
	if ( widthAsc )
		delete widthAsc;
#endif
}

#ifdef _UNICODE
HFONT TNFONT::SelectEx(bool reverse)
{
	SelectColor(reverse);
	return Select();
}
#endif

#if USE_DT2 && !defined(_UNICODE)
HFONT TNFONT::SelectAscEx(bool reverse)
{
	SelectColor(reverse);
	return SelectAsc();
}
#endif

#if USE_DT2
void TNFONT::SelectColor(bool reverse)
{
	if (reverse){
		SetTextColor(hdc, FontAttr.BgColor);
		SetBkColor(hdc, FontAttr.Color);
	} else {
		SetTextColor(hdc, FontAttr.Color);
		SetBkColor(hdc, FontAttr.BgColor);
	}
}
#endif

void TNFONT::Get( HDC hdc, HFONT hFont, int &cy, int &tmHeight, int *width )
{
	HFONT hOldFont = (HFONT)SelectObject( hdc, hFont );

	TEXTMETRIC tm;
	GetTextMetrics( hdc, &tm );
	cy = tm.tmHeight + tm.tmExternalLeading;
	tmHeight = tm.tmHeight;

	// old codes //
//	if ( fSingleByte )
//		*fSingleByte = ( tm.tmCharSet & DEFAULT_CHARSET ? false : true );
#if ONLY_DT2
	memset( width, 0, 0x20*sizeof(int) );
#ifdef WINCE
	SIZE sz;
	tchar c = ILLCHAR;
	GetTextExtentPoint32( hdc, &c, 1, &sz );
	width[ILLCHAR] = sz.cx;
	for ( c = ' ';c<=0x00FF;c++ ){
		if ( GetTextExtentPoint32( hdc, &c, 1, &sz ) ){
			width[c] = sz.cx;
		} else {
			width[c] = width[ILLCHAR];
		}
	}
#else
	if ( !GetCharWidth32( hdc, ' ', 255, &width[' '] ) )
		GetCharWidth( hdc, ' ', 255, &width[' '] );
#endif
#else
#ifdef WINCE
	SIZE sz;
	tchar c = ' ';
	GetTextExtentPoint32( hdc, &c, 1, &sz );
	SpcWidth = sz.cx;
	c = ILLCHAR;
	GetTextExtentPoint32( hdc, &c, 1, &sz );
	IllWidth = sz.cx;
#else
	if ( !GetCharWidth32( hdc, ' ', ' ', &SpcWidth ) )
		GetCharWidth( hdc, ' ', ' ', &SpcWidth );
	if ( !GetCharWidth32( hdc, ILLCHAR, ILLCHAR, &IllWidth ) )
		GetCharWidth( hdc, ILLCHAR, ILLCHAR, &IllWidth );
	if ( !GetCharWidth32( hdc, '0', '0', &cxNum) )
		GetCharWidth( hdc, '0', '0', &cxNum);
#endif
#endif
	// end of old codes //

	SelectObject( hdc, hOldFont );
}

// Obsolete Codes //
#if !USE_DT2
#define	EXCLUDE_DF	(DF_DISP|DF_OPAQUE|DF_ICON|DF_SELECTED|DF_REVERSE|DF_PAINT|DF_UNREDRAW|DF_DRAWICON|DF_DRAWNAME|DF_UNSELECTED|DF_MOUSEMOVE|DF_MOUSERELEASE|DF_WHOLECHAR|DF_NOTITLE|DF_SINGLEBYTE)
int DrawText2s( HDC hdc, const tchar *str, RECT &rc, int dispf )
{
	int cy
		= DrawText( hdc, str, -1, &rc, (dispf & ~EXCLUDE_DF)
		| DT_LEFT | DT_EXPANDTABS | /* DT_NOCLIP | */ DT_NOPREFIX | DT_WORDBREAK | (dispf & DF_DISP ? 0 : DT_CALCRECT) );
	SIZE sz;
	GetTextExtentPoint32( hdc, str, _tcslen(str), &sz );
	if ( rc.right - rc.left > sz.cx )
		rc.right = rc.left + sz.cx;
	return cy;
}
int Squre::_AdjDisp( RECT &rc, const tchar *str, int dispf, int eright )
{
	COLORREF oldtext;
	COLORREF oldback;
	if ( dispf & DF_REVERSE ){
		oldtext = SetTextColor( hdc, oldback = GetBkColor( hdc ) );
		SetBkColor( hdc, oldtext );
	}
	int cy = DrawText2s( hdc, str, rc, dispf );
	if ( dispf & DF_REVERSE ){
		SetTextColor( hdc, oldtext );
		SetBkColor( hdc, oldback );
	}
	return cy;
}
static void Get( HDC hdc, HFONT hFont, int &cy, int &tmHeight, bool *fSingleByte  )
{
#if SQUFONT
	HFONT hOldFont = (HFONT)SelectObject( hdc, hFont );
#endif

	TEXTMETRIC tm;
	GetTextMetrics( hdc, &tm );
	cy = tm.tmHeight + tm.tmExternalLeading;
	tmHeight = tm.tmHeight;
#if SQUFONT
	if ( fSingleByte )
		*fSingleByte = ( tm.tmCharSet & DEFAULT_CHARSET ? false : true );
	SelectObject( hdc, hOldFont );
#endif
}
TNFONT::TNFONT( HDC _hdc, HFONT _hFont, HFONT _hAscFont, /*BOOL _fNonDestruct, */ COLORREF _LinkColor )
{
/*	fNonDestruct = _fNonDestruct;*/
	hdc = _hdc;
#if DRAW2FONT
	hFont = _hFont;
	hAscFont = _hAscFont;
#endif
	cyAsc = 0;
	LinkColor = _LinkColor;

#if DRAW2FONT
	Get( hdc, hFont, cy, tmHeight, &fSingleByte );
	if ( hAscFont )
		Get( hdc, hAscFont, cyAsc, tmHeightAsc, NULL );
#else
	Get( hdc, NULL, cy, tmHeight, &fSingleByte );
#endif

}

TNFONT::~TNFONT()
{
#if DRAW2FONT
/*	if ( !fNonDestruct )*/
	{
		if ( hFont )
			DeleteObject( hFont );
		if ( hAscFont )
			DeleteObject( hAscFont );
	}
#endif
}
CharHT::CharHT( int _item, POINT *_pt )
{
	item = _item;
	if ( _pt )
		pt = *_pt;
	pos = -1;
	pos1 = pos2 = item1 = item2 = -1;
	fOver = CHT_NONE;
}
#endif	// !USE_DT2

