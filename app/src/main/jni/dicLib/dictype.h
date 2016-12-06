#ifndef __DICTYPE_H
#define	__DICTYPE_H

typedef int t_dictype;

#define	DT_PDIC		0
#define	DT_PDICTEXT	1
#define	DT_WX2TEXT	2
#define	DT_LEVEL	3
#define	DT_CSV		4
#define	DT_EXTPDICTEXT	5	// 拡張PDICテキスト形式（＝１行テキスト形式）
#define	DT_FENG5	6
#define	DT_EPWING	7
#define	DT_USERTEMP	8	// ユーザ定義テンプレート
#define	DT_USER		9	// ﾕｰｻﾞ定義ファイル
#define	DT_TEXT		10	// 通常のテキストファイル
#define	DT_INF		11	// スクリプトファイル
#define	DT_DLL		12
#define	DT_LABEL	13
#define	DT_EXE		14
#define	DT_ALL		15
#define	DT_WAV		16
#define	DT_INI		17
#define	DT_PDIC_OLD_ANS	18
#define	DT_PDIC_OLD_UNI	19
#define	DT_TSV		20
#define	DT_XML		21
#define	DT_QDBM_DEPOT	22
#define	DT_QDBM_VILLA	23
#define	DT_BOOKMARK	24
#define	DT_EXTPDICTEXT2	25	// １行テキスト形式＋Japaが空の場合はwordのみ
#define	DT_PDI		26		// .pdi file

//#define	N_CONVDICS	7	// コンバート対象の辞書の種類の数

int GetDicFileType( const tchar *filename );
int GetDicFileTypeEx( const tchar *filename, int &dictype_ex );

#endif	// __DICTYPE_H
