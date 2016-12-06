//---------------------------------------------------------------------------

#ifndef SquItemViewH
#define SquItemViewH
//---------------------------------------------------------------------------

#include "SquWidthAttr.h"

class TSquItemView {
protected:
	WidthAttr widthattr[ N_SECTION + 1 + 1];		// titleが訳と用例であるため
	int cxSys;
public:
	int LineInterval;
public:
	TSquItemView();
	virtual ~TSquItemView();
	void Setup();
	void InitWidthAttr( );
	WidthAttr &GetWidthAttr(int index)
		{ return widthattr[index]; }
protected:
	void _SetItemWidth(int index, int width);
public:
	// これを呼び出したら、TSquareFrame::GetActive()->Invalidate()を呼び出すこと！
	void SetItemWidth(int index, int width)
		{ _SetItemWidth(index, width); }
protected:
	int ItemOrder[N_SECTION+1];
	int ItemOrderNum;
	void BuildUpItemOrder();
public:
	// indexからSN_xxxを求める
	// ＝index番目に表示するitemIdを返す
	int GetItemIndex(int index);
public:
	tnstr GetTitleEx( int type );	// new version of GetTitle2()
	void ToggleView( int type );
	int GetWidth( int type )
		{ return widthattr[ type ].width; }
	bool IsVisible( int type )
		{ return (widthattr[ type ].attr & WA_VIEW)?true:false; }
	void SetVisible(int index, bool flag);

	COLORREF GetColor( int type )
		{ return widthattr[ type ].color; }
	COLORREF GetLinkColor( int type );
	const tchar *GetTitle( int type )
		{ return widthattr[ type ].title; }
	void SetTitle( int type, const tchar *newtitle )
		{ widthattr[ type ].title.set( newtitle ); }
	void SetColor( int type, COLORREF color )
		{ widthattr[ type ].color = color; }
	void SetLinkColor( int type, COLORREF color );
	int GetOffs(int type);

	void LoadProfile(class TRegKey *key);
	void SaveProfile(class TRegKey *key);
};

#endif
