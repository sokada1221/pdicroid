#ifndef	__WINMSG_H
#define	__WINMSG_H

#include "tid.h"

#define	MsgError	_LT(TID_ERROR)	// エラー
#define	MsgDicError	_LT(TID_DIC_ERROR)	// 辞書エラー
#define	MsgFatal	_LT(TID_Fatal)	// 致命的エラー

//extern const tchar *StrCheckDictionary;	// 辞書のチェック
#define	StrWords	_LT(TID_Words)	// 語
#define	StrChars	_LT(TID_Chars)	// 文字
#define	StrBytes	_LT(TID_Bytes)	// バイト
#define	StrMemory	_LT(TID_Memory)	// 暗
#define	StrJEdit	_LT(TID_JEdit)	// 修
#define	StrBlock	_LT(TID_Block)	// ブロック

#define	sAllSearch		_LT(CAP_ALLSEARCH)

#ifdef WINCE
#define	STR_MYDOCUMENTS	"My Documents"
#define	PATH_MYDOCUMENTS "\\My Documents"
#define	PATH_MEMORYCARD		"\\ﾒﾓﾘ ｶｰﾄﾞ"
#endif

class Pdic;

void ErrorMessage( HWND hwnd, int msgno, const tchar *str=NULL, const tchar *_title=0 );
tnstr GetErrorMessage( int msgno, const tchar *str=NULL );
BOOL DicErrorMessage( HWND hwnd, Pdic *dic, int msgno, const tchar *str=NULL, const tchar *_title=0 );
void DicErrorMessage( HWND hwnd );	// 現在オープン中の辞書グループのエラーメッセージ
int MessageDialog( HWND hwnd, LPCTSTR dlgname, const tchar *text=NULL );
const tchar *GetStateMessage( int msgno );
const tchar *GetWinErrorMessage( int error );
tnstr GetStringParam(int id, ...);

//int MsgBox(HWND hwnd, int text, int caption, UINT type);

extern const int StrColorNames[];

#endif	// __WINMSG_H

