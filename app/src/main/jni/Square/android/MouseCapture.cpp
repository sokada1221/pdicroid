//---------------------------------------------------------------------------

#include "tnlib.h"
#pragma hdrstop

#include "MouseCapture.h"
#include "winsqu.h"
#include "autolink.h"	// for AutoLinkCtrlKey

//---------------------------------------------------------------------------

#pragma package(smart_init)

TMouseViewIF::TMouseViewIF(class Squre *_squ)
	:squ(_squ)
{
}

THyperLink &TMouseViewIF::_GetHyperLink(int wordIndex, int itemIndex)
{
	//return squ->_GetHyperLink(wordIndex, itemIndex);
	return *(THyperLink*)NULL;
}

void TMouseViewIF::SetHintText(const tchar *msg)
{
	//squ->SetHintText(msg);
}

bool TMouseViewIF::IsValidHitno(int selIndex)
{
	//return squ->IsValidHitno(selIndex);
	return false;
}

bool TMouseViewIF::IsVisibleHitno(int sel_index)
{
	//return squ->IsVisibleHitno(sel_index);
	return false;
}

const tchar *TMouseViewIF::GetText(int wordIndex, int itemIndex)
{
	//return squ->GetText(wordIndex, itemIndex);
	return NULL;
}

bool TMouseViewIF::CharHitTest( int no, CharHT &cht )
{
	//return squ->CharHitTest(no, cht);
	return false;
}

int TMouseViewIF::HitTest( POINT pt, UINT flag, RECT *rcArea)
{
	//return squ->HitTest(pt, flag, rcArea);
	return -1;
}
void TMouseViewIF::MouseSelected(int index)
{
	//squ->MouseSelected(index);
}
void TMouseViewIF::SetSelIndex(int index)
{
	//squ->SetSelIndex(index);
}
void TMouseViewIF::MousePopupMenu(int index, POINT pt)
{
	//squ->MousePopupMenu(index, pt);
}
void TMouseViewIF::MouseCancelCapture()
{
	//squ->MouseCancelCapture();
}
void TMouseViewIF::ClearObject()
{
	//squ->ClearObject();
}

class JLink *TMouseViewIF::GetObject(int index, int objIndex)
{
	//return squ->GetJLinkObject(index, objIndex);
	return NULL;
}

void TMouseViewIF::RedrawObject(int index, int objIndex)
{
	//squ->RedrawObject(index, objIndex);
}

void TMouseViewIF::StartDDObject(int index, int objIndex)
{
	//squ->StartDDObject(index, objIndex);
}

tnstr TMouseViewIF::GetWord(int index)
{
	//return squ->GetWord(index);
	return NULL;
}
bool TMouseViewIF::EditObject(int index, int objIndex, int verb)
{
	//return squ->EditObject(index, objIndex, verb);
	return false;
}

void TMouseViewIF::JLinkPopupMenu(JLink *jl, POINT pt)
{
	//squ->JLinkPopupMenu(jl, pt);
}

int TMouseViewIF::MouseMicroScrollUp(int itemIndex)
{
	//return squ->MicroScrollUp(itemIndex);
	return -1;
}
int TMouseViewIF::MouseMicroScrollDown(int itemIndex)
{
	//return squ->MicroScrollDown(itemIndex);
	return -1;
}

bool TMouseViewIF::ReverseAutoLink( POINT *pt, bool checkhyp, bool autolink )
{
	//return squ->ReverseAutoLink(pt, checkhyp, autolink);
	return false;
}
void TMouseViewIF::CloseAllPopup()
{
	//squ->CloseAllPopup();
}
bool TMouseViewIF::IsAutoLinkEnabled()
{
	//return squ->IsAutoLinkEnabled();
	return false;
}

#if 0
void TMouseViewIF::CloseAutoLink()
{
	squ->CloseAutoLink();
}
#endif
bool TMouseViewIF::IsAutoLinkPopupOpened()
{
	//return squ->IsAutoLinkPopupOpened();
	return false;
}

void TMouseViewIF::HypLinkJump( THyperLink &hl, POINT &pt, int poolindex )
{
	//squ->HypLinkJump(hl, pt, poolindex);
}
bool TMouseViewIF::HypLinkJumpTest( int index, POINT pt )
{
	//return squ->HypLinkJumpTest(index, pt);
	return false;
}

void TMouseViewIF::LinkJump( const tchar *keyword )
{
	//squ->LinkJump(keyword);
}

HDC TMouseViewIF::GetDC()
{
	return squ->GetDC();
}
void TMouseViewIF::ReleaseDC()
{
	//squ->ReleaseDC();
}
void TMouseViewIF::CreateTextFonts()
{
	//squ->CreateTextFonts();
}
void TMouseViewIF::DeleteTextFonts()
{
	//squ->DeleteTextFonts();
}

void TMouseViewIF::InvalidateLine(int index)
{
	//squ->InvalidateLine(index);
}

void TMouseViewIF::MouseChangeAttr(int index, uchar bit, const RECT &rcCapAttr)
{
	//squ->MouseChangeAttr(index, bit, rcCapAttr);
}

void TMouseViewIF::DispOneLine(int hitno, int flags, CharHT *cht)
{
	//squ->DispOneLine(hitno, flags, cht);
}

void TMouseViewIF::DrawCaptureAttr(const RECT &rcArea, RECT *rcCapAttr, int httype)
{
	//squ->DrawCaptureAttr(rcArea, rcCapAttr, httype);
}

int TMouseViewIF::HitTestHyperLink(int hitno, POINT pt)
{
	//return squ->HitTestHyperLink(hitno, pt);
	return -1;
}
void TMouseViewIF::DrawUnderline( int item, int start0, int end0, int start, int end, int hitno )
{
	//squ->DrawUnderline(item, start0, end0, start, end, hitno);
}

void TMouseViewIF::ReturnProc()
{
	//squ->MouseReturnProc();
}

void TMouseViewIF::SetCursor(const tchar *name)
{
	//squ->SetCursor(name);
}

HWND TMouseViewIF::GetHandle()
{
	return squ->GetWHandle();
}

//
// TAutoLink class
//
TAutoLink::TAutoLink(TMouseViewIF &view, TMouseCapture &cap)
	:super(view)
	,View(view)
	,Cap(cap)
{
	alItem = -1;
}

void TAutoLink::LButtonUp(POINT pt, UINT key)
{
#if 0
	if ( alItem != -1 ){
#if !VIEWCLICKONLY
		if ( HyperClickPopup )
#endif
		{
			View.ReverseAutoLink( &pt, false
#if NEWAUTOLINK
				, AutoLinkCtrlKey ? key & MK_CONTROL : true
#elif USE_DT2 && !defined(SML)
				, AutoLinkConfig.fEnable
#else
				, false
#endif
				 );
			alPoint = pt;
			View.SetTimer( TM_ALTIMERCLICK, GetDoubleClickTime() /4 + 10);
		}
#if !VIEWCLICKONLY
		else
		{
			HypLinkJumpTest( pt, hitno );
		}
#endif
	}
#endif
}

// return:
//	return true if processed.
bool TAutoLink::LButtonDblClk(POINT pt, UINT key)
{
#if NEWAUTOLINK
	if ( alItem != -1 ){
		Cap.CancelCapture();
		View.KillTimer( TM_ALTIMERCLICK );
		if ( alIndex != -1 ){
			THyperLink &hl = View._GetHyperLink(alHitno, alIndex);
			View.HypLinkJump( hl, pt, alHitno );
		} else {
			if (Cap.CanAutoLink()){
				View.LinkJump( alHyperLink.key );
			}
		}
		return true;
	} else
#endif
	{
		// HyperLinkチェック
		if ( View.HypLinkJumpTest( Cap.GetSelIndex(), pt ) ){	//TODO: Cap.GetSelIndex()はalHitnoではない？？
			Cap.CancelCapture();
			return true;
		}
	}
	return false;
}

bool TAutoLink::CompIndex(int sel_index, int item_index, int hyp_index, int start)
{
	return alHitno==sel_index && alItem==item_index && (hyp_index==-2 || alIndex==hyp_index) && (start==-2 || alStart==start);
}

void TAutoLink::DrawUnderline()
{
	GetDC();
	CreateTextFonts();
	View.DrawUnderline( alItem, alStart, alEnd, alStart, alEnd, alHitno );
	DeleteTextFonts();
	ReleaseDC();
}

void TAutoLink::StartHypPopup(THyperLink &hl, POINT &pt)
{
#ifndef SMALL
	alWaitTimer = SetTimer( TM_ALTIMER,
#if !VIEWCLICKONLY
		(!HyperClickPopup && delayed) ? waittime*100 + 10 :
#endif
			1);
#else
	alWaitTimer = SetTimer( TM_ALTIMER, 1);	//*** どうしようか？
#endif
	alWordCount = hl.wordcount;	// 同じヒットがどうかのために保存
	alPoint = pt;
}
//
// TMouseCapture class
//
TMouseCapture::TMouseCapture(Squre *squ)
	:super(View, al)
	,View(squ)
	,al(View, *this)
{
}

// 範囲選択されているか？(テキストコピー用)
bool TMouseCapture::IsSelected( )
{
	return rvItem != -1 && rvStart != -1 && rvStart != rvEnd && IsValidHitno();
}

void TMouseCapture::RButtonDown(POINT pt, UINT key)
{
	int ht = HitTest( pt );
	int inx = ht & 0xff;
	int httype = ht & ~0xff;
	switch ( httype ){
		case HT_WORDITEM:
			{
#if USE_DT2
				if ( rvItem != -1 && rvStart != rvEnd && IsValidHitno() && IsVisibleHitno(hitno) ){
					// 範囲選択されている場合はヒットテストなし
					// 範囲選択が画面上にない場合は通常処理
					inx = hitno;
					goto jmp2;
				}
#endif
				GetDC( );
				MouseSelected(inx);
				if (inx != GetSelIndex()){
					ClsRegion( );
					SetSelIndex(inx);
				}
#if USE_DT2
				CharHT cht( -1, &pt );
				bool fHit = CharHitTest( inx, cht );
				if ( fHit ){
					// 範囲選択
					if ( cht.item < SN_OBJECT && rvItem >= SN_OBJECT ){
						// 前回までがオブジェクトであり、今回のクリックがオブジェクト以外の場合
						//cht.item = rvItem;
						//inx = hitno;
						ClsRegion();
					}
#ifdef USE_JLINK
					if ( cht.item >= SN_OBJECT ){
						RButtonDownObject(pt, inx, cht.item-SN_OBJECT, key, cht);
						ReleaseDC();
						break;
					} else
#endif	// USE_JLINK
					{
						hitno = inx;
						rvItem = cht.item;
						rvStart = rvEnd = cht.pos;
						ReleaseDC( );
						goto jmp1;		// 通常の右クリックメニューへ
					}
				}
#endif
				ReleaseDC( );
				goto jmp2;
			}
			// fall through
		case HT_MEMORY:
		case HT_MODIFY:
jmp1:;
			{
			MouseSelected(inx);
jmp2:
			MousePopupMenu(inx, pt);
			return;
			}
	}
}

void TMouseCapture::RButtonUp(POINT pt, UINT key)
{
	switch ( fCapturing & HT_MASK ){
#ifdef USE_JLINK
		case HT_WORDITEM:
			{
				// Get the released hit test info.
				int r_ht = HitTest(pt);
				int r_inx = r_ht & 0xff;
				//int r_httype = r_ht & ~0xff;

				if ( rvItem >= SN_OBJECT ){
					// Released on the Object.
					// Not support the auto link for now.
					CharHT r_cht( -1, &pt );
					CharHitTest( r_inx, r_cht );
					RButtonUpObject(pt, r_inx, r_cht.item-SN_OBJECT);
				}
			}
			break;
#endif
	}
	fCapturing = FALSE;
}

void TMouseCapture::LButtonDblClk(POINT pt, UINT key)
{
	RECT rcArea;
	int ht = HitTest( pt, 0xFFFF, &rcArea );
#if USE_DT2
	// ダブルクリックの間にESCを押した場合、反転バーが消えている
	CharHT cht( -1, &pt );
	if ( ht ){
		if ( rvItem == -1 ){
			hitno = ht & 0xff;
			MouseSelected(hitno);
			GetDC( );
			if ( CharHitTest( hitno, cht ) ){
				rvItem = cht.item;
				if ( cht.item >= SN_OBJECT ){
					SelectObject(hitno, &cht);
				}
				rvStart = rvEnd = cht.pos;
			}
//			SetSelIndex( hitno );
			ReleaseDC( );
		}
	} else {
		return;
	}
#endif

	if ( ht & HT_WORDITEM ){
#if !defined(SMALL) && USE_DT2
		GetDC( );
		bool fHit = CharHitTest( ht & 0xff, cht );
		ReleaseDC( );
		if (
#if !VIEWCLICKONLY
			HyperClickPopup &&
#endif
			rvItem != -1 ){
//			if ( rvItem >= SN_OBJECT || (rvStart != -1 && rvStart == rvEnd) )
			{
				if (al.LButtonDblClk(pt, key)){
					return;
				}
			}
		}
		if ( fHit && cht.item >= SN_OBJECT ){
#ifdef USE_JLINK
			if (DblClkObject(pt, ht & 0xff, cht.item - SN_OBJECT)){
				CancelCapture();
			}
#endif
		} else {
			CancelCapture();
			ReturnProc();
		}
#else	// SMALL
		CancelCapture();
		ReturnProc();
#endif
#if 0	// 要らないはず？ 2007.12.17
	} else if ( ht & HT_MEMORY || ht & HT_MODIFY ){
		CaptureAttr( ht, rcArea );
#endif
	}
}

void TMouseCapture::DDStart()
{
#if 0
	// D&D start!!
	KillTimer( ddStart );
	ddStart = 0;
	ReleaseCapture();
	int len;
	const tchar *sp = GetSelText( &len );
	if ( sp ){
		tnstr text( sp, len );
#if 0	//**
		fDragging = true;
//			BOOL fMove = FALSE;	// cannot move
		if ( TxDoDragDrop( text, fMove ) ){
			// success
		}
		fDragging = false;
#endif
	}
#endif
}

void TMouseCapture::TextCapture( POINT &pt )
{
#if USE_DT2
	CharHT cht( rvItem == SN_UNKNOWN ? -1 : rvItem, &pt );
	CharHitTest( hitno, cht );
	if ( rvItem == SN_UNKNOWN ){
		if ( cht.HitTest( htPivot ) ){
			DrawTextSelection(hitno, &cht);
			rvItem = cht.item;
			rvStart = cht.start;
			rvEnd = cht.end;
		}
	} else {
		if ( !cht.SetStartEnd( htPivot, rvStart, rvEnd ) ){
			// 範囲指定が無くなったため消去
			cht.item = rvItem;
			DrawTextSelection(hitno, &cht);
			rvItem = SN_UNKNOWN;
		} else if ( rvEnd != cht.pos ){
			rvEnd = cht.pos;
			DrawTextSelection(hitno, &cht);
		}
	}
#endif
}

#if USE_DT2
void TMouseCapture::SelectText( int end )
{
	if ( end == rvEnd )
		return;
	CharHT cht( rvItem );
	cht.start0 = rvStart;
	cht.end0 = rvEnd;
	cht.start = rvStart;
	cht.end = end;
	GetDC();
	CreateTextFonts();
	DrawTextSelection(hitno, &cht);
	DeleteTextFonts();
	ReleaseDC();
	rvEnd= end;
}
#endif	// USE_DT2

void TMouseCapture::ClsRegion( )
{
	//__assert(squ->hdc);
	if ( rvItem != -1 ){
		if ( IsValidHitno( ) && IsVisibleHitno(hitno) && ( rvItem >= SN_OBJECT || rvStart != rvEnd ) ){
			CharHT cht( rvItem );
			cht.start0 = rvStart;
			cht.end0 = rvEnd;
			cht.start = cht.end = 0;
			UnselectObject(hitno, &cht);
		}
		rvItem = rvStart = rvEnd = -1;
	}
	View.ClearObject();
}

const tchar *TMouseCapture::GetSelText(int *len)
{
	int start = rvStart;
	int end = rvEnd;
	if ( start > end ){
		start = rvEnd;
		end = rvStart;
	}
	const tchar *sp = View.GetText(hitno, rvItem);
	if ( !sp || end - start <= 0 )
		return NULL;
	*len = end - start;
	return sp+start;
}
void TMouseCapture::OffsetIndex(int offset)
{
	hitno -= offset;
}

CharHT *TMouseCapture::CreateCharHTDispStar()
{
	if ( rvItem == SN_WORD && rvStart != -1 && rvStart != rvEnd ){
		CharHT *cht = new CharHT( rvItem );
		cht->start = rvStart;
		cht->end = rvEnd;
		cht->start0 = rvStart;
		cht->end0 = rvEnd;
		return cht;
	} else {
		return NULL;
	}
}

// 通常PAINT時の選択状態のitemの描画処理
void TMouseCapture::DrawSelItem()
{
	__assert(rvItem!=-1);
	CharHT cht( rvItem );
	cht.start = rvStart;
	cht.end = rvEnd;
	cht.start0 = rvStart;
	cht.end0 = rvStart;
	DrawItem( hitno, &cht );
}

// 通常PAINT時の属性欄の反転描画処理
void TMouseCapture::DrawAttr()
{
	if ( fCapturing & (HT_MEMORY|HT_MODIFY) && !(fCapturing & HT_OUT) ){
		DrawCaptureAttr(rcCapAttr, NULL, 0);
	}
}

int TMouseCapture::BackupSel()
{
	int _rvItem = rvItem;
	if ( rvItem == SN_WORD )
		rvItem = -1;
	return _rvItem;
}

void TMouseCapture::RestoreSel(int _rvItem)
{
	rvItem = _rvItem;
}

void TMouseCapture::DrawTextSelection(int hitno, CharHT *cht)
{
	//View.DispOneLine( hitno, DF_DISP|DF_PINPOINT|DF_UNREDRAW, cht );
}
//TOOD: 2009.1.11
// DF_UNREDRAWがある場合と無い場合があったが、恐らく常にDF_UNREDRAWで問題ないはず
// 問題なければ常にDF_UNREDRAWをつけたままにする
void TMouseCapture::SelectObject(int hitno, CharHT *cht, bool unredraw)
{
	//View.DispOneLine( hitno, DF_DISP|DF_PINPOINT|DF_SELECTED|(unredraw?DF_UNREDRAW:0), cht );
}
void TMouseCapture::UnselectObject(int hitno, CharHT *cht)
{
	//View.DispOneLine( hitno, DF_DISP|DF_PINPOINT|DF_UNSELECTED|DF_UNREDRAW , cht );
}
void TMouseCapture::DrawItem(int hitno, CharHT *cht)
{
	//View.DispOneLine( hitno, DF_DISP|DF_PINPOINT|DF_UNREDRAW|DF_PAINT, cht );
}

void TMouseCapture::CaptureAttr( int ht, const RECT &rcArea )
{
#if 0
	int httype = ht & HT_MASK;
	fCapturing = httype;
	SetCapture( );
	inxCapture = ht & 0xff;

	DrawCaptureAttr(rcArea, &rcCapAttr, httype);
#endif
}

void TMouseCapture::MouseMicroScrollUp(int itemIndex)
{
#if 0
	GetDC();
	CreateTextFonts();
	if ( !View.MouseMicroScrollUp( GetCY(itemIndex) ) ){
		SelectText( 0 );
	}
	DeleteTextFonts();
	ReleaseDC();
#endif
}
void TMouseCapture::MouseMicroScrollDown(int itemIndex)
{
#if 0
	GetDC();
	CreateTextFonts();
	if ( !View.MouseMicroScrollDown( GetCY(itemIndex) ) ){
		SelectText( _tcslen(GetText(hitno, itemIndex)) );
	}
	DeleteTextFonts();
	ReleaseDC();
#endif
}

void TMouseCapture::alMouseMoveEnter()
{
#if 0
	if (
#if !VIEWCLICKONLY
		!HyperClickPopup ||
#endif
		!View.IsAutoLinkPopupOpened() ){
		SetTimer(TM_ALTIMERMOVE, 30);
	}
//	fAutoLinkCapture = ReverseAutoLink( &pt );
#endif
}

