#include "pdclass.h"
#pragma	hdrstop
#include "LangProc.h"
#include	"multidic.h"
#ifndef SMALL
#include	"utydic.h"
#endif
#include	"winsqu.h"
#include	"wpdcom.h"
#include "tid.h"
#include	"pdstr.h"
#include	"srchout.h"
#include	"squfont.h"
#include	"dictype.h"
#include "wpdcom.h"
#include "assexec.h"
#include "pdprof.h"
#include "pfs.h"

#include "WinSquUI.h"
#include "SquView.h"

#include "SrchMed.h"

#define	FSSUB	1		// 全文検索はsub searchを使用する

#define	USE_STOP	0

ex_mtimes(__srch);

//返り値 :
//	0 : normal started.
//	-1: cancel/error
int Squre::search( const tchar *str, SrchMode mode, diclist_t diclist, GenericRexp *_grexp, SrchOut *_srchout, int level1, int level2, int MemOnOff )
{
	// すでに検索中のときは中止する
	EndSearch( );
	ClearSubSearch();
	ss.SetSearchType(0);	//TODO: 5.7以前にも適用したほうがいい？

#if !USE_STOP
	SrchParam = GetSrchParam();
#endif

	// reset multithread
	SrchParam->multiThread = false;
	ForceUpdate = false;

	if (!_srchout && mode!=SRCH_ALL){
		if (prof.ReadInteger(PFS_COMMON, PFS_SEARCHMT, true)){
			SrchParam->multiThread = true;
			ForceUpdate = true;
		}
	}

#ifndef _UNICODE
	// modeの補正
	{
		if ( mode & SRCH_WORD && IsSingleByte( SN_WORD ) ){
			mode = (SrchMode)(mode | SRCH_WORD_S);
		}
		if ( mode & SRCH_JAPA && IsSingleByte( SN_JAPA ) ){
			mode = (SrchMode)(mode | SRCH_JAPA_S);
		}
		if ( mode & SRCH_EXP && IsSingleByte( SN_EXP+1 ) ){
			mode = (SrchMode)(mode | SRCH_EXP_S);
		}
	}
#endif
	int search_type = SST_SEARCH;
#if FSSUB
	if (ss.IsSubSearch()){
		dbw("sub search");
		search_type |= SST_SUBSEARCH | SST_PHASE2;
	}
#endif
	ss.SetupSearch( search_type, str, mode, diclist, level1, level2, MemOnOff );
#ifndef SMALL
	if (_grexp){
		_grexp->SetPattern(str);
	}
	ss.SetGRexp( _grexp );
#endif

#ifndef SMALL
	if ( ss.IsRexp() ){
		if ( ss.CanUseRexp() ){
			if ( !ss.Compile( ) ){
				// コンパイルエラー
				//
				int error = ss.GetRexpErrorCode( );
				ss.SetGRexp( NULL );
				if ( error != GRE_NOREGULAR ){
					MsgBox( _LT( RGS_FIRST + error ), _LT(CAP_REGULAREXPRESSION), 
						MB_OK | MB_ICONSTOP );
					return -1;
				}
				// 通常の検索を使用
			}
		} else {
			MsgBox( _LT(RGS_CANNOTUSE), _LT(CAP_REGULAREXPRESSION), MB_OK | MB_ICONSTOP );
			return -1;
		}
	}

#ifdef _Windows
	if ( _srchout ){
		if ( _srchout->Open(GetView()) ){
			return -1;
		}
		srchout = _srchout;
		srchout->outflag |= ( IsVisible( SN_JAPA ) ? OF_EXP : 0 );
		if ( (srchout->format==DT_PDICTEXT) || (srchout->format==DT_CSV) || (srchout->format==DT_TSV)){
			srchout->outflag = OF_WORD | OF_JAPA
							| ( IsVisible( SN_JAPA ) ? OF_EXP : 0 )
							| ( IsVisible( SN_LEVEL ) ? OF_LEVEL : 0 )
#ifdef NEWDIC2
							| ( IsVisible( SN_PRON ) ? OF_PRON : 0 )
#endif
			;
		}
	}
#endif	// _Windows
#endif	// ndef SMALL

	ss.ResetCount();
	ss.StartSearch( false );

	SetMessage(MSG_SEARCHING);
	
	GetDC( );
	Clear();
	ReleaseDC( );

#if FSSUB
	if (SrchParam->multiThread){
		// ここでsub_srchparamがsetされる
		StartSubSearchForNormalSearch(str, mode, diclist, _grexp ? _grexp->Duplicate() : NULL, level1, level2, MemOnOff);
	}
#endif

#ifdef WINCE
#if 0	// メニューの残骸を消すために実行したいが、
		// AppMessageProc() でheap領域か何かが破壊されるようだ？？
		// 原因不明
	AppMessageProc();
	UpdateWindow();
#endif
#endif

#if FSSUB
	Dic.setAS( str, mode, ss.grexp, SrchParam->multiThread ? sub_srchparam : SrchParam );
#else
	Dic.setAS( str, mode, ss.grexp, SrchParam );
#endif

	_mtimes_start(__srch);

#ifndef SMALL
	// 検索結果をファイルへ出力する場合はモーダルダイアログボックスを表示し、
	// しかも、バックグラウンド検索は行なわない
	if ( srchout ){
		ISearchingDlg *dlg = NULL;
		if (UIMain){
			dlg = UIMain->OpenSearchingDlg(GetView());
			UIMain->EnableMainWindow(false);
		}
		while ( 1 ){
			SearchProc( );
			if ( IsSearching() ){
				if ( dlg->IsStopped()
					|| ( srchout->maxnum != -1 && ss.GetFindCount() > srchout->maxnum )
					|| srchout->GetErrorCode()
				){
					StopSearch( );
					break;
				}
				if ( dlg->IsSpeedUp() ){
					if ( bAllowDisp ){
						GetDC();
						Clear();
						ReleaseDC();
					}
					bAllowDisp = false;
				}
			} else {
				break;
			}
			if ( !(srchout->output & OD_DEBUG) && IsOver() ){
				GetDC();
				Clear();
				ReleaseDC( );
			}
			ss.PauseSearch(NULL);	// DlgMessageProc中にIDLE処理に入り、
									// 戻りにくくなり、こちらでの処理が実行されないため
									// 弊害が起こるかもしれない。。。
			if (dlg)
				dlg->DlgMessageProc();
			ss.ContinueSearch();
		}

		if (dlg){
			dlg->Close( );
			delete dlg;
			UIMain->EnableMainWindow(true);
		}
		srchout->Close();
		if ( srchout->output == OD_BROWSER ){
			AssociateExecute( HWindow, srchout->filename );
			SetDeleteFile(srchout->filename);
		}
		Invalidate();
#if 0
		if ( !srchout->filename[0] ){
			// クリップボードへの出力なら
			if (UIMain) UIMain->EnableAutoSrch(true);
		}
#endif
		srchout = NULL;
	}
#endif	// ndef SMALL

	return 0;
}
// 検索中で表示されていないものは表示される
void Squre::StopSearch( )
{
	CancelExtSearch();
	ss.StopSearch( );
	sub_ss.StopSearch();
	SetMessage();
	bAllowDisp = true;
	SetVPos( IndexOffset, true );
}

// 検索中で表示されていないものはclearされる
void Squre::EndSearch( )
{
	if ( IsSearching( ) && !bAllowDisp ){
		clear( );
	}
	StopSearch( );
}

//
void Squre::NeedReset( )
{
	ss.NeedReset( get_num() ? ( ss.fBackWard ? GetWordAbs( 0 ) : GetWordAbs( get_num() - 1 ) ) : StrNull );
}

void Squre::PauseSearch( )
{
	ss.PauseSearch( get_num() ? ( ss.fBackWard ? GetWordAbs( 0 ) : GetWordAbs( get_num() - 1 ) ) : StrNull );
	SetVPos( IndexOffset, TRUE );
}
#ifdef _Windows
#ifndef SMALL
//---------------------------------------------------------------------------
//
// 履歴検索処理
//
//---------------------------------------------------------------------------
#include "Dic.h"
#include "histfile.h"
#include "srchout.h"
#include "ProgressDlg.h"
#include "BookmarkMan.h"

// return:
//	>=0 : number of found words.
//	<0  : error
int Squre::SearchHistory( TSearchHistoryParam &shp )
{
#if USE_HISTORY
	EndSearch( );

	SrchOut *srchout = shp.srchout;

	if ( srchout->Open(GetView()) ){
		return -1;
	}

	TWordHistory WordHist;
	bool alphasort = !(shp.order == 2 || shp.order == 3);
	bool reverse = (shp.order == 2);
	if (shp.bookmark){
		if (!alphasort)
			reverse = !reverse;		// 履歴で登録順→単語帳では登録逆順のため
		LoadBookmark(WordHist, alphasort, reverse);
	} else {
		if ( !LoadHistory( WordHist, alphasort, reverse) ){
			// no file
			MsgBox(_LT(MSG_CANNOTOPEN_HISTORYFILE), _LT(CAP_SEARCH_HISTORY), MB_OK);
			return 0;
		}
	}
	if ( WordHist.get_num() == 0 ){
		// no data
		return 0;
	}
	if ( shp.order == 1 )
		WordHist.SortByNumWord();

	// 出力処理
	srchout->outflag |= ( IsVisible( SN_JAPA ) ? OF_EXP : 0 );
	if ( (srchout->format==DT_PDICTEXT) || (srchout->format==DT_CSV) || (srchout->format==DT_TSV)){
		srchout->outflag = OF_WORD | OF_JAPA
						| ( IsVisible( SN_JAPA ) ? OF_EXP : 0 )
						| ( IsVisible( SN_LEVEL ) ? OF_LEVEL : 0 )
#ifdef NEWDIC2
						| ( IsVisible( SN_PRON ) ? OF_PRON : 0 )
#endif
		;
	}
	TProgressDialog *dlg = new TProgressDialog(GetView());
	dlg->Show();
	if (UIMain) UIMain->SetEnable(false);

	int count = 0;
	Japa japa;
	for ( int i=0;i<WordHist.get_num();i++ ){
		tnstr word = Dic.create_composit_word(WordHist[i].Word);
		if ( Dic.Read( word, &japa ) == 1 ){
			if ( shp.levelflag ){
				if ( japa.GetLevel() < shp.level_min )
					continue;
				if ( japa.GetLevel() > shp.level_max )
					continue;
			}
			if ( shp.memoryflag ){
				if ( japa.GetAttr() & WA_MEMORY ){
					if ( shp.memory_off )
						continue;
				} else {
					if ( shp.memory_on )
						continue;
				}
			}
#ifdef USE_JLINK
			// EPWINGは削除
			for ( int j=0;j<japa.jlinks.get_num();j++ ){
				if ( japa.jlinks[j].GetType() == JL_EPWING ){
					japa.jlinks.del( j );
					j--;
					continue;
				}
			}
#endif
			if ( !srchout->Output( word, japa, WordHist[i].Num ) ){
				break;
			}
			count++;
			if ( count % 10 == 0 ){
				dlg->UpdateCount( count );
				dlg->UpdatePercent( i * 100 / WordHist.get_num() );
			}
			DlgMessageProc();
			if ( dlg->IsStopped() ){
				// stop trans level
				break;
			}
		}
	}
	if (UIMain) UIMain->SetEnable(true);
	delete dlg;
	srchout->Close();
#endif	// USE_HISTORY

	return count;
}
#endif	// ndef SMALL
#endif	// _Windows

void Squre::SearchFast(const tchar *str, SrchMode mode, diclist_t diclist)
{
	if ((int)mode==0){
		mode = (SrchMode)(SRCH_FAST|SRCH_WORD|SRCH_JAPA|SRCH_EXP);
	}
	if (dl_is_empty(diclist)){
		dl_all(diclist);
	}
	GenericRexp *grexp = NULL;
	search( str, mode, diclist, grexp );
}

