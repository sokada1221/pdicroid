//---------------------------------------------------------------------------
#include "tnlib.h"
#pragma hdrstop
#include "defs.h"
#include "pdprof.h"

#include "SquItemView.h"
#include "wpdcom.h"
#include "squfont.h"
#include "pfs.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------
// TSquItemView class
//---------------------------------------------------------------------------
#define	DELIM_ITEMNAME	':'		//TODO 5: レジストリの区切り文字
TSquItemView::TSquItemView()
{
	LineInterval = 0;
	ItemOrderNum = 0;
}
TSquItemView::~TSquItemView()
{
}
void TSquItemView::Setup()
{
#ifdef _Windows
	// 表示幅の単位変換
	cxSys = GetSystemFontWidth();
	for ( int i=0;i<N_SECTION;i++ ){
		widthattr[i].width = widthattrinit[i].width * cxSys / 8;
	}

	HDC hdc = ::GetDC(NULL);	// 本当はSquView->Canvas->Handleがいいかも
	TEXTMETRIC tm;
	GetTextMetrics( hdc, &tm );
	int cxAttr = tm.tmAveCharWidth;
	::ReleaseDC(NULL, hdc);

	_SetItemWidth( SN_NUMBER, max( cxAttr*5, cxSys*4 ) );
	_SetItemWidth( SN_ATTR, max( cxAttr, cxSys )*4 + 6 );	// "暗"と"修"の長さ
	BuildUpItemOrder();
#endif
}

void TSquItemView::InitWidthAttr( )
{
	int i;
	for ( i=0;i<N_SECTION;i++ ){
		widthattr[i].Set( widthattrinit[i], i );
#ifndef NOSQUCOLOR
		LinkColors[i] = DEF_LINKCOLOR;
#endif
	}
	widthattr[i].title.set( WidthAttrInit::GetTitle(i) );
}
void TSquItemView::_SetItemWidth(int index, int width)
{
	widthattr[index].width = width;
}
void TSquItemView::BuildUpItemOrder()
{
	// Build up the display item order.
	int i = 0;
	if (IsVisible(SN_ATTR))
		ItemOrder[i++] = SN_ATTR;
	if (IsVisible(SN_WORD))
		ItemOrder[i++] = SN_WORD;
	if (IsVisible(SN_PRON)){
		ItemOrder[i++] = SN_AUDIO;
		ItemOrder[i++] = SN_PRON;
	}
	if (IsVisible(SN_LEVEL))
		ItemOrder[i++] = SN_LEVEL;
	ItemOrder[i] = -1;
	ItemOrderNum = i;
}
int TSquItemView::GetItemIndex(int index)
{
	if (index>=ItemOrderNum)
		return -1;
	return ItemOrder[index];
}
#if 0
tnstr TSquareFrame::GetTitleEx( int type )
{
	if ( type == SN_JAPA /* || type == SN_EXP*/ ){
		if ( IsVisible( SN_EXP ) ){
			int l = _tcslen( GetTitle( SN_JAPA ) ) + _tcslen( GetTitle( SN_EXP ) ) + 10;
			tchar *p = new tchar[ l ];
			_tcscpy( p, GetTitle( SN_JAPA ) );
			_tcscat( p, _T(" / ") );
			_tcscat( p, GetTitle( SN_JAPA+1 ) );
			return p;
		} else {
			return newstr( GetTitle( SN_JAPA ) );
		}
	} else {
		return newstr( GetTitle( type ) );
	}
}
void TSquareFrame::ToggleView( int type )
{
	if ( type == SN_JAPA )
		type = SN_EXP;
	widthattr[type].attr ^= WA_VIEW;
	GetActive()->InvalidateLines();
#ifdef WINCE
	GetActive()->Invalidate();
#endif
	BuildUpItemOrder();
}
#endif	// 0
void TSquItemView::SetVisible(int index, bool flag)
{
	widthattr[index].attr = (widthattr[index].attr & ~WA_VIEW) | (flag ? WA_VIEW : 0);
	BuildUpItemOrder();
}

COLORREF TSquItemView::GetLinkColor( int type )
{
	return ::LinkColors[type];
}
void TSquItemView::SetLinkColor( int type, COLORREF color )
{
	::LinkColors[ type ] = color;
}
int TSquItemView::GetOffs(int type)
{
	int x = 0;
	for ( int i=0;;i++ ){
		int item = GetItemIndex(i);
		if (item==-1)
			break;
		if (item==type){
			return x;
		}
		x += widthattr[item].width;
	}
	return x;	// 非表示項目もここに来る
}

void TSquItemView::LoadProfile(TRegKey *key)
{
	UINT array[ N_SECTION ];
	int i;
	for ( i=0;i<N_SECTION;i++ ){
		array[i] = widthattr[i].width * 8 / cxSys;
	}
	key->GetVarArray( PFS_TITLEWIDTH, array, N_SECTION );
	for ( i=0;i<N_SECTION;i++ ){
		widthattr[i].width = array[i] * cxSys / 8;
	}
	for ( i=0;i<N_SECTION;i++ ){
		array[i] = ( widthattr[i].attr & WA_VIEW ) == WA_VIEW;
	}
	key->GetVarArray( PFS_TITLEVIEW, array, N_SECTION );
	// 表示のON/OFF
	for ( i=0;i<N_SECTION;i++ ){
		WORD attr = (WORD)(widthattr[i].attr & ~WA_VIEW);
		if ( i==SN_WORD )
			attr |= WA_VIEW;	//
		widthattr[i].attr = (WORD)(attr | ( array[i] ? WA_VIEW : 0 ));
	}

	// 表示項目名
	tnstr_vec strarray;
	key->Read(PFS_ITEMNAME, strarray, DELIM_ITEMNAME);
	for ( int i=0;i<N_SECTION+1;i++ ){
		if (i<strarray.get_num()){
			widthattr[i].title.set(strarray[i]);
		} else {
			widthattr[i].title.set( WidthAttrInit::GetTitle(i) );
		}
	}

	LineInterval = key->ReadInteger( PFS_LINTER, LineInterval );
	BuildUpItemOrder();
}

void TSquItemView::SaveProfile(TRegKey *key)
{
	UINT array[ N_SECTION ];
	int i;
	for ( i=0;i<N_SECTION;i++ ){
		array[i] = widthattr[i].width * 8 / cxSys;
	}
	key->PutVarArray( PFS_TITLEWIDTH, array, N_SECTION );

	// 表示のON/OFF
	for ( i=0;i<N_SECTION;i++ ){
		array[i] = ( widthattr[i].attr & WA_VIEW ) == WA_VIEW;
	}
	key->PutVarArray( PFS_TITLEVIEW , array, N_SECTION );

	// 表示項目名
	tnstr_vec strarray;
	for ( int i=0;i<N_SECTION+1;i++ ){
		strarray.add(widthattr[i].title);
	}
	key->Write(PFS_ITEMNAME, strarray, DELIM_ITEMNAME);

	key->WriteInteger( PFS_LINTER, LineInterval );
}
