//---------------------------------------------------------------------------

#ifndef SquWidthAttrH
#define SquWidthAttrH
//---------------------------------------------------------------------------

#include "wsqudef.h"

//	表示幅属性構造体(初期化用)
struct WidthAttrInit {
//	const tchar *title;
	int width;			// profile上は1/8文字単位、動作中はドット単位
	WORD attr;
	COLORREF color;

	static tnstr GetTitle(int index);
};


//	表示幅属性構造体
struct WidthAttr {
	tnstr title;
	int width;			// 1/8文字単位
	WORD attr;
	COLORREF color;
	void Set( WidthAttrInit &wai, int index );
};

extern WidthAttrInit widthattrinit[ N_SECTION + 1 ];

#endif

