//---------------------------------------------------------------------------

#include "pdclass.h"
#pragma hdrstop

#include "SquView.h"
#include "winsqu.h"
#include "wsview.h"
#include "wpdcom.h"
#include "winmsg.h"
#include "pdprof.h"

#include "SquFrm.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#define	DEF_FONT_POINT	11		// default font size [point]

#if USE_MEMICON
#include "tnimage.h"
#include "id.h"
TNImage Squre::imgMemory(IMG_MEMORY);
TNImage Squre::imgModify(IMG_MODIFY);
#endif

//---------------------------------------------------------------------------
//	Item Bar
//---------------------------------------------------------------------------
int Squre::GetOffs( int type )
{
	return GetX0() + ItemView.GetOffs(type);
}
COLORREF Squre::GetColor( int type )
{
	return ItemView.GetColor(type); 
}
COLORREF Squre::GetLinkColor( int type )
{
	return ItemView.GetLinkColor(type);
}
const tchar *Squre::GetTitle( int type )
{
	return ItemView.GetTitle(type);
}
int Squre::GetWidth( int type )
{
	return ItemView.GetWidth(type);
}
void Squre::SetTitle( int type, const tchar *newtitle )
{
	ItemView.SetTitle(type, newtitle);
}
void Squre::SetColor( int type, COLORREF color )
{
	ItemView.SetColor(type, color);
}
void Squre::SetLinkColor( int type, COLORREF color )
{
	ItemView.SetLinkColor(type, color);
}
bool Squre::IsVisible(int type)
{
	return ItemView.IsVisible(type);
}

//---------------------------------------------------------------------------
//	Common
//---------------------------------------------------------------------------
void Squre::SetHintText(const tchar *msg)
{
	View->SetHintText(msg);
}
void Squre::InitView()
{
	LastIndex = 0;
	MaxLines = MAX_NDISP*8;
	IndexOffset = 0;
	IndexMiOffset = 0;

	charcolor = -1;
	backcolor = -1;

	InitFont();
}
int Squre::GetLY( )		// 項目バーを除いた縦のドット数
{
	return View->GetViewHeight();
}
int Squre::GetLX()
{
	return View->GetViewWidth();
}
//---------------------------------------------------------------------------
//	Fonts and Colors
//---------------------------------------------------------------------------
void Squre::SetupView( HDC hdc )
{
	// システムフォントの大きさを得る
	cxSys = GetSystemFontWidth();
	cySys = GetSystemFontHeight();

	// 各文字のフォントの大きさを得る
	SetupTextFont( hdc );
}
void Squre::SetOrg()
{
	SetMaxLine( );

	InvalidateLines( );

	LastIndex = 0;
}
struct YCC {
	int y;
	int cb;
	int cr;
};
struct tRGB {
	int r;
	int g;
	int b;
};
void GetTextSize( HDC hdc, TFontAttr &fa, int &cx, int &cy, int &charset, int *attr2)
{
#if SQUFONT
	HFONT hFont = fa.CreateFont();
	HFONT hOldFont = (HFONT)SelectObject( hdc, hFont );
#endif

	TEXTMETRIC tm;
	GetTextMetrics( hdc, &tm );
	cx = tm.tmAveCharWidth;
	cy = tm.tmHeight + tm.tmExternalLeading;
	charset = tm.tmCharSet;

#if !USE_MEMICON
	// 暗,修の横幅
	if ( attr2 ){
		SIZE size;
		GetTextExtentPoint32( hdc, StrMemory, lstrlen(StrMemory), &size );
		*attr2 = size.cx;
	}
#endif

#if SQUFONT
	SelectObject( hdc, hOldFont );
	DeleteObject( hFont );
#endif
}
YCC RGBtoYCC(tRGB in)
{
	YCC ycc;
#if 0
	ycc.y = 299 * in.r + 587 * in.g + 114 * in.b;
	ycc.cb = -168 * in.r - 331 * in.g + 500 * in.b;
	ycc.cr = 500 * in.r - 418 * in.g -81 * in.b;
#else
	ycc.y = (299 * in.r + 587 * in.g + 114 * in.b)/255;
	ycc.cb = in.r*1000/255 - ycc.y;
	ycc.cr = in.b*1000/255 - ycc.y;
#endif
	return ycc;
}
tRGB YCCtoRGB(YCC in)
{
	tRGB out;
#if 0
	out.r = ((in.y +1402 * (in.cr))/1000);
	out.g = ((in.y - 344 * (in.cb) - 714 * (in.cr))/1000);
	out.b = ((in.y +1772 * (in.cb))/1000);
#else
	int r = in.y + in.cb;
	int b = in.y + in.cr;
	out.r = r*255/1000;
	out.b = b*255/1000;
	out.g = ((1000*in.y - 299*r - 114*b) / 587)*255/1000;

	// 正規化
#if 0
	int maxv = max(max(out.r,out.g),out.b);
	if (maxv>255){
		out.r = out.r*255/maxv;
		out.g = out.g*255/maxv;
		out.b = out.b*255/maxv;
	}
#endif

	if (out.r>255) out.r = 255;	
	else if (out.r<0) out.r = 0;
	if (out.b>255) out.b = 255;	
	else if (out.b<0) out.b = 0;
	if (out.g>255) out.g = 255;	
	else if (out.g<0) out.g = 0;
#endif
	return out;
}
// aとbの輝度における中間色を求める
// CbCrはaのものを用いる
COLORREF GetGrayReverse(COLORREF a, COLORREF b)
{
	tRGB rgba;
	tRGB rgbb;
	rgba.r = GetRValue(a);
	rgba.g = GetGValue(a);
	rgba.b = GetBValue(a);
	rgbb.r = GetRValue(b);
	rgbb.g = GetGValue(b);
	rgbb.b = GetBValue(b);
	YCC ycca = RGBtoYCC(rgba);
	YCC yccb = RGBtoYCC(rgbb);
	YCC out;
#if 1
	out.y = (ycca.y+yccb.y)/2;
#else
	if (yccb.y<ycca.y){
		// background(yccb.y)のほうが暗い
		out.y = (ycca.y+yccb.y)*2/3;
	} else {
		// background(yccb.y)のほうが明るい
		out.y = (ycca.y+yccb.y)/3;
	}
#endif
	out.cb = ycca.cb;
	out.cr = ycca.cr;
	tRGB rgb = YCCtoRGB(out);
	return RGB(rgb.r, rgb.g, rgb.b);
}
void Squre::_SetupTextFonts( HDC hdc, TFontAttr &fa, int &cxMax, int &cyMax, int type )
{
	int cx, cy;
	int charset;
	GetTextSize( hdc, fa, cx, cy, charset );
	cxMax = min( cx, cxMax );	// 一番小さいサイズを得る
	cyMax = min( cy, cyMax );
#ifdef USE_SINGLEBYTE
	fSingleByte[ type ] = charset & DEFAULT_CHARSET ? FALSE : TRUE;
#endif
}
#ifdef NOSQUCOLOR
	#define	_SETCOLOR( x )
#else
	#define	_SETCOLOR( x )	else { SetColor( x, charcolor ); }
#endif

void Squre::SetupTextFont( HDC hdc )
{
#ifdef USE_SINGLEBYTE
	int s = 0;	// シングルバイトフラグ
#endif

	int charset;
	int cxMax, cyMax;
	GetTextSize( hdc, nfont, cxMax, cyMax, charset, &cxAttr2 );
	int cxText = cxMax;
#if USE_MEMICON
	cxAttr = imgMemory.GetCX();
	cyAttr = imgMemory.GetCY();
	cxAttr2 = cxAttr;
#else
	cxAttr = cxMax;
	cyAttr = cyMax;
#endif
#if SQUFONT
	if ( useuniqfont & SNF_WORD )
		_SetupTextFonts( hdc, wfont, cxMax, cyMax, SN_WORD );
	_SETCOLOR( SN_WORD );
#ifdef NEWDIC2
	if ( useuniqfont & SNF_PRON )
		_SetupTextFonts( hdc, pfont, cxMax, cyMax, SN_PRON );
	_SETCOLOR( SN_PRON );
#endif
	if ( useuniqfont & SNF_JAPA )
		_SetupTextFonts( hdc, jfont, cxMax, cyMax, SN_JAPA );
	_SETCOLOR( SN_JAPA );

	if ( useuniqfont & SNF_EXP1 )
		_SetupTextFonts( hdc, efont, cxMax, cyMax, SN_EXP1 );
	_SETCOLOR( SN_EXP1 );
#ifdef EPWING
	if ( useuniqfont & SNF_EPWING )
		_SetupTextFonts( hdc, gfont, cxMax, cyMax, SN_EPWING );
	_SETCOLOR( SN_EPWING );
#endif
#ifdef USE_SINGLEBYTE
	if ( fSingleByte[ SN_WORD ] )
		s |= SRCH_WORD;
	if ( fSingleByte[ SN_PRON ] )
		s |= SRCH_PRON;
	if ( fSingleByte[ SN_JAPA ] )
		s |= SRCH_JAPA;
	if ( fSingleByte[ SN_EXP1 ] )
		s |= SRCH_EXP;

	dic.SetSingleByte( s );
#endif
#endif	// SQUFONT

#if SQU_DISP_NUM
	cxTextMax = max( cxMax, cxAttr );	// 最低nfont以上の大きさにする
#endif
	cyText = max( cyMax, cyAttr );

	cxLevelText = cxText;

	SetMaxLine( );

	// 不一致時の反転色
	COLORREF fore = GET_FORECOLOR( useuniqfont & SNF_WORD ? GetColor( SN_WORD ) : charcolor );
	COLORREF back = GET_BACKCOLOR( backcolor );
	graycolor = GetGrayReverse(fore, back);
}
void Squre::ResetFont()
{
	TRegKey *common = prof.SetSection(PFS_COMMON);
	LoadDicProfiles(CurGroupReg, common);
}
void Squre::ResetColor()
{
	ResetBackColor();
}
#ifndef NOSQUCOLOR
void Squre::ResetBackColor( )
{
	View->ResetBackColor(backcolor);
}
#endif
//---------------------------------------------------------------------------
//	Line Management
//---------------------------------------------------------------------------
void Squre::SetMaxLine( )
{
	MaxLines = GetY0() + GetLY() - GetHomeY();		// 最大一覧表示行数（ドット単位）
	if ( cyText == 0 ) return;
	int lh = cyText + ItemView.LineInterval;
	if ( MaxLines/lh > MAX_NDISP ){
		MaxLines = MAX_NDISP*lh;
	} else if ( MaxLines/lh < MIN_NDISP ){
		MaxLines = MIN_NDISP*lh;
	}
}
//---------------------------------------------------------------------------
//	Item View
//---------------------------------------------------------------------------
#if SQUFONT
void InitFont( TFontAttr &fa, int defpoint, const tchar *fontname )
{
	fa.Name = fontname;
	fa.CharSet = DEFAULT_CHARSET;		// 2008.2.23 changed.
	fa.Size = defpoint;
}
#endif
void Squre::InitFont()
{
	HDC hdc = ::GetDC( NULL );
	int logpixelsy = GetDeviceCaps( hdc, LOGPIXELSY );	// pixels per inch of the screen.
	int defpoint = -1 * ( ( logpixelsy * DEF_FONT_POINT ) / 72 );
	::ReleaseDC( NULL, hdc );

#if SQUFONT
	::InitFont( nfont, defpoint, prof.GetDefFontName() );
	::InitFont( wfont, defpoint, prof.GetDefFontName() );
	::InitFont( pfont, defpoint, prof.GetDefPronFontName() );
	::InitFont( jfont, defpoint, prof.GetDefFontName() );
	::InitFont( efont, defpoint, prof.GetDefFontName() );
#ifdef EPWING
	::InitFont( gfont, defpoint, prof.GetDefFontName() );
#endif
#endif
}
void Squre::ClearLastIndex()
{
	IndexOffset = 0;
	IndexMiOffset = 0;
	LastIndex = 0;
}
void Squre::InvalidateLastIndex()
{
	LastIndex = -1;
}
//---------------------------------------------------------------------------
//	Page control
//---------------------------------------------------------------------------
int Squre::GetLastPageOffset( )
{
	// 最終行を含んで一杯に表示できるときのoffsetを求める
	// もし、offset < 0 となるときは最適化の対象ではない
	// また、途中でlinelist[] = 0のときも中止する
	int lsum = 0;
	int i;
	for ( i=get_num()-1;i>=0;i-- ){
		RecalcLine( i );
		int l = LinesList[ i ].NumLines;
		if ( !l )
			return -1;
		lsum += l
#ifndef SMALL
			+ ItemView.LineInterval
#endif
		;
		if (lsum > MaxLines){
			break;
		}
	}
	return i+1;
}

