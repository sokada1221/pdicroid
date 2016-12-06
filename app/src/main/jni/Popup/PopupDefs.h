//---------------------------------------------------------------------------

#ifndef PopupDefsH
#define PopupDefsH
//---------------------------------------------------------------------------

#include "pdconfig.h"

#define	PSIMOPEN	1		// 検索開始と同時にpopup windowを開く

#ifdef SMALL
#define	ONLYCLICK	1
#else
#define	ONLYCLICK	0
#endif

// 英国式<->米国式スペル変換をするか？(dialog boxで)
#if defined(WINCE) || defined(UNICODE) || defined(SML)
#define	USE_UKSPELL	0
#else
#define	USE_UKSPELL	1
#endif

// Configuration Switches //
#define	MAX_MENUNUMWORD	20	// メニュー一覧表示時の最大数

#ifdef SMALL
#define	PSMENUBTN	0
#define	PSMENUBTN2	1	// [↓]メニューボタン
#define	PSMENUBTN3	1	// [→]次候補ボタン
#define	CTRLCLOSE	0
#define	PSDRAGMOVE	1
#else
#define	PSMENUBTN	0
#define	PSMENUBTN2	0	// [↓]メニューボタン
#define	PSMENUBTN3	0	// [→]次候補ボタン
#define	CTRLCLOSE	1
#define	PSDRAGMOVE	1
#endif

#ifndef WINCE
#define	PSSIZING	1
#define	NOHITLIST	0	// ヒットしないときの単語一覧
#else	// WINCE
#define	PSSIZING	0	// WinCEでは,WM_SIZING messageがない？
#define	NOHITLIST	0	// ヒットしないときの単語一覧
#endif	// WINCE

#define	PSNEWSCR	1	// 新しいscroll barを用いる

#define	UMP_POPUP_CREATED		0xF0	// TMenuWindow classでpopup windowがcreateされた。sender:TPopupWindow*
#define	UMP_POPUP_DELETED		0xF1	// Objectがdeleteされた
#define	UMP_POPUP_GETATTR		0x1F	// 属性の取得

//#include "draw4.h"

// PSF_xxx と共通
#define	MWS_NOACTIVATE			0x00001000	// WindowをActivateしない
#define	MWS_CLICKPOPUP			0x00002000	// リンクはクリックでポップアップ
#define	MWS_MULTIWORD			0x00004000	// 複数単語popup
#define	MWS_NOSEARCHMENU		0x00008000	// popupmenuのCM_SWORD,CM_SJAPAはなし
#define	MWS_NOCONFIGMENU		0x00010000	// popupmenuのCM_PSCONFIGはなし
#define	MWS_CTRLCLOSE			0x00020000	// Ctrl key を離すとClose

#define	MWS_INACTIVECLOSE		0x0001	// 非アクティブ化されたときはクローズする(default)

#define	MWS_CAPTURE				0x0002	// マウスをキャプチャーして、メニュー外に移動してもｸﾛｰｽﾞしないようにする
#define	MWS_CLICKCLOSE			0x0004	// このウィンドウをマウスでクリックされたらクローズする
#define	MWS_PASSMOUSEMOVE		0x0008	// マウスの移動メッセージを親ウィンドウに渡す
#define	MWS_CLOSEANYEVENTS		0x0010	// キーのイベントが発生するとcloseする
#define	MWS_USEHYPERLINK		0x0020	// HyperLinkを検索し、有効にする(PopupWindow classのみ)
#define	MWS_USEHOLDER			0x0100
#define	MWS_PARENTMENU			0x0200	// 親windowはMenuWindowだよー
//#define	MWS_MULTIHIT			0x0400	// 複数の候補がある
#define	MWS_DISABLE_POPUPWIN	0x0800	// ”ポップアップ検索ウィンドウへ”はなし（＝PSWからのpopup）

#define	MWS_FIXEDSIZE			0x00040000	// ポップアップウィンドウは固定サイズ（Setup()によるサイズ調整はしない）

#define	MWS_LINKOBJECT			0x00100000	// 登録単語のポップアップではなく、fileなどのリンクオブジェクトの表示である
											// ポップアップメニューに影響
#ifndef SMALL
#define	MWS_DRAGMOVE			0x00200000	// Move window by mouse dragging
#define	MWS_FIXEDMODE			0x00400000	// Fixed mode
#endif
#define	MWS_MENUWINDOW			0x00800000	// (内部で使用) - MenuWindowであることを示す
#define	MWS_POPUPWINDOW			0x01000000	// (内部で使用) - PopupWindowであることを示す


#define	PC_WORD				0x0001
#define	PC_PRON				0x0002
#define	PC_JAPA				0x0004
#define	PC_EXP				0x0008
#define	PC_OBJECT			0x0010
#define	PC_BOTH				(PC_WORD|PC_JAPA)
#define	PC_ALL				(PC_WORD|PC_JAPA|PC_PRON|PC_EXP|PC_OBJECT)

#define	MWA_NONE			0x0000
#define	MWA_CHANGEWIN_FOR	0x0001
#define	MWA_CHANGEWIN_BAK	0x0002
#define	MWA_REFRESH			0x0003
#define	MWA_CHANGEWORD		0x0004

#define	CM_MENUBUTTON		3100	// Menu Button
#define	CM_NEXTBUTTON		3101	// Next Button

// SQM_xxxと同じ値
#define	PSF_SHOWJAPA		0x0001
#define	PSF_SHOWEXP			0x0002
#define	PSF_SHOWLEVEL		0x0004
#define	PSF_SHOWPRON		0x0008	// 発音記号を表示する
#define	PSF_SHOWOBJECT		0x0400	// SQM_に無いが・・・
// #define	PSF_SHOWWORD	0x0010	// reserved
// #define	PSF_TITLE		0x0020	// reserved
// #define	PSF_FILENAME	0x0040	// reserved
//#define	PSF_SHOWEPWING		0x0100
// #define	PSF_SHOWATTR	0x0204
#define	PSF_SHOWDICNAME		0x0800
#define	PSF_SHOWMASK		(PSF_SHOWJAPA|PSF_SHOWLEVEL|PSF_SHOWPRON|PSF_SHOWEXP|/*PSF_SHOWEPWING|*/PSF_SHOWOBJECT|PSF_SHOWDICNAME|PSF_PLAY|PSF_TTSPLAY)

// menuwin.h の MWS_と同じ値
#define	PSF_NOACTIVATE		0x00001000	// ポップアップウィンドウをactivateしない
#define	PSF_CLICKPOPUP		0x00002000	// 強制的にMWS_CLICKPOPUPにする
#define	PSF_MULTIWORD		0x00004000	// 複数語のポップアップ
#define	PSF_NOSEARCHMENU	0x00008000	// popupmenuのCM_SWORD,CM_SJAPAはなし
#define	PSF_NOCONFIGMENU	0x00010000	// popupmenuのCM_PSCONFIGはなし
#define	PSF_CTRLCLOSE		0x00020000	// Ctrl を離すとClose

//#define	PSF_FIXEDSIZE		0x40000	// window sizeは固定

// Popup Window classに渡すflag (でよい? 2009.2.4)
#define	PSF_COMMASK			(PSF_NOACTIVATE|PSF_CLICKPOPUP|PSF_MULTIWORD|PSF_NOSEARCHMENU|PSF_NOCONFIGMENU|PSF_CTRLCLOSE/*|PSF_FIXEDSIZE*/|PSF_ADDHIST)

#define	PSF_QUICKPOPUP		0x000100000	// クイックポップアップか？
#define	PSF_FORCEMENU		0x000200000	// 強制的にメニュー形式で表示する
#define	PSF_CLICKSEARCH		0x000400000	// クリックによる検索
#define	PSF_PLAY			0x000800000	// 音声再生
#define	PSF_ONLYHITWORD		0x001000000	// ヒットした単語のみ
										// 親ウィンドウがdeactivate時にcloseする必要がある
//#define	PSF_NOMAPPING		0x2000000
#define	PSF_NOHITREC		0x004000000	// ヒットしない場合は単語登録へ
#define	PSF_POPUPWINDOW		0x008000000	// ポップアップ検索ウィンドウからの検索

#define	PSF_ADDHIST			0x010000000	// close時履歴に保存する
#define	PSF_TTSPLAY			0x020000000	// 人工音声再生

#define	DEF_PS_ALPHA_VALUE	0xF4		// Default popup window alpha value.

// Menu Command //
#define	PSC_CALLPROC		0x80000000u	// procedureによる呼び出し
#define	PSC_CTRL			0x100
#define	PSC_SHIFT			0x200
#define	PSC_FORCE			0x800		// 強制的にそのもののキーとして扱う
#define	DEF_PSCMD(cmd)		(PSC_CALLPROC | (cmd))

#define	PSC_LEFTCLICK		0x1000		// 左クリックボタン設定有り (for Ctrl+G)

enum {
	// for TPopupBaseWindowClient class
	PUF_GetHandle,
	PUF_SetPosition,
	PUF_SetSize,
	PUF_Show,
	PUF_ShowNoActivate,
	PUF_IsWindow,
	PUF_IsVisible,
	PUF_Close,
	PUF_PostClose,
	PUF_GetModalResult,
	PUF_SetModalResult,
	PUF_GetStyle,
	PUF_SetStyle,
	PUF_GetViewFlag,
	PUF_SetViewFlag,
	PUF_GetOwner,
	PUF_SetOwner,
	PUF_GetPS,
	PUF_SetPS,
	PUF_SetLastAction,
	PUF_GetReceiver,
	PUF_SetReceiver,
	PUF_GetParent,
	PUF_EvKeyDown,
	PUF_EvKeyUp,
	PUF_EvChar,
	PUF_SetCallback,

	// for TPopupWindow class
	PUF_CreatePopupWindow,
	PUF_GetTitle,
	PUF_SetTitle,
	PUF_SetMemory,
	PUF_Copy,
	PUF_SearchAutoLink,
	PUF_SetJapa,
	PUF_GetChildPS,

	// for TMenuWindow class
	PUF_CreateMenuWindow,
	PUF_AppendMenu,
	PUF_GetMenuItemNum,
};

#endif

