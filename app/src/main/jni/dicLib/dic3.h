#ifndef	__DIC3_H
#define	__DIC3_H

#include "pdconfig.h"

#ifdef USE_UNIVDIC
#include "univdic.h"
#else
#define	TUniversalDic	Pdic
#include	"dic.h"
#endif

#include	"japa.h"
#include	"que.h"
#include	"multidef.h"
#include "multiwd.h"
#include "LangProcDef.h"
#include "diclist.h"

#if USE_MARK
class MarkArray : public ObjectArray<DicLoc> {
public:
	MarkArray():ObjectArray<DicLoc>( MAX_MULTIDIC ){}
	~MarkArray(){}
};
#endif

#if 0
#define	MPF_NETWORK		0x0001		// ネットワーク用
#define	MPF_MERGE		0x0002		// マージモード
#define	MPF_PARA		0x0004		// 並列表示モード
#define	MPF_DICNAME		0x0008		// 辞書名を訳語に付加
// MPF_MERGEもMPF_PARAも指定しないときは、MD_NOTMERGEと同じ扱い
#endif

class TMultiAllSearchParam {
protected:
	int num;	// 現在開いている辞書の数
	AllSearchParam *Params[MAX_MULTIDIC];
public:
	MultiWord mw;		//検索単語などの格納
	int srchflag[MAX_MULTIDIC];	//検索する・しないのフラグテーブル 0:しない 1:する -1:既に終了
	int lastdicAS;				//最後に見つかった辞書の番号
	const TKCodeTranslateSet *KCodeTrans;
	bool multiThread;
public:
	TMultiAllSearchParam();
	~TMultiAllSearchParam();
	// Operations //
	void Open(class MultiPdic &dic);
	void AllClose();
	const TKCodeTranslateSet *GetKCodeTrans() const
		{ return KCodeTrans; }
protected:
	void FillParams();
public:
	void SetupSearch(bool dirflag=false);	// dirflag = true if backward.
	void ResetSearch();
	void Stop(bool wait);
	int GetFoundWord(tnstr &word, Japa *japa);
	int GetNearestWord(const tchar *str, tnstr &word, Japa *japa);
	// Getter/Setter //
	int GetMode() const
		{ return mw.GetMode(); }
	void SetMode(int mode)
		{ mw.SetMode(mode); }
	int SetMergeMode(int mode);
	// operators //
	AllSearchParam *operator[](int index) const
		{ return Params[index]; }
	int GetLastFindDicNo()
		{ return lastdicAS; }
};

class TMultiDicArray : public FlexObjectArrayBase {
typedef FlexObjectArrayBase super;
protected:
	virtual void delete_object( void *obj )
	{
		if (obj)
			((TUniversalDic*)obj)->Release();
	}
public:
	TMultiDicArray(int maxdic)
		:super(NULL, maxdic)
	{
	}
	TUniversalDic &operator [](int i)	const {return *(TUniversalDic*)array[i];}
};

class MultiPdic {
protected:
	TMultiDicArray sdic;
	int errcode;				// マルチ辞書関連のエラーコード（すべてのエラーを表していない）
	int errdic;					// エラーの発生した辞書番号(-3:どの辞書でもない 0以降:標準辞書)
								// ただし、常にエラーの発生した辞書とは限らない（エラーが返った直後が正しい）
	TMultiAllSearchParam SrchParam;

// 不規則辞書関係
public:
	bool OpenIrregDic();	// irregular dic. fileに変更があったか調べる
protected:
	void CloseIrregDic();
public:
	bool SearchIrreg( const tchar *word, tnstr &trsword );

	// Language Processor //
protected:
	TLangProc *LangProc;
	const TKCodeTranslateSet *KCodeTrans;
public:
	void SetLangProc(const tchar *name){}
	TLangProc *GetLangProc() const
		{ return LangProc; }
// Should include LangProc.h before including this header.
#ifdef LangProcH
	tnstr create_composit_word(const tchar *word)
		{ return LangProc->CompositeWord(word); }
	tnstr create_kword(const tchar *word)
		{ return LangProc->KWord(word); }
#endif	// LangProcH
	inline const TKCodeTranslateSet *GetKCodeTrans() const
		{ return KCodeTrans; }

public:
	MultiPdic();
	virtual ~MultiPdic();
	const tchar *GetDicName( int i )
		{ return sdic[i].GetFileName( ); }
	int SearchDicName( const tchar *dicname );
	int GetDicNum( )				{ return sdic.get_num(); }
#if !defined(USE_UNIVDIC)
	int Open( int dn, const tchar *filename, const tchar *epwname, const tchar *gconvname, const tchar *flinkpath, int openflag=DICFLAG_NONE, const tchar *nettempfile=NULL );
#endif
		//指定の辞書番号へ挿入する
	TMultiAllSearchParam *CreateSearchParam();
protected:
	TUniversalDic *_open( const tchar *pathname, const tchar *indexname, const tchar *gconvname, const tchar *flinkpath, int dicno, int _readonly, const tchar *nettempname=NULL );
public:
#ifdef MACCS
	bool CanClose( );
#endif
	void AllClose();	// 辞書を全てクローズして、初期化する
	int ODAClose();
	void ODAReopen(int flags=-1);
protected:
	int Close( int dn );						//指定の辞書をクローズし、削除する
public:
	virtual TUniversalDic *new_open( const tchar *fname, int dicno )	{return 0;}
	bool ThreadUp();
	void ThreadDown();
#if USE_DICFASTMODE
	void SetFastMode( bool bFast );
#endif
	int GetErrorCode( );
	int GetErrorDicNo( )		{return errdic;}
	TUniversalDic &operator []( int i )	{return sdic[i];}

#if !defined(SMALL)
	int GetMergeMode( )	{ return SrchParam.GetMode(); }
	int SetMergeMode( int _mode );	//返り値は前のmode
#endif

	////////////////////////////
	//Pdicクラスと同じメソッド//
	////////////////////////////
	//検索
protected:
//	int BSearch( const tchar *word, int dn )
//		{ return bsearch( word, dn ); }
	int bsearch( const tchar *word, int dn, AllSearchParam *all );
public:
	int dsearch( const tchar *word, Japa *japa, int dn );			//指定の辞書に、wordがあるかどうかを調べる
#ifdef PDICW
	int dn_search( const tchar *word, int flags=-1 );					//wordが見つかる最初の辞書を探す
#else
//	int dn_search( const tchar *word );
#endif
	int Find( const tchar *word, Japa *japa=NULL, diclist_t diclist=~0 );
	int FindPart( const tchar *str, diclist_t diclist=~0 );
	//int Search( const tchar *str, tnstr &word, Japa *japa, diclist_t diclist=~0 );
	int Search2( const tchar *str, tnstr &word, Japa *japa, diclist_t nDicList=~0);
	bool SearchPart( const tchar *str, tnstr_vec &words, diclist_t nDicList );
	bool EnumKeyWords(const tchar *key, tnstr_vec &words, diclist_t nDicList=~0, bool searchpart=false);
	int Read( const tchar *str, Japa *japa, diclist_t nDicList=~0 );

	// 登録、修正、削除
	int Record( const tchar *word, const Japa &japa, int dicno )
		{ return MultiPdic::record( word, japa, dicno ); }
	int Update( const tchar *word, const Japa &japa, int dicno )
		{ return MultiPdic::update( word, japa, dicno ); }
	int Update0( const tchar *word, const Japa &japa, int dicno )		// for network
		{ return MultiPdic::update0( word, japa, dicno ); }
	int Erase( const tchar *word, int dicno )
		{ return MultiPdic::erase( word, dicno ); }
	int AllErase( const tchar *word )
		{ return MultiPdic::all_erase( word ); }

	//基本操作
	int record( const tchar *word, const Japa &japa, int dn=0 );		//単語の追加(ユーザ辞書へ)
	int update( const tchar * word, const Japa &japa, int dn );			//単語を修正
	int erase( const tchar * word, int dn );						//wordを削除
	int all_erase( const tchar * word );							//すべての辞書からwordを削除
	int rename( const tchar *oldword, const tchar *newword, int dn );

	//属性処理
	int GetAttrLen( int )
		{ return 1; }
	//int GetAllAttrLen( );
	wa_t get_attr( const tchar *word );
	int set_all_attr( const tchar *word, int bit );
	int reset_all_attr( const tchar *word, int bit );
	int change_attr( const tchar *word, uchar attr, uchar mask, diclist_t dicflags=~0 );
	int ChangeAttrEx(const tchar *word, uchar bit, bool f, wa_t addattr);

	// ネットワーク用のための処理
	int CheckEraseUser( const tchar * word );
	int update0( const tchar * word, const Japa &japa, int dn );			//単語を修正
	int set_all_attr0( const tchar *word, int bit );
	int reset_all_attr0( const tchar *word, int bit );
	int change_all_attr0( const tchar *word, uchar attr, uchar mask );
	int set_attr0( const tchar *word, int bit, int dn );
	int reset_attr0( const tchar *word, int bit, int dn );
	int add_level0( int dn, const tchar *word, int level, int minlevel=WA_MINLEVEL, int maxlevel=WA_MAXLEVEL );

	//インクリメンタルサーチ
	void initNsearch( const tchar * word, TMultiAllSearchParam *all=NULL );
	void initNsearch2( const tchar * word, TMultiAllSearchParam *all=NULL );
	bool CheckPrevWord( const tchar *word, tnstr *fword=NULL );
	bool CheckNextWord( const tchar *word, tnstr *fword=NULL );
	int nsearch( tnstr &fword, Japa &fjapa, int dicno );

	//全検索
//	TMultiAllSearchParam &GetSrchParam()
//		{ return SrchParam; }
	int SetAllSearch( const tchar * word, SrchMode mode, GENERICREXP *regp )
		{ return setAS( word, mode, regp ); }
	int setAS( const tchar * word, SrchMode mode, GENERICREXP *regp, TMultiAllSearchParam *all=NULL );
	void SetDirection( bool bForward );
	void resetAS( );
	int NextAllSearch( tnstr &word, Japa *japa, diclist_t diclist )
		{ return nextAS( word, japa, diclist ); }
	int nextAS( tnstr &word, Japa *japa, diclist_t diclist, TMultiAllSearchParam *all=NULL );		//辞書指定検索
	int nextASMT( tnstr &word, Japa *japa, diclist_t diclist, TMultiAllSearchParam *_all=NULL );
	int nextASMT( tnstr &word, Japa *japa, diclist_t diclist, TMultiAllSearchParam *_all, bool forward );
	int nextAS2( tnstr &word, Japa *japa, diclist_t diclist );		// AS_CONINUEがないバージョン
	int PrevAllSearch( tnstr &word, Japa *japa, diclist_t diclist )
		{ return prevAS( word, japa, diclist ); }
	int prevAS( tnstr &word, Japa *japa, diclist_t diclist, TMultiAllSearchParam *all=NULL );		//辞書指定検索
	int prevASMT( tnstr &word, Japa *japa, diclist_t diclist, TMultiAllSearchParam *all=NULL );
	int GetLastFindDicNo()
		{ return SrchParam.GetLastFindDicNo(); }
public:
	int JumpAS( const tchar *word, bool fBack, bool fSameBack=false, TMultiAllSearchParam *all=NULL )
		{ return jumpAS( word, fBack, fSameBack, all ); }
	int jumpAS( const tchar *word, bool fBack, bool fSameBack=false, TMultiAllSearchParam *all=NULL );

	// LangProc depend search //
	int SearchLongestWord( const tchar *words, const tchar *prevwords, int curpos, int option, class MatchArray *HitWords );
	int SearchLongestWordExt(const tchar *words, const tchar *prevwords, class MatchArray *HitWords, int &thread_key, FNLPSLWExtCallback callback, int user);
	int SearchLongestWordCmd(int cmd, int thread_key);

	//情報取得
	int is_readonly( int dn )
		{ return sdic[dn].IsReadOnly(); }

public:
#ifdef USE_OLE
	void FreeObjects( );	// 使用されていないオブジェクトを解放する
#endif
public:
	bool SetLinkAttr( int flag, bool f );

#if defined(USE_DICORDER)
protected:
	t_order order;		// ソート順
public:
	int GetOrder( )
		{ return (int)order; }
	void SetOrder( int f )
		{ order = (t_order)f; }
	int cmpword( const _kchar *str1, const _kchar *str2 )
		{ return ::cmpword( str1, str2, order ); }
	int cmpnword( const _kchar *str1, const _kchar *str2, int n )
		{ return ::cmpnword( str1, str2, n, order ); }
#else
	int GetOrder( ) { return 0; }
	void SetOrder( int ){}
	int cmpword( const _kchar *str1, const _kchar *str2 )
		{ return ::cmpword( str1, str2  ); }
	int cmpnword( const _kchar *str1, const _kchar *str2, int n )
		{ return ::cmpnword( str1, str2, n ); }
#endif

	int GetDicNumCond( int flags );	// フラグのOR条件に一致する辞書の数を返す(DICF_...)
	int GetDicCond( int flags );	// フラグのOR条件に一致する辞書番号をフラグ化したものをかえす
	int GetFirstCond( int flags );	// フラグのOR条件に一致する最初の辞書番号を返す

#ifdef USE_SINGLEBYTE
	int SingleByte;	// シングルバイトフラグ(SRCH_...でor)
	void SetSingleByte( int f )
		{ SingleByte = f; }
	int GetSingleByte( )
		{ return SingleByte; }
#else
	int GetSingleByte( )
		{ return 0; }
#endif
	int SearchFileLinkPath( const tchar *filename, const tchar *commonpath, tnstr &fullpath );

#ifdef USE_UNIVDIC
	int Bind( TUniversalDic *dic, int dn=-1 );
#endif

	// for InetDic //
public:
	bool IsNetworkUsed();
	int ReqUpload();
	void ReqDownload(int max_count);
	bool IsNetDownloadAllowed();
	void SetNetDownloadAllowed(bool allow);
};

#endif	// __DIC3_H

