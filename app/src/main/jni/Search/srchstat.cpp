#include	"tnlib.h"
#pragma	hdrstop
#include	"dic.h"
#include	"srchstat.h"

static void SetupGRexpCom(GenericRexp *grexp, SrchMode &mode, bool &underline);

SrchStat::SrchStat()
{
	type = 0;
	state = 0;
	grexp = NULL;
	lastGrexpError = 0;
	underline = true;
	StartTime = 0;
//	first_count = 0;
#if USE_ANDSRCH
	andMode = (SrchMode)0;
	andGrexp = NULL;
#endif
}

SrchStat::~SrchStat()
{
	if ( grexp )
		delete grexp;
#if USE_ANDSRCH
	if (andGrexp)
		delete andGrexp;
#endif
}
void SrchStat::SetupSearch( int _type, const tchar *_word, SrchMode _mode, diclist_t diclist, int _level1, int _level2, int _MemOnOff)
{
	state = 0;
	type = _type;
	word.set( _word );
	mode = _mode;
	uDicList = diclist;
	level1 = _level1;
	level2 = _level2;
	MemOnOff = _MemOnOff;
#if USE_ANDSRCH
	ResetAndSearch();
#endif
}

#if USE_ANDSRCH
void SrchStat::SetupAndSearch(const tchar *word, SrchMode mode, GENERICREXP *grexp)
{
	andWord = word;
	andMode = mode;
	if (andGrexp)
		delete andGrexp;
	andGrexp = grexp;

	SetupGRexpCom(andGrexp, andMode, andUnderline);
}
void SrchStat::ResetAndSearch()
{
	andMode = (SrchMode)0;
	if (andGrexp)
		delete andGrexp;
	andGrexp = NULL;
}
#endif

// 検索・再検索開始
void SrchStat::StartSearch( bool fBack )
{
	state |= SS_SEARCHING;
	state &= ~SS_PAUSE;
	SetRemain( fBack );
	fBackWard = fBack;
	StartTime = clock();
}


void SrchStat::PauseSearch( const tchar *_lastfound )
{
	if ( _lastfound )
		lastfound.set( _lastfound );
	if ( state & SS_SEARCHING ){
		state |= SS_PAUSE;
	}
}

// インクリメンタルサーチ開始(再検索ではない）
void SrchStat::StartIncSearch( )
{
	state = 0;		// 1995.9.22弊害が出ないことを祈る
					// SS_NEEDRESETを解除する必要があるため
	StartSearch( false );
	type = SST_INCSEARCH;

	level1 = 0;
	level2 = WA_MAXLEVEL;
	MemOnOff = 0;
	findcount = 0;
	fcnum = 0;
#if USE_ANDSRCH
	ResetAndSearch();
#endif
}

// StartIncSearch()を行った後、単語が見つからず、再度同一条件で検索再開する場合のみ
void SrchStat::RestartIncSearch()
{
	__assert(findcount==0);
	state = SS_SEARCHING;
	SetRemain(false);
}

//Note:
// SetPatternのすんだgrexpを渡すこと
void SrchStat::SetGRexp( GenericRexp *_grexp )
{
	if ( grexp ){
		delete grexp;
	}
	grexp = _grexp;

	SetupGRexpCom(grexp, mode, underline);
}

static void SetupGRexpCom(GenericRexp *grexp, SrchMode &mode, bool &underline)
{
	if (grexp){
		//__assert(!grexp->GetPattern.empty());
		//grexp->SetPattern(_word);
		if (mode&SRCH_IGN){
			GenericRexp *rexp = grexp;
			do {
				rexp->SetFlag(GRXP_IGNORECASE, true);
				rexp = rexp->next;
			} while (rexp);
		}
		// 下線表示可能かどうかの判定
		if (grexp->_not /*|| (grexp->next&&grexp->connection==0)*/){
			// 否定形
			// ２つ以上指定ありで、AND検索
			// 下線表示をしない
			underline = false;
		}
	} else {
		mode = (SrchMode)(mode & ~SRCH_REGEXP);
	}
}

static bool IsRexpCom(GenericRexp *grexp, SrchMode mode)
{
	if ( grexp
		&& ( mode & SRCH_REGEXP )
		&& ( mode & SRCH_EXTMASK )
		&& ( mode & ( SRCH_WORD | SRCH_JAPA | SRCH_EXP | SRCH_PRON | SRCH_TITLE | SRCH_FILENAME | SRCH_FILECONT) )
		){
		return true;
	}
	return false;
}
bool SrchStat::IsRexp( )
{
	if (IsRexpCom(grexp, mode))
		return true;
	return IsRexpCom(andGrexp, andMode);
}

bool SrchStat::CanUseRexp( )
{
	if ( grexp ){
		return grexp->CanUse();
	}
	return false;
}

bool SrchStat::Compile( )	// grexpでコンパイルする(必ずCanUseRexp()でチェックした後に呼ぶこと！）
{
	if (IsRexpCom(grexp, mode)){
		if (!grexp->Compile()){
			lastGrexpError = grexp->GetErrorCode();
			return false;
		}
	}
	if (IsRexpCom(andGrexp, andMode)){
		if (!andGrexp->Compile()){
			lastGrexpError = andGrexp->GetErrorCode();
			return false;
		}
	}
	return true;
}

void SrchStat::NeedReset( const tchar *_lastfound )
{
	if ( state & SS_SEARCHING ){
		lastfound.set( _lastfound );
	}
	state |= SS_NEEDRESET;
}

// 反転表示可能な検索であるかどうか
// bitにはSRCH_WORD,SRCH_JAPA...などが入る
bool SrchStat::IsTextSearch( SrchMode bit )
{
	return (type & SST_SEARCH) && !(mode & SRCH_ALL) && (mode & bit);
}

// [mSec]
int SrchStat::GetPastTime() const
{
	return (int)(clock() - StartTime);
}

//Note:
//	Let o.grexp be NULL!!
SrchStat &SrchStat::Assign(SrchStat &o)
{
	word = o.word;
	uDicList = o.uDicList;
	level1 = o.level1;
	level2 = o.level2;
	MemOnOff = o.MemOnOff;
	if (grexp)
		delete grexp;
	grexp = o.grexp;
	o.grexp = NULL;
	lastfound = o.lastfound;
	underline = o.underline;
	mode = o.mode;
	type = o.type;
	state = o.state;
	findcount = o.findcount;
	fcnum = o.fcnum;
#if USE_ANDSRCH
	andWord = o.andWord;
	andMode = o.andMode;
	if (andGrexp)
		delete andGrexp;
	andGrexp = o.andGrexp;
	o.andGrexp = NULL;
	andUnderline = o.andUnderline;
#endif
//	first_count = o.first_count;
	fBackWard = o.fBackWard;
	StartTime = o.StartTime;
	return *this;
}

