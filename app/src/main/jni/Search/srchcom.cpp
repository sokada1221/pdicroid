#include "tnlib.h"
#pragma	hdrstop
#include "pddefs.h"
#include "srchcom.h"
#include "faststr.h"
#include "tid.h"
#include "msgbox.h"
#include "LangProc.h"

SearchCommonStruct::SearchCommonStruct( )
{
	fs = NULL;
#ifdef MIXMJ
	jfs = NULL;
#endif
	LangProc = NULL;
}

SearchCommonStruct::SearchCommonStruct( SrchStat *ss, TLangProc *lp, bool andSrch )
{
	Init(ss, lp, andSrch);
}

SearchCommonStruct::~SearchCommonStruct()
{
	if ( fs ) delete fs;
#ifdef MIXMJ
	if (jfs) delete jfs;
#endif
	if (LangProc)
		LangProc->Release();
}

void SearchCommonStruct::Init(SrchStat *ss, TLangProc *lp, bool andSrch )
{
	sword = andSrch ? ss->andWord : ss->word;
	SrchMode mode = andSrch ? ss->andMode : ss->mode;
	grexp = andSrch ? ss->andGrexp : ss->grexp;

	fIgnore = (mode & SRCH_IGN) == SRCH_IGN;
	fRegular = (grexp ? TRUE : FALSE);
#ifdef USE_SRCH_FUZZY
	fFuzzy = (mode & SRCH_FUZZY) == SRCH_FUZZY;
#endif
	fWord = (mode & SRCH_WORD) == SRCH_WORD;
	match = _tcslen( sword );
//	fSingle = false;
	fs = new _tFindStr( _mstr(sword,s), fIgnore );
#ifdef MIXMJ
	jfs = new _jFindStr( sword, fIgnore );
#endif
	LangProc = lp;
	if (LangProc && fFuzzy){
		LangProc->IncRef();
		sword_buf.set(sword);
		LangProc->NormalizeSearchPattern(sword_buf.c_str());
		sword = sword_buf.c_str();
	} else {
		fFuzzy = false;
		LangProc = NULL;
	}
}

void SearchCommonStruct::SetFS( const tchar *word, bool fIgnore )
{
	if ( fs ) delete fs;
	fs = new _tFindStr( _mstr(word,s), fIgnore );
#ifdef MIXMJ
	if ( jfs ) delete jfs;
	jfs = new _jFindStr( word, fIgnore );
#endif
}

// 検索共通ルーチン準備
// -1 : コンパイルエラー(主に正規表現)
// -2 : DLLエラー
// regsはオープン済みであり、呼出側でCloseをする。
int SearchCommonStruct::Setup( bool fRecompile )
{
	if ( fRecompile && grexp && fRegular ){
		if ( grexp->CanUse() ){
			if ( grexp->Compile( sword ) == TRUE ){
				// コンパイル成功
			} else {
				// コンパイルエラー
				//
				error = grexp->GetErrorCode( );
				return -1;
			}
		} else {
			return -2;
		}
	}
	len = _tcslen( sword );
	return 1;
}
// 検索共通ルーチン
// 0 : 見つからなかった
// 1 : 見つかった(posにstrからの位置)
int SearchCommonStruct::Search(const tchar *str)
{
	if (fFuzzy){
		CompBuf.set(str);
		LangProc->NormalizeSearchPattern(CompBuf.c_str());
		str = CompBuf;
	}
	if (fRegular){
		matches.clear();
		if ( grexp->Compare( str, &matches ) ){
			if (matches.get_num()==0){
				return 0;
			}
			pos = matches[0].loc;
			match = matches[0].len;
			return matches.get_num()>0;
		}
	} else {
		const tchar *q;
#if 0
		if ( fSingle ){
			if ( fIgnore ){
				q = fstristr( str, _tcslen(str), *GetTFS() );
			} else {
				q = fstrstr( str, _tcslen(str), *GetTFS() );
			}
		} else
#endif
		{
			if ( fIgnore ){
				q = jfstristr( str, _tcslen(str), *GetTFS() );
			} else {
				q = jfstrstr( str, _tcslen(str), *GetTFS() );
			}
		}
		if ( q ){
			pos = STR_DIFF( q, str );
			return 1;
		}
	}
	return 0;
}
// 返り値がTRUEのときは続行不可能
bool SearchCommonError( HWND hwnd, int status, int errorno)
{
	switch ( status ){
		case 1:
			return FALSE;
#ifndef WINCE
		case -1:
			MsgBox( hwnd, _LT(RGS_FIRST + errorno), _LT(CAP_REGULAREXPRESSION), MB_OK | MB_ICONSTOP );
			return TRUE;
		case -2:
			MsgBox( hwnd, _LT(RGS_CANNOTUSE), _LT(CAP_REGULAREXPRESSION), MB_OK | MB_ICONSTOP );
			return TRUE;
#endif
	}
	return FALSE;
}

