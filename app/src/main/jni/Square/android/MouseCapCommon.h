#ifndef __MouseCapCommon_h
#define	__MouseCapCommon_h

#include "defs.h"
#include "pdconfig.h"
#include "CharHT.h"
#include "wsqudef.h"	// for SN_OBJECT
#include "MouseCapBase.h"

// Used Timer.
#define	TM_ALTIMER		143
#define	TM_ALTIMERCLICK	145
//	TM_ALTIMERMOVE
//	IDT_DD
//	TM_MOUSETIMER
#define	TM_MOUSETIMER	1

class TMouseViewIFCommon : public TMouseViewIFBase {
typedef TMouseViewIFBase super;
public:
	// For TMouseCapture
	virtual bool CharHitTest( int no, struct CharHT &cht ){return false;}
	virtual int HitTest( POINT pt, UINT flag=0xffff, RECT *rcArea=NULL ){return HT_NONE;}			// HT_WORDITEM, HT_UPPER, HT_LOWERの判定
	virtual void MouseSelected(int wordIndex){}
	virtual void SetSelIndex(int index){}
	virtual void MousePopupMenu(int index, POINT pt){}
	virtual void MouseCancelCapture(){}

	// Object operations
	virtual class JLink *GetObject(int index, int objIndex) = 0;
#if 0
	virtual void MouseMoveObject(POINT &pt, int index,int objIndex) = 0;
	virtual void LButtonDownObject(UINT key, POINT &pt, int index, int objIndex, struct CharHT &cht) = 0;
	virtual void LButtonUpObject(POINT &pt, int index, int objIndex, UINT key) = 0;
	virtual bool DblClkObject(POINT &pt, int index, int objIndex){ return false; }
	virtual void RButtonDownObject(POINT &pt, int index, int objIndex, UINT key, CharHT &cht){}
	virtual void RButtonUpObject(POINT &pt, int index, int objIndex){}
#endif
	virtual void RedrawObject(int index, int objIndex) = 0;
	virtual void StartDDObject(int index, int objIndex) = 0;
	virtual tnstr GetWord(int index) = 0;	// object magnifyをsupportする場合は必要
	virtual bool EditObject(int index, int objIndex, int verb) = 0;
	virtual void JLinkPopupMenu(JLink *jl, POINT pt) = 0;

	// For TMouseCapture and TAutoLink
	virtual bool IsAutoLinkEnabled(){ return false; }
	virtual bool IsAutoLinkPopupOpened() { return false; }
	virtual void CloseAllPopup(){}

	// Common
	virtual HDC GetDC() = 0;
	virtual void ReleaseDC() = 0;
	virtual void InvalidateLine(int index) = 0;

	// Draw for TMouseCapture
	virtual void MouseChangeAttr(int index, uchar bit, const RECT &rcCapAttr){}
	virtual void DrawCaptureAttr(const RECT &rcArea, RECT *rcCapAttr, int httype){}

	// Draw for Auto Link
	virtual int HitTestHyperLink(int hitno, POINT pt){ return -1; }

	// Timer //
	virtual bool SetTimer(int id, int timer);
	virtual void KillTimer(int id);

	virtual void SetCapture();
	virtual void SetCursor(const tchar *name){}

protected:
	virtual HWND GetHandle() = 0;
};

class TAutoLinkCommon {
protected:
	TMouseViewIFCommon &View;
	int CursorState;
public:
	TAutoLinkCommon(TMouseViewIFCommon &view);
	void MouseMove(POINT pt, UINT key, int ht);
};

//Note: Derived class should handle EvTimer().
class TMouseCaptureCommon : public TMouseCaptureBase {
typedef TMouseCaptureBase super;
protected:
	TMouseViewIFCommon &View;
	TAutoLinkCommon &al;

	int hitno;			// ヒット単語番号
	// 範囲選択に関するメンバ	///////////
	int rvStart;		// 開始文字位置
	int rvEnd;			// 終了文字位置
	int rvItem;			// 選択対象(SN_WORD,SN_JAPA）
#if USE_DT2
	CharHT htPivot;		// 左クリック開始点情報
#endif
	int ddStart;		// Timer ID

	// timer
	UINT mousetimer;

	RECT rcCapAttr;		// Capturing rect for Attr.
	int inxCapture;		// 同上

	int LastMoveIndex;
	int LastMoveObjIndex;
	POINT ptCapture;	// オブジェクトのキャプチャー用

public:
	TMouseCaptureCommon(TMouseViewIFCommon &view, TAutoLinkCommon &al);

	int GetSelIndex() const
		{ return hitno; }
	int GetSelItemIndex() const
		{ return rvItem; }
	void SetSelIndex(int index, int itemIndex)	// wsobj.cpp専用 -> 削除したい
		{ hitno = index; rvItem = itemIndex; }
	void SetSelItemIndex(int index)
		{ rvItem = index; }

	POINT GetCapturePoint()
		{ return ptCapture; }
	void SetCapture(POINT pt)
		{ ptCapture = pt; }

	// Event Handlers //
	virtual void LButtonDown(POINT pt, UINT key);
	virtual void LButtonUp(POINT pt, UINT key);
	virtual void MouseMove(POINT &pt, UINT key);
	//virtual void LButtonDblClk(POINT pt, UINT key){}
	void EvTimer(int id);

	void AlTimer(int timerId);
	void Deactivate();
	void ReleaseCaptureCommon();

	virtual void ClsRegion( ) = 0;

	// Methods for TAutoLink class
	void CancelCapture();

	bool CompObjIndex(int _index, int _objIndex)
		{ return hitno==_index && rvItem==_objIndex+SN_OBJECT; }

	bool CanAutoLink()
		{ return IsAutoLinkEnabled() && !IsTextSelected(); }

protected:
	virtual void DDStart() = 0;
	virtual void TextCapture( POINT &pt ) = 0;
	//void SelectText( int end );
	virtual void CaptureAttr( int ht, const RECT &rcArea ){}

	bool IsTextSelected()	// 範囲選択中か？
		{ return rvItem!=-1 && rvItem<SN_OBJECT && rvStart!=rvEnd; }
	
	// Delegators //
	//const tchar *GetText(int wordIndex, int itemIndex)
	//	{ return View.GetText(wordIndex, itemIndex); }
	bool CharHitTest( int no, CharHT &cht )
		{ return View.CharHitTest(no ,cht); }
	int HitTest( POINT pt, UINT flag=0xffff, RECT *rcArea=NULL )			// HT_WORDITEM, HT_UPPER, HT_LOWERの判定
		{ return View.HitTest(pt, flag, rcArea); }
	void MouseSelected(int wordIndex)
		{ View.MouseSelected(wordIndex); }
	void SetSelIndex(int index)
		{ View.SetSelIndex(index); }
	void MousePopupMenu(int index, POINT pt)
		{ View.MousePopupMenu(index, pt); }

	HDC GetDC()
		{ return View.GetDC(); }
	void ReleaseDC()
		{ View.ReleaseDC(); }

	virtual void SelectObject(int hitno, CharHT *cht, bool unredraw=false) = 0;
	virtual void UnselectObject(int hitno, CharHT *cht) = 0;

	void DrawCaptureAttr(const RECT &rcArea, RECT *rcCapAttr, int httype)
		{ View.DrawCaptureAttr(rcArea, rcCapAttr, httype); }
	virtual void MouseMicroScrollUp(int itemIndex){}
	virtual void MouseMicroScrollDown(int itemIndex){}

	void SetCapture()
		{ View.SetCapture(); }

	bool SetTimer(int id, int timer)
		{ return View.SetTimer(id, timer); }
	void KillTimer(int id)
		{ View.KillTimer(id); }
	void StopMouseTimer();

	bool IsAutoLinkEnabled()
		{ return View.IsAutoLinkEnabled(); }

	// mouse operation for object
	void MouseMoveObject(POINT &pt, int index, int objIndex);
	void LButtonDownObject(UINT key, POINT &pt, int index, int objIndex, CharHT &cht);
	void LButtonUpObject(POINT &pt, int index, int objIndex, UINT key);
	bool DblClkObject(POINT &pt, int index, int objIndex);
	void RButtonDownObject(POINT &pt, int index, int objIndex, UINT key, CharHT &cht);
	void RButtonUpObject(POINT &pt, int index, int objIndex);
		
	// for TAutoLink
	virtual void alLButtonUp(POINT pt, UINT key){}
	virtual void alMouseMove(POINT pt, UINT key, int ht)
		{ al.MouseMove(pt, key, ht); }
	virtual void alMouseMoveEnter(){}
};

#endif	// __MouseCapCommon_h

