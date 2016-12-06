#ifndef	__REXPDLL_H
#define	__REXPDLL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include	"rexp.h"

//---------------------------------------------------------//
int FAR PASCAL ReComp( LPCTSTR pat );
typedef int (FAR PASCAL *LPRECOMP)( LPCTSTR );
//---------------------------------------------------------//
//	正規表現のコンパイル
//
//	pat : 正規表現(NULLであると、前にコンパイルしたものを使用）
//	戻り値
//	NULL : 正常終了
//	それ以外 : エラーメッセージへのポインタ

int FAR PASCAL ReCompv( unsigned char FAR ** patv );
VOID FAR PASCAL ReInitPattern(VOID);
unsigned FAR PASCAL ReAddPattern( LPCTSTR pat );
VOID FAR PASCAL ReEndPattern(VOID);


//---------------------------------------------------------//
int FAR PASCAL ReExec( LPCTSTR str);
typedef int (FAR PASCAL *LPREEXEC)( LPCTSTR );
//---------------------------------------------------------//
//	照合
//
//	str : 照合文字列
//
//	0 : アンマッチ
//	0以外　: 正規表現リストの番号のビット

//---------------------------------------------------------//
VOID FAR PASCAL ReIgnoreCase( int value);
typedef VOID (FAR PASCAL *LPREIGNORECASE)( int );
//---------------------------------------------------------//
//	半角英字の大文字、小文字の同一視
//	0 : 区別する
//	1 : 同一視


//---------------------------------------------------------//
int FAR PASCAL ReErrno(VOID);
typedef int (FAR PASCAL *LPREERRNO)(VOID);
//---------------------------------------------------------//
//	拡張エラー番号の取得

/**** エラー番号 ******************************************
        　RE_ERR_NONE           エラーはない

        　以下 re_comp() 時

        　RE_ERR_CCL_NUM        文字クラスのバッファが溢れた
        　RE_ERR_CCL_TERM       文字クラス指定の終端文字が見つからない
        　RE_ERR_MEMORY         メモリが足りない
        　RE_ERR_PAR            括弧の対応がおかしい
        　RE_ERR_SYNTAX         構文エラー

        　以下 re_exec() 時

        　RE_ERR_TABLE          NFA -> DFA 変換ワークテーブルが溢れた
        　RE_ERR_DFA_STAT       DFA の状態が多すぎてバッファが溢れた
***********************************************************/

#ifdef	__cplusplus
};
#endif

#endif	// __REXPDLL_H
