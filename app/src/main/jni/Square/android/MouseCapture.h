//---------------------------------------------------------------------------

#ifndef MouseCaptureH
#define MouseCaptureH
//---------------------------------------------------------------------------

#include "MouseCapCommon.h"
#include "squfont.h"
#include "hyplink.h"

class TMouseViewIF : public TMouseViewIFCommon {
protected:
	class Squre *squ;
public:
	TMouseViewIF(class Squre *squ);

	// 将来的に自分のclassに持って行きたい
	virtual class THyperLink &_GetHyperLink(int wordIndex, int itemIndex);

	// Virtualized methods //
	__override void SetHintText(const tchar *msg);

	// For TMouseCapture
	virtual bool IsValidHitno(int selIndex);
	virtual bool IsVisibleHitno(int sel_index);
	virtual const tchar *GetText(int wordIndex, int itemIndex);
	virtual bool CharHitTest( int no, CharHT &cht );
	virtual int HitTest( POINT pt, UINT flag=0xffff, RECT *rcArea=NULL );			// HT_WORDITEM, HT_UPPER, HT_LOWERの判定
	virtual void MouseSelected(int wordIndex);
	virtual void SetSelIndex(int index);
	virtual void MousePopupMenu(int index, POINT pt);
	virtual void MouseCancelCapture();
	virtual void ClearObject();

	// For TMouseCapture
	virtual class JLink *GetObject(int index, int objIndex);
	virtual void RedrawObject(int index, int objIndex);
	virtual void StartDDObject(int index, int objIndex);
	virtual tnstr GetWord(int index);
	virtual bool EditObject(int index, int objIndex, int verb);
	virtual void JLinkPopupMenu(JLink *jl, POINT pt);

	virtual int MouseMicroScrollUp(int itemIndex);
	virtual int MouseMicroScrollDown(int itemIndex);

	// For TMouseCapture and TAutoLink
	__override bool IsAutoLinkEnabled();
	virtual bool ReverseAutoLink( POINT *pt, bool checkhyp, bool autolink );
	virtual void CloseAllPopup();
	//virtual void CloseAutoLink();
	__override bool IsAutoLinkPopupOpened();
	virtual void HypLinkJump( THyperLink &hl, POINT &pt, int poolindex );
	virtual bool HypLinkJumpTest( int index, POINT pt );
	virtual void LinkJump( const tchar *keyword );

	// Common
	virtual HDC GetDC();
	virtual void ReleaseDC();
	virtual void CreateTextFonts();
	virtual void DeleteTextFonts();
	virtual void InvalidateLine(int index);

	// Draw for TMouseCapture
	virtual void MouseChangeAttr(int index, uchar bit, const RECT &rcCapAttr);
	virtual void DispOneLine(int hitno, int flags, CharHT *cht);
	virtual void DrawCaptureAttr(const RECT &rcArea, RECT *rcCapAttr, int httype);
	
	// Draw for Auto Link
	virtual int HitTestHyperLink(int hitno, POINT pt);
	virtual void DrawUnderline( int item, int start0, int end0, int start, int end, int hitno );

	virtual void ReturnProc();

	virtual void SetCursor(const tchar *name);

protected:
	__override HWND GetHandle();
};

class TAutoLink : public TAutoLinkCommon {
typedef TAutoLinkCommon super;
protected:
	TMouseViewIF &View;
	class TMouseCapture &Cap;
	int alHitno;
	int alItem;
	int alIndex;
	int alStart;
	int alEnd;

	int alWordCount;
	int alWaitTimer;
	POINT alPoint;

	// 自動ポップアップだけで使用
	THyperLink alHyperLink;
public:
	TAutoLink(TMouseViewIF &view, TMouseCapture &cap);

	// Event Handlers //
	void LButtonUp(POINT pt, UINT key);
	bool LButtonDblClk(POINT pt, UINT key);
	//void MouseMove(POINT pt, UINT key, int ht);

	bool IsIndexValid() const
		{ return alItem!=-1; }
	void InvalidateIndex()
		{ alItem = -1; }
	int GetSelIndex() const
		{ return alHitno; }
	int GetItemIndex() const
		{ return alItem; }
	int GetHypIndex() const
		{ return alIndex; }
	void SetIndex(int selIndex, int itemIndex, int hypIndex, int start, int end)
	{
		alHitno = selIndex;
		alItem = itemIndex;
		alIndex = hypIndex;
		alStart = start;
		alEnd = end;
	}
	THyperLink &GetHyperLink()
		{ return alHyperLink; }
	bool CompIndex(int sel_index, int item_index, int hyp_index=-2, int start=-2);

	int GetWordCount() const
		{ return alWordCount; }
	void SetWordCount(int count)
		{ alWordCount = count; }

	POINT GetPoint() const
		{ return alPoint; }
	void SetPoint(POINT pt)
		{ alPoint = pt; }
	
	void DrawUnderline();

	void StartHypPopup(THyperLink &hl, POINT &pt);
	
protected:
	// Helpers //
	void GetDC()
		{ View.GetDC(); }
	void ReleaseDC()
		{ View.ReleaseDC(); }
	void CreateTextFonts()
		{ View.CreateTextFonts(); }
	void DeleteTextFonts()
		{ View.DeleteTextFonts(); }
	bool SetTimer(int id, int timer)
		{ return View.SetTimer(id, timer); }
	void KillTimer(int id)
		{ View.KillTimer(id); }
};

class TMouseCapture : public TMouseCaptureCommon {
typedef TMouseCaptureCommon super;
protected:
	TMouseViewIF View;
public:
	TAutoLink al;
protected:
	//int hitno;			// ヒット単語番号
public:
	TMouseCapture(Squre *squ);

	bool IsSelected( );	// 範囲選択されているか？(テキストコピー用)
	bool IsValidHitno()
	{
		if (rvItem==-1) return false;
		return View.IsValidHitno(hitno);
	}
	bool IsVisibleHitno(int sel_index)
		{ return View.IsVisibleHitno(sel_index); }

	// Event Handlers //
	//void LButtonDown(POINT pt, UINT key);
	//void LButtonUp(POINT pt, UINT key);
	void RButtonDown(POINT pt, UINT key);
	void RButtonUp(POINT pt, UINT key);
	//void MouseMove(POINT &pt, UINT key);
	void LButtonDblClk(POINT pt, UINT key);
	//void EvTimer(int id);
	//void AlTimer(int timerId);
	//void Deactivate();
	//void ReleaseCaptureCommon();

	__override void ClsRegion( );

	const tchar *GetSelText(int *len);

	void OffsetIndex(int offset);

	// special helpers //
	CharHT *CreateCharHTDispStar();
	void DrawSelItem();
	void DrawAttr();
	int BackupSel();
	void RestoreSel(int _rvItem);

protected:
	__override void DDStart();
	__override void TextCapture( POINT &pt );
	void SelectText( int end );
	__override void CaptureAttr( int ht, const RECT &rcArea );

	// Delegators //
	const tchar *GetText(int wordIndex, int itemIndex)
		{ return View.GetText(wordIndex, itemIndex); }
	//bool CharHitTest( int no, CharHT &cht )
	//	{ return View.CharHitTest(no ,cht); }
	//int HitTest( POINT pt, UINT flag=0xffff, RECT *rcArea=NULL )			// HT_WORDITEM, HT_UPPER, HT_LOWERの判定
	//	{ return View.HitTest(pt, flag, rcArea); }
	//void MouseSelected(int wordIndex, int way)
	//	{ View.MouseSelected(wordIndex, way); }
	//void SetSelIndex(int index)
	//	{ View.SetSelIndex(index); }
	//void MousePopupMenu(int index, POINT pt)
	//	{ View.MousePopupMenu(index, pt); }

	//void GetDC()
	//	{ View.GetDC(); }
	//void ReleaseDC()
	//	{ View.ReleaseDC(); }
	void CreateTextFonts()
		{ View.CreateTextFonts(); }
	void DeleteTextFonts()
		{ View.DeleteTextFonts(); }

	void DrawTextSelection(int hitno, CharHT *cht);
	__override void SelectObject(int hitno, CharHT *cht, bool unredraw=false);
	__override void UnselectObject(int hitno, CharHT *cht);
	void DrawItem(int hitno, CharHT *cht);
	//void DrawCaptureAttr(const RECT &rcArea, RECT *rcCapAttr, int httype)
	//	{ View.DrawCaptureAttr(rcArea, rcCapAttr, httype); }
	__override void MouseMicroScrollUp(int itemIndex);
	__override void MouseMicroScrollDown(int itemIndex);

	//void SetCapture()
	//	{ super::SetCapture(); }
	void ReturnProc()
		{ View.ReturnProc(); }

	//bool SetTimer(int id, int timer)
	//	{ return View.SetTimer(id, timer); }
	//void KillTimer(int id)
	//	{ View.KillTimer(id); }
	//void StopMouseTimer();

	//bool IsAutoLinkEnabled()
	//	{ return View.IsAutoLinkEnabled(); }

	// for TAutoLink
	__override void alLButtonUp(POINT pt, UINT key)
		{ al.LButtonUp(pt, key); }
//	__override void alMouseMove(POINT pt, UINT key, int ht)
//		{ al.MouseMove(pt, key, ht); }
	__override void alMouseMoveEnter();
};

#endif

