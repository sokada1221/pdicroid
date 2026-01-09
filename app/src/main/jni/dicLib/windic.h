#ifndef	__WINDIC_H
#define	__WINDIC_H

#include	"dicdef.h"
#ifdef	NOUSEDLL
#ifndef	__DIC3_H
#include	"dic3.h"
#endif	//__DIC3_H
#else
#include	"pdicdll.h"
#endif

#ifdef	NOUSEDLL
// DLL未使用バージョンの定義

// 検索フラグ
#define	PDSF_WORD		0x0001
#define	PDSF_JAPA		0x0002
#define	PDSF_ALL		0x0004
#define	PDSF_MEMORY		0x000c
#define	PDSF_JEDIT		0x0010

#define	PDSF_IGN		0x0100
#define	PDSF_HEAD		0x0200
#define	PDSF_REGEXP		0x0800
#define	PDSF_ZENHAN		0x1000
#define	PDSF_KANA		0x2000

#define	PDSF_BACKWARD	0x4000		// 後方検索フラグ
#define	PDSF_RESRCH		0x8000		// 継続検索フラグ

//typedef BOOL FAR PASCAL (*SEARCHSETPROC)( LPPDICDATA lpData, WORD wNo, LPVOID lpClientData );

// 見つかった単語のセット用コールバック関数
typedef BOOL (*FNSETWORDJAPA)( int num, const tchar *word, Japa &japa, void *user );

class WinPdic : public MultiPdic {
public:
	// 新規作成のみ、オープンはしない
	int Create( const tchar *path, const tchar *dicname, int dicsize, int dicid, int order, int fhyper );
//		int IncrementalSearch( const tchar *word, FindStruct &fs );
//		int SearchMark( MultiMarkInfo &pdmark );
//		int SearchJump( MultiMarkInfo &pdmark );
//		int SearchJump( const tchar *word, BOOL fBack )
//			{ return jumpAS( word, fBack ); }
//		int AllSearch( const tchar *word, WORD flag, SEARCHSETPROC lpfn, PFindStructAS lpfs, WORD wDicNo, PdicRegExp *rexp );
//		void SearchReverse( )
//			{ MultiPdic::reverse(); }
	int FindWord( const tchar *word, Japa *japa, int dicno )	// japaがNULLでないとセットされる
		{ return dsearch( word, japa, dicno ); }
	int SearchSmallest( const tchar *str, tnstr &word, diclist_t nDicList );

	// 検索
	// ヒットする最も近い英単語を探しその検索用英単語を得る
	BOOL SearchOptimalWord( const tchar *word, int maxnum, tnstr &searchword );
	// wordで始まる単語をすべて得る
	void ForwardSearch( const tchar *word, int maxnum, FNSETWORDJAPA func, void *user );

	// strにヒットする英単語の辞書の数を探す
	// flag : DICF_...
	int GetHitNum( const tchar *str, diclist_t nDicList = dl_val_all, int flag = -1 );

#if defined(USE_JLINK)
	// オブジェクトから辞書番号を求める
	int GetDicNoFromObject( JLink &jl );
#endif
};
#else
#define	LJAPABUF	1000
#define	LWORDBUF	256
class PdicDll {
protected:
	PDIC pdic;

	// 以下のバッファは共有する
	static PDICDATA setdata;
	static PDICDATA getdata;		// japa,maxjapaはjapabuf,LJAPABUFに固定されている
	static tchar *wordbuf;
	static tchar *japabuf;

	static int nObjNum;			// このオブジェクトの数

	tchar szTempName[ MAXDICNAME ];
public:
	PdicDll();
	~PdicDll();
	BOOL Initialize( HANDLE hInstance )
		{ return ::Initialize( hInstance, &pdic ); }
	BOOL Exit( )
		{ return ::Exit( &pdic ); }
	BOOL Open( LPCTSTR lpFilename, BOOL bReadOnly )
		{ return ::Open( &pdic, lpFilename, bReadOnly ); }
	BOOL Close( int nDicNo )
		{ return ::Close( &pdic, nDicNo ); }
	BOOL AllClose( )
		{ return ::AllClose( &pdic ); }

	// 情報取得

	int GetErrorCode( )
		{ return ::GetErrorCode( &pdic ); }
	const tchar *GetDicName( int nDicNo  )
		{
			::GetDicName( &pdic, nDicNo, szTempName, MAXDICNAME );
			return szTempName;
		}
	int GetDicNum( )
		{ return ::GetDicNum( &pdic ); }
	int GetErrorDicNo( )
		{ return ::GetErrorDicNo( &pdic ); }

//		WORD GetAttrLen( int dicno )
//			{ return ::GetAttrLen( &pdic, dicno ); }

	// 検索

//		int SearchOne( const tchar *word );	// １つの単語のみを検索
//											// 返り値は辞書番号又は辞書数

//		const tchar *GetFindWord( );			// 最後の検索の得られた単語、日本語訳
//		const tchar *GetFindJapa( );

	int FindWord( const tchar *word, short nDicNo )
		{ return ::UFindWord( &pdic, word, nDicNo ); }

	int FindDicNo( const tchar *word )
		{ return ::FindDicNo( &pdic, word ); }

	int IncrementalSearch( const tchar *word, FindStruct &fs );
		// 返り値は見つかった単語数
		// maxnumより多いときは、maxnum+1を返す

	int AllSearch( const tchar *word, WORD flag, SEARCHSETPROC lpfn, PFindStructAS lpfs )
		{ return ::AllSearch( &pdic, word, flag, lpfn, (LPVOID)lpfs ); }
	int AllSearch( const tchar *word, WORD flag, SEARCHSETPROC lpfn, PFindStructAS lpfs, WORD wDicNo )
		{ return ::UAllSearch( &pdic, word, flag, lpfn, (LPVOID)lpfs, wDicNo ); }

	int GetLastFindDicNo( )
		{ return ::GetLastFindDicNo( &pdic ); }

//		int SearchReverse( );

	// マーク・ジャンプ

#if 0
	int SearchMark( PDICMARK &pdmark )
		{ return ::SearchMark( &pdic, &pdmark ); }
	int SearchJump( PDICMARK &pdmark )
		{ return ::SearchJump( &pdic, &pdmark ); }
#endif


	// モード設定、取得

	BOOL SetMergeMode( BOOL f )
		{ return ::SetMergeMode( &pdic, f ); }
	BOOL GetMergeMode( )
		{ return ::GetMergeMode( &pdic ); }

	// 登録、修正、削除
	int Record( const tchar *word, Japa &japa, short dicno );
	int Update( const tchar *word, Japa &japa, short dicno );
	int Erase( const tchar *word, short dicno )
		{ return ::Erase( &pdic, word, dicno ); }
	int AllErase( const tchar *word )
		{ return ::AllErase( &pdic, word ); }
};
#endif

#ifdef	NOUSEDLL
class MPdic : public WinPdic {
};
#else
class MPdic : public PdicDll {
};
#endif

#ifdef OLETEST
extern Pdic dic;
#else
//extern MPdic dic;
#endif

MPdic *GetActiveDic();	// defined in MainDic.cpp

#endif	// __WINDIC_H

