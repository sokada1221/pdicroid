// begin of jreusr.h
// ライブラリの使用者用ヘッダファイル

#ifndef		JREUSR
#define		JREUSR

// __WINDOWS_H is used by Borland C++, _INC_WINDOWS is used by Visual C++ why not same?

#if !defined(__WINDOWS_H) && !defined(_INC_WINDOWS)

#ifndef STRICT
#define STRICT
#endif	// STRICT
#include <windows.h>

#endif	// !defined(__WINDOWS_H) && !defined(_INC_WINDOWS)


// ____________________________________________________________型定義



// ____________________________________________________グローバル変数



// ______________________________________________________________定数
#ifndef _JRE_ERR_CODE
#define _JRE_ERR_CODE
// エラーコード.nErrorが1～63まで.
#define		CantAllocate	(01)	//	メモリの確保ができない.絶対的なメモリ不足.
#define		MemoryTooFew1	(02)	//	メモリが足りない1.(実行時に決まるもの.メモリサイズの予測に失敗した)
#define		MemoryTooFew2	(03)	//  メモリが足りない2.(DLL作成時に決まるもの)
#define		ReTooLong		(10)	//	正規表現が長すぎる.
#define		TooComplex		(13)	//	もっと簡単な表現にして下さい.このDLLの弱いパターンで,見つけられない.ただし滅多な事では発生しない.
#define		MismatchBracket	(20)	//	括弧の対応が正しくない.
#define		InvalidChClass	(21)	//	キャラクタクラスの内容が解釈できない.殆どの場合範囲指定が正しくない.
#define		EscErr			(24)	//	エスケープシークェンスが解釈できない.
#define		Unknown			(31)	//  なんだか良くわからないエラー.複合的な要因で発生するエラー.殆どがメモリサイズの予測に失敗.
#define		NoReString		(32)	//  検索パターンを指定して下さい.正規表現長が0バイト.
#define		IncorrectUsing	(33)	//  パラメータがおかしい.DLLの不正な使用法.
#define		ReNotExist		(34)	//	検索する前にコンパイルして下さい.
#define		InternalErr		(35)	//  DLLのバグを検出した.これが発生したら作者に連絡して欲しい.
#define		UsrAbort		(36)	//  ユーザー(アプリケーション)による中断.
#define		OldVersion		(37)	//	古い(解釈できない)バージョンの呼び出し手順を使用した.
// CantAllocate, TooComplex, MemoryTooFew2, ReNotExist, IncorrectUsing, UsrAbortは検索時にも発生するもの
#endif
#define JGC_SHORT (1)
#define JGC_LONG (2)

// jre.dllで使用するメッセージの最大保証長.このサイズのバッファに収まらない文字列は渡しません(nullまで含めてこのサイズ).
#define JRE_MAXLEN (128)

// GetJreMessageの言語番号.
#define GJM_JPN (0)
#define GJM_ENG (1)


// ____________________________________________________________構造体
// JRE構造体.
// JRE構造体は極力使用しないでください.今後のﾊﾞｰｼﾞｮﾝで廃止します(バージョン2.xxまででｻﾎﾟｰﾄを止めます).
// これに代わる構造体はJRE2構造体です.
#pragma pack(push,1)
typedef struct tagJRE{
	BOOL bConv;						// この構造体の使用中を示すフラグ.
	int nStart;						// 検索開始位置.バイト数.先頭は0.
	int nWarning;					// ウォーニングコード.
	int nError;						// エラーコード.
	int nLength;					// マッチ長.バイト数.
	int nPosition;					// マッチ位置.先頭は0.(お恥ずかしい)
	WORD wTranslate;				// 変換テーブル番号.
	LPSTR lpszTable;				// 変換テーブル.
	FARPROC lpfnUsrFunc;			// コールバック関数へのFARポインタ.
	int nCompData1;					// コンパイルデータ1.
	GLOBALHANDLE hCompData2;		// コンパイルデータ2.ハンドル.
	GLOBALHANDLE hCompData3;		// コンパイルデータ3.ハンドル.
	GLOBALHANDLE hCompData4;		// コンパイルデータ4.ハンドル.
} JRE;
typedef JRE *PJRE;
typedef JRE NEAR *NPJRE;
typedef JRE FAR *LPJRE;


// バージョン1.06以降はこの構造体を使用してください.
// ただし,バージョン1.xxでは仕様が流動的です(どないせーちゅうねん).
#ifdef STRICT
typedef struct tagJRE2{
	DWORD dwSize;					// この構造体のサイズ.
	BOOL bConv;						// この構造体の使用中を示すフラグ.
	UINT nStart;					// 検索開始位置.バイト数.先頭は0.
	int nWarning;					// ウォーニングコード.
	int nError;						// エラーコード.
	UINT nLength;					// マッチ長.バイト数.
	UINT nPosition;					// マッチ位置.先頭は0.(お恥ずかしい)
	WORD wTranslate;				// 変換テーブル番号.
	LPSTR lpszTable;				// 変換テーブル.
	FARPROC lpfnUsrFunc;			// コールバック関数へのFARポインタ(使わないように).
	UINT nCompData1;					// コンパイルデータ1.
	HGLOBAL hCompData2;		// コンパイルデータ2.ハンドル.
	HGLOBAL hCompData3;		// コンパイルデータ3.ハンドル.
	HGLOBAL hCompData4;		// コンパイルデータ4.ハンドル.
} JRE2, * PJRE2, NEAR * NPJRE2, FAR * LPJRE2;
#else
typedef struct tagJRE2{
	DWORD dwSize;					// この構造体のサイズ.
	BOOL bConv;						// この構造体の使用中を示すフラグ.
	int nStart;						// 検索開始位置.バイト数.先頭は0.
	int nWarning;					// ウォーニングコード.
	int nError;						// エラーコード.
	int nLength;					// マッチ長.バイト数.
	int nPosition;					// マッチ位置.先頭は0.(お恥ずかしい)
	WORD wTranslate;				// 変換テーブル番号.
	LPSTR lpszTable;				// 変換テーブル.
	FARPROC lpfnUsrFunc;			// コールバック関数へのFARポインタ(使わないように).
	int nCompData1;					// コンパイルデータ1.
	GLOBALHANDLE hCompData2;		// コンパイルデータ2.ハンドル.
	GLOBALHANDLE hCompData3;		// コンパイルデータ3.ハンドル.
	GLOBALHANDLE hCompData4;		// コンパイルデータ4.ハンドル.
} JRE2, * PJRE2, NEAR * NPJRE2, FAR * LPJRE2;
#endif	// STRICT
#pragma pack(pop)

// _________________________________________________ﾌﾟﾛﾄﾀｲﾌﾟおよびﾏｸﾛ
// バージョンに関係ないAPI
BOOL FAR PASCAL IsMatch(LPSTR lpszStr, LPSTR lpszRe);
int FAR PASCAL GlobalReplace(LPSTR lpszRe, LPSTR lpszObj, LPSTR lpszStr, LPHANDLE lphGMemTo);
WORD FAR PASCAL JreGetVersion(void);
HGLOBAL FAR PASCAL DecodeEscSeqAlloc2(LPSTR lpszRe);
int FAR PASCAL GetJreMessage(int nMessageType, int nLanguage, LPSTR lpszBuff, int cbBuff);
// バージョン1API
BOOL FAR PASCAL JreOpen(LPJRE lpjreJre);
BOOL FAR PASCAL JreCompile(LPJRE lpjreJre, LPSTR lpszRe);
BOOL FAR PASCAL JreGetMatchInfo(LPJRE lpjreJre, LPSTR lpszStr);
BOOL FAR PASCAL JreClose(LPJRE lpjreJre);
// バージョン2API
BOOL FAR PASCAL Jre2Open(LPJRE2 lpjreJre);
BOOL FAR PASCAL Jre2Compile(LPJRE2 lpjreJre, LPSTR lpszRe);
BOOL FAR PASCAL Jre2GetMatchInfo(LPJRE2 lpjreJre, LPSTR lpszStr);
BOOL FAR PASCAL Jre2Close(LPJRE2 lpjreJre);


// GetProcAddressを使用する際のポインタ変数を宣言しやすくしておく.
#ifdef STRICT
typedef BOOL (FAR PASCAL * LPISMATCH) (LPSTR, LPSTR);
typedef int (FAR PASCAL * LPGLOBALREPLACE) (LPSTR, LPSTR, LPSTR, LPHANDLE);
typedef WORD (FAR PASCAL * LPJREGETVERSION) (VOID);
typedef HGLOBAL (FAR PASCAL * LPDECODEESCSEQALLOC2) (LPSTR);
typedef int (FAR PASCAL * LPGETJREMESSAGE) (int, int, LPSTR, int);
typedef BOOL (FAR PASCAL * LPJRE2OPEN) (LPJRE2);
typedef BOOL (FAR PASCAL * LPJRE2COMPILE) (LPJRE2, LPSTR);
typedef BOOL (FAR PASCAL * LPJRE2GETMATCHINFO) (LPJRE2, LPSTR);
typedef BOOL (FAR PASCAL * LPJRE2CLOSE) (LPJRE2);
#else
typedef FARPROC LPISMATCH;
typedef FARPROC LPGLOBALREPLACE;
typedef FARPROC LPJREGETVERSION;
typedef FARPROC LPDECODEESCSEQALLOC2;
typedef FARPROC LPGETJREMESSAGE;
typedef FARPROC LPJRE2OPEN;
typedef FARPROC LPJRE2COMPILE;
typedef FARPROC LPJRE2GETMATCHINFO;
typedef FARPROC LPJRE2CLOSE;
#endif




#endif
// end of jreusr.h
