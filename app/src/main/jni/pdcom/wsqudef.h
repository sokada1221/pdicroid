#ifndef __WSQUDEF_H
#define	__WSQUDEF_H

// Configration Switches //
#ifdef SMALL
#define	USE_SLASH	0		// 日本語訳と用例の区切りに " / "を使用するかどうか
#else
#define	USE_SLASH	1		// 日本語訳と用例の区切りに " / "を使用するかどうか
#endif

#define	N_SECTION	6		// 表示のセクション数

#define	SN_NUMBER	0
#define	SN_ATTR		1
#define	SN_WORD		2
#define	SN_PRON		3
#define	SN_LEVEL	4
#define	SN_JAPA		5
#define	SN_EXP		5			// 注意
#define	SN_EXP1		6
#define	SN_EPWING	7			// フォント処理でのみ使用
#define	SN_POPUP	8			// フォント処理でのみ使用

#define	SN_AUDIO	9			// for audio file and voice object

#define	SNF_NUMBER	1
#define	SNF_ATTR	2
#define	SNF_WORD	4
#define	SNF_PRON	8
#define	SNF_LEVEL	0x10
#define	SNF_JAPA	0x20
#define	SNF_EXP		0x20		// 注意
#define	SNF_EXP1	0x40
#define	SNF_EPWING	0x80		// ﾌｫﾝﾄ処理でのみ使用
#define	SNF_POPUP	0x100		// popup search window(フォント処理でのみ使用）

#define	SN_OBJECT	0x10		// オブジェクト(SN_OBJECT+...)

#define	SN_UNKNOWN	0x0f		// ヒットテスト用
								// 単語,訳などのある行にヒットしたが、
								// 文字にはヒットしていない

#define	SNFONT_NUM		6

#define	SNFONT_COMMON	0		// 共通（番号・属性、タイトル、ファイル名）
#define	SNFONT_WORD		1		// 見出語
#define	SNFONT_PRON		2		// 発音記号
#define	SNFONT_JAPA		3		// 訳語
#define	SNFONT_EXP		4		// 用例
#define	SNFONT_EPWING	5		// EPWING / ポップアップ検索ウインドウ

// 表示幅属性の属性フラグ
#define	WA_VARWIDTH		1		// 幅を可変出来る
#define	WA_VARFONT		0x10	// フォントを変えられる
#define	WA_MENU			0xf0	// メニューを持てる(マスクとして利用）
#define	WA_VIEW			2		// 表示／非表示
#define	WA_PUSH			4		// プッシュできる

#define	MAX_ITEMNAME	32

#define	DEF_LINKCOLOR	RGB(0,128,0)

#define	RGB_BLACK	RGB( 0, 0, 0 )

int sn2type( int sn );
int sn2snfont(int sn);	// SN_xxx -> SNFONT_xxx

#endif	// __WSQUDEF_H

