#include "tnlib.h"
#pragma hdrstop
#include "MouseCapCommon.h"
#include "dicdef.h"
#include "jlink.h"

bool TMouseViewIFCommon::SetTimer(int id, int timer)
{
	//return ::SetTimer(GetHandle(), id, timer, NULL);
	return false;
}
void TMouseViewIFCommon::KillTimer(int id)
{
	//::KillTimer(GetHandle(), id);
}

void TMouseViewIFCommon::SetCapture()
{
	//::SetCapture(GetHandle());
}

//
// TAutoLinkCommon class
//
TAutoLinkCommon::TAutoLinkCommon(TMouseViewIFCommon &view)
	:View(view)
{
	CursorState = 0;
}

void TAutoLinkCommon::MouseMove(POINT pt, UINT key, int ht)
{
#if 0
	int hl_index = View.HitTestHyperLink(ht&0xff, pt);
	if (hl_index>=0){
		if (!CursorState){
			CursorState=1;
			View.SetCursor(IDC_HAND);
		}
	} else {
		if (CursorState){
			CursorState = 0;
			View.SetCursor(0);
		}
	}
#endif
}

//
// TMouseCaptureCommon class
//
TMouseCaptureCommon::TMouseCaptureCommon(TMouseViewIFCommon &view, TAutoLinkCommon &_al)
	:super(view)
	,View(view)
	,al(_al)
#if USE_DT2
	,htPivot( -1 )
#endif
{
#if USE_DT2
	rvStart = rvEnd = rvItem = -1;
#endif
	ddStart = 0;

	mousetimer = 0;

	LastMoveIndex = -1;
	LastMoveObjIndex = -1;
}

void TMouseCaptureCommon::LButtonDown(POINT pt, UINT key)
{
	RECT rcArea;
	int ht = HitTest( pt, 0xFFFF, &rcArea );
	int inx = ht & 0xff;
	int httype = ht & ~0xff;
	switch ( httype ){
		case HT_WORDITEM:
			{
			fCapturing = ht;
			SetCapture( );
			GetDC( );
			MouseSelected(inx);
			do {
#if USE_DT2
				if ( key & MK_CONTROL
					&& rvItem != -1
					&& rvItem < SN_OBJECT
					&& rvStart != rvEnd
					){
					// まだ不完全
					TextCapture( pt );
					break;
				}
				CharHT cht( -1, &pt );
				bool fHit = CharHitTest( inx, cht );
				htPivot = cht;
#endif
#ifdef USE_JLINK
				if (cht.item >= SN_OBJECT){
					LButtonDownObject(key, pt, inx, cht.item - SN_OBJECT, cht);
				} else
#endif
				{
					// ここにD&Dの追加(OLE2)
					// 範囲選択内のLButtonDownであれば、タイマーの監視またはMouseMoveを監視し、
					// 一定時間経過、またはマウス移動の場合、D&Dを開始する
					// 選択範囲内でない場合 - 今まで通り、ClsRegion()
					// タイマー中にLButtonUp++ - 何もしない
					// タイマー中にMouseMove - D&D開始
					// タイマー中にESC - LButtonUpと同じ扱い
#ifdef OLE2
#if 0	// Text D&Dは保留 --- テキストオブジェクトをどうやって作ったらいいのやら・・・・
					if ( rvItem == cht.item && rvStart != rvEnd && cht.pos >= rvStart && cht.pos <= rvEnd ){
						// 選択範囲内
						ddStart = SetTimer( IDT_DD, 300, NULL );
						// HDC,TextFontsはReleaseしない！！
						break;
					} else
#endif
#endif
					{
						ClsRegion( );	// 前回の範囲選択解除
					}
				}
#if USE_DT2
				if ( !fHit ){
					// rvItem = SN_UNKNOWN;	// capture解除に変更した 2009.1.11
					fCapturing = 0;
					ReleaseCapture();
					rvItem = -1;
					break;
				} else {
					rvItem = cht.item;
				}
#endif
				hitno = inx;
#if USE_DT2
				rvStart = rvEnd = cht.pos;
#endif
				SetSelIndex( inx );
			} while (0);
			ReleaseDC();
			}
			return;
		case HT_MEMORY:
		case HT_MODIFY:
			CaptureAttr( ht, rcArea );
			return;
	}
}

void TMouseCaptureCommon::LButtonUp(POINT pt, UINT key)
{
#if 0
	if ( !fCapturing )
		return;

	ReleaseCapture();
#ifdef OLE2
	if ( ddStart ){
		// D&Dで何もしない
		KillTimer( ddStart );
		ddStart = 0;
		ClsRegion();
		fCapturing = HT_NONE;
		return;
	}
#endif
	// Get the released hit test info.
	int r_ht = HitTest(pt);
	int r_inx = r_ht & 0xff;
	//int r_httype = r_ht & ~0xff;

	// Released hit test info.
	if ( fCapturing & ( HT_MEMORY | HT_MODIFY ) ){
		if ( !(fCapturing & HT_OUT) ){
			uchar bit = (uchar)( ( (uint)fCapturing == HT_MEMORY ) ? WA_MEMORY : WA_JEDIT );
			View.MouseChangeAttr(inxCapture, bit, rcCapAttr);
		}
		fCapturing = HT_NONE;
	}
	if ( fCapturing & HT_WORDITEM ){
		fCapturing = HT_NONE;
#if USE_DT2
		if ( rvItem != -1 ){
			GetDC();
			if ( rvItem >= SN_OBJECT ){
				// Released on the Object.
				// Not support the auto link for now.
				CharHT r_cht( -1, &pt );
				CharHitTest( r_inx, r_cht );
				LButtonUpObject(pt, r_inx, r_cht.item-SN_OBJECT, key);
			} else
			if ( rvStart != -1 && rvStart == rvEnd ){
				// Not object.
				CharHT cht( -1, &pt );
				if (CharHitTest( r_inx, cht )){
					if (cht.item==rvItem){
						// hit another item
						switch (cht.item){
							case SN_LEVEL:
								MousePopupMenu(r_inx, pt);
								break;
						}
					}
				}
				// HyperLinkチェック
				alLButtonUp(pt, key);
				// オブジェクト以外の選択で、範囲選択になっていない場合は解除
				rvItem = rvStart = rvEnd = -1;
			}
			ReleaseDC();
		}
#endif	// USE_DT2
	}
	StopMouseTimer();
#endif
}

void TMouseCaptureCommon::MouseMove(POINT &pt, UINT key)
{
#if 0
#if USE_DT2
	if (!fCapturing){
		// 移動してもすぐに反応させないために
		alMouseMoveEnter();
	}
#endif

	// Get the moving hit test info.
	int ht = HitTest( pt );
	int ht_inx = ht & 0xff;
	//int ht_type = ht & ~0xff;
	CharHT cht( -1, &pt );
	if (ht){
		GetDC();
		CharHitTest( ht_inx, cht );
		ReleaseDC();
	}

	switch ( fCapturing & HT_MASK ){
		case HT_WORDITEM:
			switch ( ht & HT_MASK ){
				// 自動スクロール
				case HT_UPPER:
				case HT_LOWER:
					if ( mousetimer )	// すでにスクロール処理を行っている場合は無視
						break;
#if USE_DT2
					// テキスト以外はスクロールの対象外
					if (rvItem>=SN_OBJECT){
						goto jobjmove;
					} else
					if ( rvItem!=SN_WORD && rvItem!=SN_PRON && rvItem!=SN_JAPA && rvItem!=SN_EXP1 ){
						break;
					}
#endif

					mousetimer = ht & HT_MASK;
#if USE_DT2
					if ( mousetimer == HT_UPPER ){
						MouseMicroScrollUp(rvItem);
					} else {
						MouseMicroScrollDown(rvItem);
					}
#endif
					if ( !mousetimer )
						break;
					DWORD dwRepeat;
					SystemParametersInfo( SPI_GETKEYBOARDSPEED, 0, &dwRepeat, NULL );
					dwRepeat /= 16;
					if ( !SetTimer(TM_MOUSETIMER, dwRepeat) ){
						mousetimer = 0;
						break;
					}
					break;
				case HT_WORDITEM:
					if ( mousetimer ){
						StopMouseTimer();
#if 0	// これを有効にすると、画面に表示しきれる単語の最下行が範囲選択できなくなってしまう・・・
					} else {
						if ( View.IsVisibleHitno(ht & 0xff) ){
							mousetimer = HT_LOWER;
							goto jm1;
						}
#endif
					}
#if USE_DT2
					if ( rvItem != -1 ){
#ifdef USE_JLINK
						if ( rvItem >= SN_OBJECT ){
							// オブジェクトのキャプチャー
							goto jobjmove;
						} else
#endif
						{
							if (rvStart!=rvEnd){
								// 範囲選択中はpopup&link無効
								View.CloseAllPopup();
							}

							// テキストのキャプチャー
							GetDC();
							TextCapture( pt );
							ReleaseDC();
						}
						break;
					}
#endif
					break;
				default:
					StopMouseTimer();
					break;
			}
			break;
		case HT_MEMORY:
		case HT_MODIFY:
			GetDC();
			if ( (ht & (HT_MEMORY|HT_MODIFY)) && (ht & 0xff) == inxCapture ){
				if ( fCapturing & HT_OUT ){
					fCapturing &= ~HT_OUT;
					DrawCaptureAttr(rcCapAttr, NULL, 0);
				}
			} else {
				if ( !(fCapturing & HT_OUT) ){
					fCapturing |= HT_OUT;
					DrawCaptureAttr(rcCapAttr, NULL, 0);
				}
			}
			ReleaseDC();
			break;
		case 0:	// no capturing
			alMouseMove(pt, key, ht);
jobjmove:
		{
			POINT ptObj = pt;
			int objIndex = -1;
			if (ht&&cht.item>=SN_OBJECT){
				objIndex = cht.item-SN_OBJECT;
				ptObj.x -= cht.cpos.x;
				ptObj.y -= cht.cpos.y;
			}
			MouseMoveObject(ptObj, ht_inx, objIndex);
		}
		break;
	}
#endif
}

void TMouseCaptureCommon::EvTimer(int id)
{
#if 0
	switch (id){
		case TM_ALTIMER:
		case TM_ALTIMERCLICK:
			AlTimer(id);
			break;
	}
#ifdef OLE2
	if ( id == ddStart ){
		DDStart();
		return;
	} else
#endif
	if (id==mousetimer){
#if USE_DT2
		switch ( mousetimer ){
			case HT_UPPER:
				MouseMicroScrollUp(rvItem);
				break;
			case HT_LOWER:
				MouseMicroScrollDown(rvItem);
				break;
		}
#endif
	}
#endif
}

void TMouseCaptureCommon::AlTimer(int timerId)
{
	ReleaseCaptureCommon();
}

void TMouseCaptureCommon::Deactivate()
{
	ReleaseCaptureCommon();
}

void TMouseCaptureCommon::ReleaseCaptureCommon()
{
#if 0
	if (fCapturing){
		ReleaseCapture();
		fCapturing = HT_NONE;
	}
#endif
}

void TMouseCaptureCommon::CancelCapture()
{
	ReleaseCaptureCommon();
	View.MouseCancelCapture();
}

void TMouseCaptureCommon::StopMouseTimer()
{
	if ( mousetimer ){
		KillTimer(TM_MOUSETIMER);
		mousetimer = 0;
	}
}

// index and objIndex are hit point at released.
// pt : 相対座標 from the top-left of the object.
void TMouseCaptureCommon::MouseMoveObject(POINT &pt, int index, int objIndex)
{
}
void TMouseCaptureCommon::LButtonDownObject(UINT key, POINT &pt, int index, int objIndex, CharHT &cht)
{
}
// index and objIndex are the hit test info at the released point.
void TMouseCaptureCommon::LButtonUpObject(POINT &pt, int index, int objIndex, UINT key)
{
}
bool TMouseCaptureCommon::DblClkObject(POINT &pt, int index, int objIndex)
{
	return false;
}

void TMouseCaptureCommon::RButtonDownObject(POINT &pt, int index, int objIndex, UINT key, CharHT &cht)
{
}
void TMouseCaptureCommon::RButtonUpObject(POINT &pt, int index, int objIndex)
{
}


