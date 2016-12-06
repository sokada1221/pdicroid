#include	"tnlib.h"
#pragma	hdrstop
#include "ole2s.h"
#include "pdconfig.h"
#include "LangProc.h"
#include	"multidic.h"
#include	"japa.h"
#include	"jrectrl.h"
#include	"wpdcom.h"

// strが見つかる辞書の数を返す
// nDicList : 検索対象辞書（フラグ式）
// flag : 検索対象辞書条件(DICF_...)
int WinPdic::GetHitNum( const tchar *str, diclist_t nDicList, int flag )
{
	bool composit = is_composit_word(str);
#if MIXDIC
	_kwstrdef(__str, str);
#else
	#define	__str	str
#endif
	int n = 0;
	for ( int i=0;i<sdic.get_num();i++ ){
		if ( !dl_check(nDicList, i) ){
			continue;
		}
		if ( !sdic[i].CheckCond( flag ) ){
			continue;
		}
		int r = sdic[i].BSearch( (const _mchar*)__str );
		if ( r != -1 ){
			if ( r == 1 ){
				if (composit){
					if ( !cmpword( __str, sdic[i].getfword() ) ){
						n++;
					}
				} else {
					// str is a cword.
					if ( !cmpword( __str, find_cword_pos(sdic[i].getfword()) ) ){
						n++;
					}
				}
			}
		} else {
			return -1;	// エラー
		}
	}
	return n;
}
#if !NEWINCSRCH
// strに最も近い単語をdiclistで指定する辞書から探し、wordにセットする
// ただし、日本語訳は正しく無い！！！！！
// diclist:辞書リスト
// 返り値：-1 エラー
//			1 見つかった
//			0 無かった
int WinPdic::SearchSmallest( const tchar *str, tnstr &word, diclist_t diclist )
{
	int ndic = sdic.get_num();
	MultiWord mw( ndic );
	int srchflag[ MAX_MULTIDIC ];
	for ( int i=0;i<ndic;i++ ){
		if ( !dl_check(diclist, i) ){
			srchflag[i] = -1;
			continue;
		}
		int r = sdic[i].bsearch( str );
		if ( r != -1 ){
			if ( r ){
				Japa fjp;
				sdic[i].getfjapa( fjp );
				mw.set( i, _mstr(sdic[i].getfword()), fjp );
				srchflag[i] = 0;
			}
		} else {
			return -1;	// エラー
		}
	}
	int minstr;
	if ( mw.get_sameword( srchflag, minstr ) ){
		word.set( mw[ minstr ] );
		return 1;
	} else {
		return 0;
	}
}
#endif	// !NEWINCSRCH


#if 0
int WinPdic::Create( const tchar *path, const tchar *dicname, int dicsize, int dicid, int dicorder, int hyper )
{
	Pdic *newdic = new Pdic;
	if ( newdic == NULL )
		return -1;
#ifdef NEWDIC2
	if ( newdic->Create( path, dicname, dicsize, dicid, dicorder, hyper ) == -1 ){
#else
	if ( newdic->Create( path, dicname, dicsize ) == -1 ){
#endif
		int r = newdic->GetErrorCode();
		delete newdic;
		return r;
	}
	newdic->Close();
	delete newdic;
	return 0;
}
#endif

// ヒットする最も近い見出語を探しその検索用見出語を得る
// 戻り値:TRUE  １語以上ある
//        FALSE １語も無い
BOOL WinPdic::SearchOptimalWord( const tchar *word, int maxnum, tnstr &searchword )
{
	tnstr fw;
	tnstr sword = create_kword(word);

	tchar *p = &sword[sword.length()];
	tchar fc = 0;
	int loopct = 0;
	BOOL fFound = FALSE;
	int prevfound = 0;		// 前回の検索で見つかった単語数
	for(;;){
		fc = *p;
		*p-- = '\0';					//１文字ずつ単語を切り詰めていく

		int foundnum = 0;
		initNsearch( sword );
		while ( foundnum < maxnum+1 ){
			switch ( nextAS( fw, NULL, -1 ) ){
				case AS_FOUND:
#if 0
					if ( dic.cmpnword( fw, sword, l ) > 0 ){
						// 検索終り
						break;
					}
#endif
					fFound = TRUE;
					foundnum++;
					continue;
				case AS_CONTINUE:
					continue;
				default:
					break;
			}
			break;
		}
		if ( foundnum == maxnum+1 ){
			if ( !prevfound ){
				// 今回がmaxを越えて、前回は見つからなかった場合は今回の単語を用いるようにする
				loopct = 0;
			}
			break;
		}
		if ( sword[1] == '\0' )
			break;
		loopct++;
		prevfound = foundnum;
	}

	if ( loopct ){
		*++p = fc;
	}

	searchword = sword;

	return fFound;
}

// wordで始まる見出語をすべて取得する
void WinPdic::ForwardSearch( const tchar *word, int maxnum, FNSETWORDJAPA func, void *user )
{
	initNsearch( word );
	int num = 0;
	while ( maxnum ){
		tnstr fw;
		Japa fj;
		switch ( nextAS( fw, &fj, -1 ) ){
			case AS_FOUND:
#if 0
				if ( cmpnword( fw, word, strlen(word) ) > 0 )
					break;
#endif
				if ( !func( num, fw, fj, user ) )
					break;
				maxnum--;
				num++;
				continue;
			case AS_CONTINUE:
				continue;
			default:
				break;
		}
		break;
	}
}

#if defined(NEWDIC2) && defined(USE_JLINK)
// オブジェクトから辞書番号を求める
int WinPdic::GetDicNoFromObject( JLink &jl )
{
	for ( int i=0;i<GetDicNum();i++ ){
		if ( &sdic[i] == jl.GetDic() ){
			return i;
		}
	}
	return -1;
}
#endif

