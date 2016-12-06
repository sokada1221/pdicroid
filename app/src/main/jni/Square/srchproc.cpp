#include	"tnlib.h"
#include	"multidic.h"
#pragma	hdrstop
#include	"id.h"
#include	"winsqu.h"
#include	"winmsg.h"
#include "srchcom.h"
#include "autolink.h"
#include "SrchMed.h"
#include "BookmarkMan.h"

_mtimes( __srch, "search");

#define	N_ALLOWDISP		15		// この件数を超えたら表示

// 検索処理ルーチン（バックグラウンド版）
// 0 : 何もしなかった
// 1 : １つ以上見つかった
int Squre::SearchProc( bool bOnlyOne )
{
	if ( !ss.IsSearching() || ss.IsPaused() ){
		return 0;
	}

	if (!SrchMed)
		OpenSrchMed();
	
	if ( ss.GetSearchState() & SS_NEEDRESET ){
		ResetSearch( );
	}

	int r = 0;
	bool bEnd = false;
	tnstr *fword = NULL;
	Japa *fjapa = NULL;
	int level;
	while (get_num() != MAX_NDISP || !ss.IsFindCompleted() || ForceUpdate){
		// TSearchMediator::AddSubWordX()にInsertPool()処理があるため、
		// このルーチン以外で終了となる場合があるため、
		// 先頭に終了判定を設ける
		int c;
		if ( !fword ){
			fword = new tnstr;
			fjapa = new Japa;
		}
		const tchar *w = SrchMed->GetWord();
		if (w==(const tchar*)AS_CONTINUE){
			c = AS_CONTINUE;
		} else
		if (w!=(const tchar*)AS_NORMAL_SEARCH){
			if (w==(const tchar *)AS_END){
				// end of the search
				if (SrchMed->SetupNextSearch(ss.fBackWard, true)){
					continue;
				} else {
					c = AS_END;	// 本当に終了
				}
			} else {
				SrchMed->NeedJump = true;
				__EnableDDNBegin(CanDispDicName());
				int r = Dic.Read(w, fjapa, ss.uDicList);
				__EnableDDNEnd();
				if (r){
					// found
					BM_JSetMark(find_cword_pos(w), *fjapa);
					*fword = w;
					c = AS_FOUND+1;
				} else {
					// deleted?
					continue;
				}
			}
		} else {
			// normal search
			__EnableDDNBegin(CanDispDicName());
			if ( ss.fBackWard ){
				c = Dic.prevAS( *fword, fjapa, ss.uDicList, SrchParam );
			} else {
				c = Dic.nextAS( *fword, fjapa, ss.uDicList, SrchParam );
			}
			__EnableDDNEnd();
		}
		switch ( c ){
			case AS_CONTINUE:
				if (bOnlyOne)
					continue;
#if 0
				if (bAllowDisp && LastVRange!=get_num()){
					SetVRangeSearching();
				}
#endif
				break;
			case AS_ERROR:	// エラー
				if (Dic.GetErrorCode()==DICERR_GEN_CHANGED){
					const tchar *word = ss.fBackWard ? SrchMed->GetTopSearchWord() : SrchMed->GetLastSearchWord();
					if (word){
						Dic.jumpAS( word, ss.fBackWard, false, SrchParam );
					} else {
						// 最初からやり直し
						// tricky!!!
						int dicno = Dic.GetErrorDicNo();
						if (dicno>=0){	// 念のため
							Dic[dicno].SetAllSearch( ss.word, ss.mode, ss.grexp, (*SrchParam)[dicno] );	// tricky code
						}
					}
					continue;
				}
				ss.StopSearch( );
				SetMessage( );
				bEnd = true;
				break;
			case AS_END:	// 全て終了
				// end of the search
				if (SrchMed->SetupNextSearch(ss.fBackWard, true)){
					// continue
					break;
				}
				ss.StopSearch( );
				ss.SetNothing( ss.fBackWard );
				bEnd = true;
				break;
			case AS_FOUND:
				// レベル制限
				BM_JSetMark(find_cword_pos(*fword), *fjapa);
				level = fjapa->GetLevel();
				if ( level < ss.level1 || level > ss.level2 ){
					continue;
				}
				// 暗記必須制限
				if ( ss.MemOnOff ){
					if ( ss.MemOnOff == 1 ){
						// On制限
						if ( !(fjapa->GetAttr() & WA_MEMORY) )
							continue;
					} else {
						// Off制限
						if ( fjapa->GetAttr() & WA_MEMORY )
							continue;
					}
				}

				//
				// fall through.
				//
			case AS_FOUND+1:	// found by sub search.
				//DBW("%ws", fword->c_str());
				//DBW("%s", __cstr(fword->c_str()).utf8());

				if (ss.IsAndSearch()){
					SearchCommonStruct scs( &ss, Dic.GetLangProc(), true );
					if ( scs.Setup(false) == 1 ){
						if (!scs.Search(fjapa->japa)){
							//continue;	// not found
							break;	// not found
						}
					}
				}

				ss.IncFindCount();
				r = 1;
				if ( ss.fBackWard ){
					int index = SrchParam->multiThread ? SrchMed->FindInsertPoint(*fword) : 0;
					SrchMed->InsertPoolFixed(index, fword, fjapa, SrchParam->GetLastFindDicNo(), true);
				} else {
					if ( !bAllowDisp ){
						// N_ALLOWDISP件超えたら表示させる
						if ( get_num() > N_ALLOWDISP ){
							bAllowDisp = true;
							cury = GetStarLoc( SearchString, 0, Dic.GetOrder() );
							AllowDisp( bOnlyOne );
							Invalidate();
						}
					}
					if (SrchParam->multiThread){
						int index = SrchMed->FindInsertPoint(*fword);
						bool add_mode = (index==pool.get_num());
						// RewindWord()について：
						//  add_modeの場合、poolへの追加を行わないといけない。(ForceUpdate==trueだと捨てられる場合がある)
						//	もし追加を行わないと、SubWordsIndexはpoolの外のindexに更新されたままとなり、
						//	次に取得すべきindex（つまりSubWordsIndex)が不正な値となってしまう。
						//	したがって、ForceUpdateで捨ててしまう場合は、SubWordsIndexを戻す必要があるがここでは判断ができない。。。
						//	
						//Note:
						// index==pool.get_num() && pool.get_num()==MAX_NDISPだと先頭が捨てられてしまうため
						// ForceUpdate時は追加しない
						const bool will_top_drop = add_mode && pool.get_num()==MAX_NDISP;
						if (!ForceUpdate || !will_top_drop){
							/*int ret = */SrchMed->InsertPoolFixed(index, fword, fjapa, SrchParam->GetLastFindDicNo(), false);
							//DBW("index=%d/%d add=%d ret=%d : %ws", index, pool.get_num(), add_mode, ret, find_cword_pos(fword->c_str()));
						} else {
							SrchMed->RewindWord();
						}

						if (ForceUpdate){
							if (IndexOffset>0 || index<LastIndex){
								//TODO: insert2()で!ForceUpdateによるIndexOffsetの更新をなくした場合この処理は不要？
								IndexOffset = 0;
								Invalidate();
							} else {
								// insert追加されたとき、UpdateProc()を呼ぶ必要がある
								// InsertPoolFixed()でcorrect=trueにすればよいが、IndexOffset>0のとき再々描画になってしまうのを避けるため
								if (!add_mode){
									UpdateProc(index);
								}
							}
						}

					} else {
						if (!SrchMed->AddPool(fword, fjapa, SrchParam->GetLastFindDicNo())){
							bEnd = true;
						}
					}
				}
				fword = NULL;
				fjapa = NULL;
				if ( bEnd || (get_num() == MAX_NDISP && ss.IsFindCompleted() && !ForceUpdate) ){
					// 検索終了処理
					ss.StopSearch( );
					ss.SetRemain( ss.fBackWard );
					bEnd = true;
					break;
				}
				break;
#ifdef _DEBUG
			default:
				__assert__;
				break;
#endif
		}
		break;
	}
	if ( fword ){
		delete fword;
		delete fjapa;
	}
	if ( bEnd ){
		_mtimes_stop(__srch);
		SetVRange2( true );
		SetVPos( IndexOffset );
		int pasttime = ss.GetPastTime();
		if ( get_num() || ( srchout && ss.IsFound() ) ){
			SetMessage(GetSrchMsgId(),
				pasttime,
				GetFoundCount());
		} else {
			SetMessage(MSG_NOTFIND3, pasttime);
		}
		if ( IsIncSearch() /* && ( ss.state & SS_NEEDREDISP ) */ ){
			if ( !bAllowDisp )
				Invalidate();

			bAllowDisp = true;
			AllowDisp( bOnlyOne );
#if INCSRCHPLUS
			StartISPTimer();
#endif
		}
	}
	return r;
}

#define	MAX_SUB_NDISP	(MAX_NDISP*8)	// 最大検索数

// sub search routine
// 0 : 何もしなかった
// 1 : １つ以上見つかった
int Squre::SubSearchProc( )
{
	if ( !sub_ss.IsSearching() || sub_ss.IsPaused() ){
		return 0;
	}

	if ( sub_ss.GetSearchState() & SS_NEEDRESET ){
		// ここにはこないはず
		// ResetSearch( );
		__assert(false);
	}

#if 0
	if (SrchMed->NeedJump){
		const tchar *jword = SrchMed->GetLastSearchWord();
		if (jword){
			Dic.JumpAS(jword, false);
		}
		SrchMed->NeedJump = false;
	}
#endif

	int r = 0;
	tnstr *fword = new tnstr;
	Japa _fjapa;
	Japa *fjapa = sub_ss.IsAndSearch() ? &_fjapa : NULL;
	bool end = false;
	int index;
	while ( 1 ){
		int c;
		__EnableDDNBegin(CanDispDicName());
		if ( sub_ss.fBackWard ){
			c = Dic.prevAS( *fword, fjapa, sub_ss.uDicList, sub_srchparam );
		} else {
			c = Dic.nextAS( *fword, fjapa, sub_ss.uDicList, sub_srchparam );
		}
		__EnableDDNEnd();
		switch ( c ){
			case AS_CONTINUE:		// アイドル
#if 0
				if (ss.IsSubSearch() && bAllowDisp && LastVRange!=get_num()){
					SetVRangeSearching();
				}
#endif
				break;
			case AS_ERROR:	// エラー
				sub_ss.SetNothing( sub_ss.fBackWard );
				end = true;
				break;
			case AS_END:	// 全て終了
				sub_ss.SetNothing( sub_ss.fBackWard );
				if (ss.IsSubSearch() && sub_ss.GetSearchPhase()==SST_PHASE2){
					// must be in foreground search, because the ChangeSubSearchToMain() uses the map when popup search invoked.
					sub_words_map.clear();	// clear the duplicate check to release the memory.
				}
#if 0 && defined(_DEBUG)
				switch (sub_ss.GetSearchPhase()){
					case SST_PHASE1:
						DBW("Phase1:%d", sub_words->get_num());
						break;
					case SST_PHASE2:
						DBW("Phase2:%d", sub_words->get_num());
						break;
				}
#endif
				end = true;
				break;
			case AS_FOUND:
				//DBW("%s:%ws", sub_ss.GetSearchType()&SST_PHASE1?"f":"s",fword->c_str());
				if (sub_ss.IsAndSearch()){
					SearchCommonStruct scs( &sub_ss, Dic.GetLangProc(), true );
					if ( scs.Setup(false) == 1 ){
						if (!scs.Search(fjapa->japa)){
							break;	// not found
						}
					}
				}

				// 2014.10.17 下の+1からここへ移動
				if (sub_words_map.count(fword->c_str())){
					// duplicated.
					break;
				}

				if (sub_words->get_num()>=MAX_SUBSEARCH){
					sub_ss.SetNothing( sub_ss.fBackWard );	// over the limitation.
					end = true;
					break;
				}
				sub_ss.IncFindCount();
				r = 1;

				// +1 (上記参照)
				
				if (sub_ss.GetSearchType()&SST_PHASE1){
					index = SrchMed->AddSubWord1(fword);
					// +2 (下記参照)
				} else {
					//Note:
					// 全文検索では、levelによるソートは行わない
					const bool no_level = sub_srchparam->multiThread;
					index = SrchMed->AddSubWord2(fword, no_level);
				}

				// 2014.10.17 上記+2からここへ移動(PHASE2でも重複チェックが必要なはず)
				sub_words_map[fword->c_str()] = true;	//TODO: wordのpatternで判断するように

				//DBW("%d:sub_words:%ws", sub_ss.GetSearchType()&SST_FIRSTSEARCH?1:0, fword->c_str());
				if (ss.IsSubSearch()){
					// sub search is showing in the main.
					//DBW("IndexOffset=%d LastIndex=%d", IndexOffset, LastIndex);
					if (index<=IndexOffset){
						// the index of the added item is top or before the visible items.
						//Note: insert()内で処理した方が良いかも？
					}
					if (SrchMed->IsInPool(index) && SrchMed->IsVisible(index, IndexOffset, LastIndex)){
						//DBW("IsVisible");
						Invalidate();
					}
#if 0
					if (bAllowDisp && LastVRange!=get_num()){
						SetVRangeSearching();
					}
#endif
				} else {
					// my search is background.
					if (sub_words->get_num() == MAX_SUB_NDISP){
						sub_ss.SetRemain( sub_ss.fBackWard );
						end = true;
					}
				}
				fword = NULL;	// discard
				break;
			default:
				__assert__;
				break;
		}
		break;
	}
	if ( fword ){
		delete fword;
	}
	if (end){
		//DBW("End sub search");
		// 検索終了処理
		sub_ss.StopSearch( );
	}
	return r;
}

void Squre::AllowDisp( bool bOnlyOne )
{
	if ( get_num() && !bOnlyOne ){	// 1998.3.8 !bOnlyOneを追加(wsinc.cppの150行目付近の検索で、ここでのClear()にされるのを防ぐため
#if !NEWINCSRCH
		int i1, i2;
		GetKeepLoc( SearchString, _tcslen( SearchString ), i1, i2 );
		if ( i1 == i2 && i1 >= get_num() )
		{
			// 検索する単語が検索結果の中に含まれない場合は、Not found.
			NotFoundCount = _tcslen( SearchString );
			Clear();
			SetMessage( GetStateMessage( MSG_NOTFIND3 ) );
			return;
		}
#endif
		if ( cury == -1 )
			cury = 0;
	}
	if ( IndexOffset + cury >= get_num() ){
		IndexOffset = 0;
	}
}

bool Squre::IdleProc( )
{
	bool ret = true;	// done

	// Sub Search //
	if (sub_ss.IsSearching() && !sub_ss.IsPaused() ){
		SubSearchProc();
		if (!sub_ss.IsSearching()){
			if (sub_ss.GetSearchType()&SST_PHASE1){
				// End of the first phase search.
				// second phase search.
				_StartSubSearch(sub_srchword, false);	// 2009.7.3 NULL→sub_ss.word 2009.7.7 sub_ss.word→sub_srchword
				if (!sub_ss.IsSearching()){	// caused by error or something..
					__assert_debug;
					SetMessage();
				}
			} else
			if (sub_ss.GetSearchType()&SST_PHASE2){
				// End of all sub search.
				if (ss.IsSearching()){
					if (ss.IsSubSearch()){
						ss.StopSearch();
					}
				}
				if (!ss.IsSearching()){
					int pasttime = sub_ss.GetPastTime();
					SetMessage((const tchar*)NULL, pasttime, sub_words->get_num());
				}
			} else {
				// Start first phase.
				//DBW("%d %d %d %d", ss.GetSearchType(), ss.GetSearchState(), sub_ss.GetSearchType(), sub_ss.GetSearchState());
				__assert__;	// never come here
				//StartSubSearch();
			}
		}
		ret = false;
	}

	// Main Seach //
	if ( ss.IsSearching() && !ss.IsPaused() ){
		SearchProc( );
		SearchProc( );	// 高速化のため
		if ( ss.IsIncSearch()
			&& (!ss.IsSearching() || ss.IsPaused()) ){
			if (CanStartSubSearch()){
				// Start the sub search.
				if (sub_ss.GetSearchType()&SST_PHASE1){
					// End of the first phase search.
					//sub_ss.SetFirstPhaseCount();
#if 0	// Ver.5.4.18まであった問題の確認用
					if (sub_ss.IsSearching()){
						dbw("Error!! : %d", sub_words->get_num());
					}
#endif
					if (!sub_ss.IsSearching()	// まだ開始していない場合(2010.4.27)
						&& sub_words->get_num() < MAX_SUB_NDISP){
						// second phase search.
						_StartSubSearch(NULL, false);
					}
				} else
				if (sub_ss.GetSearchType()&SST_PHASE2){
					// End of all sub search.
				} else {
					// Start first phase.
					_StartSubSearch();
				}
			}
		}
		ret = false;
	}

	return ret;
}

