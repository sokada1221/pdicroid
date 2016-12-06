#ifndef	__SRCHSTAT_H
#define	__SRCHSTAT_H

#include "dicdef.h"
#include "diclist.h"

#define	USE_ANDSRCH			1	// AND訳語検索(also defined in SrchMed.h)

// 検索状態を制御・管理

#define	SST_INCSEARCH	0x0001
#define	SST_SEARCH		0x0002
#define	SST_NODESEARCH	0x0004
#define	SST_SUBSEARCH	0x0010	// sub search
//#define	SST_FIRSTSEARCH	0x0020	// sub searchの頭出し検索フラグ
#define	SST_PHASE1		0x0020	// sub searchのfirst phase(頭出し検索)
#define	SST_PHASE2 		0x0040	// sub searchのsecond phase(全文検索)

#define	SS_SEARCHING	0x0001	// 現在検索実行中
#define	SS_PAUSE		0x0002	// 検索停止中(TempCloseなどで) - SS_SEARCHINGが立っていないと無視される
#define	SS_FWD			0x0004	// 前方にまだデータが有り
#define	SS_BWD			0x0008	// 後方にまだデータが有り
#define	SS_NEEDRESET	0x0080	// 検索を再設定しないと再検索できない！フラグ

#define	F_FORWARD		FALSE
#define	F_BACKWARD		TRUE

class SrchStat {
public:

// 検索パラメータ ///////////////
// 共通
	tnstr word;				// 検索語（検索再開時に使用する）
	diclist_t uDicList;			// 検索対象辞書リスト
	int level1;				// 単語レベル制限
	int level2;
	int MemOnOff;			// 暗記必須制限(0:なし 1:On 2:Off)
	GenericRexp *grexp;
	int lastGrexpError;
	tnstr lastfound;		// 最後に見つかった見出し語
							// 再検索用 - NeedResearch()を呼ぶとセットされる
	bool underline;			// 下線表示可能か？

// 通常の検索固有
	SrchMode mode;

protected:
// 基本検索状態変数
	int type;
	int state;

// 検索状態
	int findcount;			// 見つかった単語数
	UINT fcnum;				// バッファが一杯になっても強制的に続行する語数
							// ヒットする単語がある限り、必ずfindcount == fcnumとなる
							// もし、使用しない場合は必ず0にすること！！

public:
#if USE_ANDSRCH
	tnstr andWord;
	SrchMode andMode;
	GenericRexp *andGrexp;
	bool andUnderline;
	void ResetAndSearch();
#endif
public:
// 後方検索用ワーク変数

	bool fBackWard;			// 後方検索

protected:
	int StartTime;			// 検索開始時刻

public:
	SrchStat( );
	~SrchStat();

	// セット
	void SetupSearch( int _type, const tchar *_word, SrchMode _mode, diclist_t diclist, int _level1, int _level2, int _MemOnOff);
	void SetupAndSearch(const tchar *word, SrchMode mode, GENERICREXP *grexp);
	bool IsAndSearch()
		{ return andMode!=0; }

	// 正規表現検索関係
	void SetGRexp( GENERICREXP *_grexp );
	// 以下の関数は、SetupSearchを行なった後に呼ぶこと！！
	bool IsRexp( );		// grexp,modeを調べて、正規表現検索を行なうべきものであるかどうか
	bool CanUseRexp( );	// grexpを調べて、正規表現検索を使えるかどうか調べる
	bool Compile( );	// grexpでコンパイルする(必ずCanUseRexp()でチェックした後に呼ぶこと！）
	int GetRexpErrorCode()
		{ return lastGrexpError; }

	// 検索開始
	void StartSearch( bool fBack );
	void StartIncSearch( );
	void RestartIncSearch();
	// 検索中止、終了
	void StopSearch( )
		{ state &= ~(SS_SEARCHING|SS_PAUSE); }
	void PauseSearch( const tchar *_lastfound );
	void ContinueSearch( )
		{ state &= ~SS_PAUSE; }
	// 指定方向にデータ有り
	void SetRemain( bool f )	// f は後方検索でTRUE 前方検索でFALSE
		{ state |= ( f ? SS_BWD : SS_FWD ); }
	// 指定方向のデータはない
	void SetNothing( bool f )	// f は後方検索でTRUE 前方検索でFALSE
		{ state &= ~( f ? SS_BWD : SS_FWD ); }
	bool IsIncSearch( )
		{ return type & SST_INCSEARCH ? true : false; }
	bool IsSubSearch()
		{ return type & SST_SUBSEARCH ? true : false; }
	bool IsSubSearchEnd()	// PHASE2までsub searchを終了したか？
		{ return (type & SST_PHASE2) && (state==0); }

	int GetSearchType( ) const
		{ return type; }
	void SetSearchType(int _type)
		{ type = _type; }
	void ClearSearchType()
		{ type = 0; state = 0; }

	int GetSearchPhase()
		{ return type & (SST_PHASE1|SST_PHASE2); }
	void SetPhase(int phase)
		{ type = type & ~(SST_PHASE1|SST_PHASE2) | phase; }

	int GetSearchState() const
		{ return state; }
	void AddSearchState(int flag)
		{ state |= flag; }
	void ClearSearchState()
		{ state = 0; }

	void ClearNeedReset()
		{ state &= ~SS_NEEDRESET; }
	bool IsSearching( ) const
		{ return state & SS_SEARCHING; }
	bool IsPaused() const
		{ return state & SS_PAUSE; }
	void NeedReset( const tchar *_lastfound );
	bool IsTextSearch( SrchMode bit );	// 反転表示可能な検索であるかどうか
	bool CheckLevel(int level) const
	{
		return level >= level1 && level <= level2;
	}

	int GetFindCount() const
		{ return findcount; }
	int GetFcnum() const
		{ return fcnum; }
	void IncFindCount()
	{
		findcount++;
	}
	//TODO: StartSearch()と一緒にできればしたい
	void ResetCount()
	{
		findcount = 0;
		fcnum = 0;
	}
	// Request additional search.
	void ReqAdd(int _fcnum)
	{
		findcount = 0;
		fcnum = _fcnum;
	}
	bool IsFindCompleted()
		{ return findcount>=fcnum; }
	bool IsFound() const
		{ return findcount > 0; }

	int GetPastTime() const;

	// operators //
	SrchStat &Assign(SrchStat &);
};

#endif	// __SRCHSTAT_H

