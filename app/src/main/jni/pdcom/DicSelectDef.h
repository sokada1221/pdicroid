//---------------------------------------------------------------------------

#ifndef DicSelectDefH
#define DicSelectDefH
//---------------------------------------------------------------------------

#include "dicdef.h"
#include "diclist.h"

enum {
	DL_MULTI = 1,	// 複数選択
	DL_DISRO = 2,	// namesのreadonlyが1以上のときは選択不可にする
};

struct TRDicSelect {
	bool alldic;
	diclist_t diclist;

	enum { SelEmpty, SelAll };

	TRDicSelect(){}

	TRDicSelect(int init_mode)
	{
		switch(init_mode){
			case SelEmpty:
				alldic = true;
				dl_empty(diclist);
				break;
			case SelAll:
				alldic = true;
				//dl_all(diclist);
				break;
		}
	}

	// int numdics;	// 使用辞書数
	bool IsSelected(int numdics) const
	{
		if (alldic)
			return true;
		return dl_is_selected(diclist, numdics);
	}
};

// Item selection //
struct TRItemSelect {
	bool itemWord;
	bool itemJapa;
	bool itemExp;
	bool itemPron;
//	bool itemObjTitle;
//	bool itemFileLinkFileName;

	void SetPartBySqm(int sqm);
	int GetPartBySqm();
	SrchMode GetSrchMode();
};

#endif
