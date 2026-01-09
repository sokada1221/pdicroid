#ifndef __PDCONFIG_H
#define	__PDCONFIG_H

// macro //
// OS_UTF8		: OSのnative char setがUTF-8である場合defined

//#define	EJUNIOR		/* for eijiro-junior */

//#define	__PROTO		// prototype version, remove this definition before final release.

#define	NEWDIC51
//#define	USE_ESTDB
//#define	USE_HBTREE
//#define	USE_EXTPDIC

#if defined(USE_ESTDB) || defined(USE_HBTREE) || defined(USE_EXTPDIC)
#define	USE_EXTDB
#endif

//#define	USE_FASTINCR	// uses fast all text incrimental search

#undef PDICW
#undef NEWDIC2
#undef NEWDIC3
#undef NEWDIC4
#undef ND3ONLY
#undef ND4ONLY
#undef MACCS
#undef USE_REG
#undef GUI
#undef	NOUSE_LABEL
#undef	NOCOLOR
#undef	SML
#undef	USE_PS

#define	NOUSEDLL

#ifndef DEPW
#define	PDICW
#endif

#ifndef USE_BACKUP
#define	USE_BACKUP			1
#endif

//#define	CUSTITEMNAME	// 項目名のcustomize

#define	MAX_WEBSRCHMENU		20		// ポップアップ検索時の最大メニュー数

#define	PRINTF		0

#ifdef UNICODE
// ----------- Unicode共通 ----------- //
#define	_tsize	2

// BOCUがdefault
#define	DIC_BOCU
#define	USE_BOCU1
#define	USE_BOCU
#undef	MIXMJ		// _mcharと_jcharの型が異なる場合
#define	KMIXDIC				// 見出語とsystem codeが異なる場合(MIXDICに近い意味)
#define	MIXDIC			1
#if !defined(DLL)
//#define	USE_JBMP
#endif

#define	NEWDIC2
#define	NEWDIC3
#define	ND3ONLY
#define	MACCS
#define	USE_REG
#define	NEWDIC4
#define	ND4ONLY
#define	USE_RANGECODEC
//#define	USE_ZLIBCOMP

#if defined(WINCE) || defined(CBUILDERDEBUG)

// ----------- Unicode/WinCE ----------- //
// ----------- Unicode/WinCE sim debug ----------- //
#define	GUI
#define	RO_COMP			1
#define	SUPPORT_LANG	1
#define	USE_DT2		0
#define	USE_DIRPOP	0
#define	NOUSE_LABEL
#define	SMALL
#define	NOCOLOR
#define	SML
#define	USE_PS

#elif defined(CMD32)
// ----------- Unicode/Command ----------- //
#ifdef CMD32U
// CMD32 only utility(optimization, check)
#else
#define	USE_JLINK
#define	USE_FILELINK
#endif
//#define	USE_UNIVDIC
#define	USE_COMP

#define	USE_REGEXPP		// Uses Regex++
//#define	USE_REXP
//#define	USE_JRE
//#define	USE_BREGONIG	// Uses bregonig

#define	USE_DT2		0
//#define	USE_WTEST
//#define	OLE2
//#define	USE_OLE
//#define	INC_OLE1
#define	USE_DDE
#define	USE_REG
#define	USE_PS

# define LANDIC			0
# define INETDIC		0

#define	EXTHEADER

#else	//

#ifdef DLL
// ----------- Unicode/DLL ----------- //
//#undef	NOUSEDLL

//#define	USE_JLINK	// not yet supported
//#define	USE_FILELINK	// not yet supported
//#define	USE_UNIVDIC		// これを有効にすると不可思議な動作になってしまう？？？
#define	USE_COMP	// not yet supported

#define	USE_DT2		0
//#define	USE_WTEST
//#define	USE_REXP	// not yet supported
//#define	USE_JRE		// not yet supported
//#define	GUI
//#define	OLE2
//#define	USE_OLE
//#define	INC_OLE1
//#define	USE_DDE
#define	USE_REG
//#define	USE_PS

# define LANDIC			0
#ifndef INETDIC
# define INETDIC		0
#endif

#define	EXTHEADER

#elif defined(UNIX)
// ----------- Unicode/UNIX,Android ----------- //
//#define	USE_JLINK
//#define	USE_FILELINK
//#define	USE_UNIVDIC
#define	USE_COMP

//#define	USE_REGEXPP		// Uses Regex++
//#define	USE_REXP
//#define	USE_JRE
//#define	USE_BREGONIG	// Uses bregonig

#define	DISPDICNAME	// 辞書ファイル名表示

#define	USE_DT2		0
//#define	USE_WTEST
//#define	OLE2
//#define	USE_OLE
//#define	INC_OLE1
#define	INCOLE2			0	// OLE2 is a standard function
//#define	USE_DDE
//#define	USE_REG
//#define	USE_PS
//#define	USE_POPUPHOOK
//#define	USE_TTS

//#define	POPEDIT_RICHEDIT	/* TPopupEditはRichEdit control */

# define LANDIC			0
# define INETDIC		0	// 当面はinetdic non-support

#define	EXTHEADER

//#define	GUI
//#define	EPWING

#else	// !UNIX
// ----------- Unicode/Win32 ----------- //
#define	USE_JLINK
#define	USE_FILELINK
//#define	USE_UNIVDIC
#define	USE_COMP

//#define	USE_REGEXPP		// Uses Regex++
//#define	USE_REXP
//#define	USE_JRE
#define	USE_BREGONIG	// Uses bregonig

#define	DISPDICNAME	// 辞書ファイル名表示

#define	USE_DT2		1
#define	USE_WTEST
#define	OLE2
#define	USE_OLE
//#define	INC_OLE1
#define	INCOLE2			1	// OLE2は標準機能 
#define	USE_DDE
#define	USE_REG
#define	USE_PS
#define	USE_POPUPHOOK
#define	USE_TTS

#define	POPEDIT_RICHEDIT	/* TPopupEditはRichEdit control */

# define LANDIC			0
# define INETDIC		1

#define	EXTHEADER

#ifndef DIC_TEST	// If not test/dictest program.
#define	GUI
#define	EPWING
#endif

#endif	// !DLL
#endif	// WINCE

#else	// !UNICODE

// ----------- non-Unicode共通 ----------- //
#define	_tsize	1

#if defined(MIXDIC) || (defined(DIC_UTF8) && !defined(OS_UTF8))
	// UTF-8である場合
#  define	MIXDIC			1
#  define	NEWDIC2
#  define	NEWDIC3
#  define	ND3ONLY
#else	// !MIXDIC
	// non-UTF-8
#endif	// !MIXDIC

#ifdef CMD32

// ----------- PDOUT32 ----------- //
#define	NEWDIC2
#define	NEWDIC3
#define	MACCS

#define	USE_JLINK
#define	USE_FILELINK
#define	USE_UNIVDIC
#define	USE_COMP

#define	USE_DT2		0
//#define	USE_WTEST
//#define	USE_REXP
//#define	USE_VWXW
#define	USE_JRE
//#define	GUI
//#define	OLE2
//#define	USE_OLE
//#define	INC_OLE1
//#define	USE_DDE
//#define	USE_REG
#define	USE_PS		0

#if !defined(SML)
#define	INIT_INI2REG
#endif

#else	// !CMD32

#ifdef _Windows

// ----------- PDIC for Win32/DLL ----------- //
#ifdef DLL

#undef	NOUSEDLL

#define	NEWDIC2
#define	NEWDIC3
#define	MACCS
#define	USE_JLINK
#define	USE_FILELINK
#define	USE_COMP

#define	USE_DT2		1
//#define	USE_WTEST
//#define	USE_REXP
//#define	USE_VWXW
#define	USE_JRE
//#define	OLE2
//#define	USE_OLE
//#define	INC_OLE1
//#define	USE_DDE
#define	USE_REG
#define	USE_PS		0

#if !defined(SML)
#define	INIT_INI2REG
#endif

#else	// !DLL
#ifdef LIGHT
// ----------- PDIC SuperLight ----------- //
#define	SML

#define	NEWDIC2
#define	NEWDIC3
#define	MACCS
//#define	USE_JLINK
#define	USE_COMP
#define ND3ONLY

#define	USE_DT2		1
#define	USE_WTEST
//#define	USE_REXP
//#define	USE_VWXW
#define	USE_JRE
#define	GUI
//#define	OLE2
//#define	USE_OLE
//#define	INC_OLE1
//#define	USE_DDE
#define	USE_REG
#define	USE_PS		1

#else	// !LIGHT
// ----------- PDIC for Win32 ----------- //
#define	NEWDIC2
#define	NEWDIC3
#define	MACCS
#define	USE_JLINK
#define	USE_FILELINK
#define	USE_COMP

#define	USE_DT2		1
#define	USE_WTEST
//#define	USE_REXP
//#define	USE_VWXW
#define	USE_JRE
#define	GUI
#define	OLE2
#define	USE_OLE
#define	INC_OLE1
#define	USE_DDE
#define	USE_REG
#define	USE_PS		1
#define	USE_POPUPHOOK
#define	USE_TTS
#define	DISPDICNAME	// 辞書名を表示の設定を使用する

#endif	// !LIGHT

#if !defined(SML)
#define	INIT_INI2REG
#endif
#endif	// !DLL

#else	// !_Windows

#error	// unknown OS

#endif	// !_Windows

#endif	// !CMD32

#endif	// !UNICODE


#if (MIXDIC && !defined(DIC_UTF8))
//#define	USE_SINGLEBYTE
#endif

// Japa classの文字列に混在文字種を使用するか？
#if MIXDIC && !defined(DIC_UTF8) && !defined(USE_BOCU)
#define	MIXJAPA
#define	_jMixChar	TNMixChar
#else
#define	_jMixChar	tnstr
#endif

#ifdef SMALL
#undef SML
#define	SML
#endif

#ifdef __ANDROID__
#define	SQUFONT		0
#else
#define	SQUFONT		1
#endif
//#define	WORDFONT	0	// no longer used

// Fontを使用するかどうか
#ifdef SMALL
#define	WITHPSFONT	1	// 本体のフォント設定とポップアップ検索ウィンドウのフォントを一緒に設定
#else
#define	WITHPSFONT	0
#endif

// 特殊文字入力を使用するか？
#ifdef SML
#define	USE_SPECIALCHAR	0
#else
#define	USE_SPECIALCHAR	0
#endif

// View //
#ifdef SMALL
#define	VIEWCLICKONLY	1	// 本体ではclickのみのpopupか？
#define	NEWHYPLINK		1	// hyper linkなどで、underline表示はしない
#else
#define	VIEWCLICKONLY	1
#define	NEWHYPLINK		1	// hyper linkなどで、underline表示はしない
#endif

// 履歴
#ifdef LIGHT
#define	USE_HISTORY		0
#else
#define	USE_HISTORY		1
#endif

// 絞り込み検索 or not
#define	NEWINCSRCH		1

// ポップアップ＆リンク検索
#if !defined(SML)
#define	NEWAUTOLINK		1
#else
#define	NEWAUTOLINK		0
#endif

// ラベリング
#if !defined(LIGHT)
#define	INTLABEL		1		// labellingは内部機能で？
#define	NEWLABEL		1		// DEPW.DLLでラベリング編集
#else
#define	INTLABEL		1		// labellingは内部機能で？
#define	NEWLABEL		0
#endif

// 辞書グループ
#ifdef SML
#define	DICGRPBTN		0		// 辞書グループボタンを使わない
#else
#define	DICGRPBTN		1		// 辞書グループボタンを使う
#endif

#ifdef SMALL
#define	DICGRPMENU		1		// 辞書グループ切り替えメニューを使う
#else
#define	DICGRPMENU		0		// 辞書グループ切り替えメニューを使わない
#endif

// 単語テスト //
#ifdef SML
#define	WTESTALTVIEW	0		// 単語テスト中も本体参照可能に
#else
#define	WTESTALTVIEW	0		// 単語テスト中も本体参照可能に
#endif

// Net Dic //
#ifndef INETDIC
# if defined(SML)
# define LANDIC			0
# define INETDIC		0
# else
# define LANDIC			0
# define INETDIC		0
# endif
#endif	/* INETDIC */

#undef NETDIC
#if LANDIC || INETDIC
#define	NETDIC			1
#else
#define	NETDIC			0
#endif

#ifndef INCOLE2
#if !defined(PDC600)
#define	INCOLE2			1	// OLE2は標準機能 
#else
#define	INCOLE2			0
#endif
#endif	// ndef INCOLE2

/* Font settings */
#ifdef SMALL
#define	USE_DEFFONTSIZE		0	// 入力時default font sizeを利用する・しない
#else	// !SMALL
#define	USE_DEFFONTSIZE		1	// 入力時default font sizeを利用する・しない
#endif	// !SMALL

/* Incremental Search plus */
#if defined(SML)
#define	INCSRCHPLUS		0
#else
#define	INCSRCHPLUS		0
#endif

/* View auto popup */
#if defined(SML) && (!defined(WINCE) || defined(WINPPC) || defined(PKTPC))
#define	USE_VIEWAUTOPOP	0
#else
#define	USE_VIEWAUTOPOP	0
#endif

/* Auto Search via DrawClipboard */
#if !defined(SMALL) /*|| !defined(WINCE) || defined(PKTPC)*/
#define	USE_DRAWCLIP	1
#else
#define	USE_DRAWCLIP	0
#endif

/* Web search */
#if !defined(WINCE) && !(defined(SML))
#define	USE_WEBSEARCH	1
#endif

/* popup search windowの強調表示 */
#if defined(SMALL)
//#define	POPEDIT_RICHEDIT	/* TPopupEditはRichEdit control */
#endif

#if defined(PDICW) && !defined(SMALL)
//#define	POPEDIT_RICHEDIT	/* TPopupEditはRichEdit control */
#endif

#ifdef CBUILDERDEBUG
#define	POPEDIT_RICHEDIT	/* TPopupEditはRichEdit control */
#endif

/* File History 関連 */
#ifndef LIGHT
#define	USE_FILEHISTORY		/* popup search windowでfile history menu */
#define	MAX_FILEHISTORY	9	/* 最大のファイル履歴数 */
#endif

/* MarkHistory関連 */
#if defined(POPEDIT_RICHEDIT) && !defined(SML)
#define	USE_MARKHIST
#define	USE_MARKNWORDS
#endif

/* Unicode関連の定義 */

/* Delayed Delete File */
#ifndef SMALL
#define	USE_DELAYED_DELFILE
#endif

/* Debug */
#define	DICDBG1			0
	/* 英辞郎辞書変換debug */

#if DICDBG1
#define	DICDBG			1
#else
#define	DICDBG			0
#endif

#ifdef _MSC_VER
#define	TO_BOOL(x)		((x)?true:false)
#else
#define	TO_BOOL(x)		(x)
#endif

void DBW(const char *, ...);
#ifdef _Windows
class __mtimes {
protected:
	LARGE_INTEGER totaltime;
	LARGE_INTEGER starttime;
	const char *label;
	int counter;
public:
	__mtimes( const char *_label )
		{ label = _label; counter = 0; totaltime.QuadPart = 0; start(); }
	~__mtimes()
		{ show(); }
	void start()
		{
			QueryPerformanceCounter( &starttime );
			counter++;
		}
	void stop()
		{
			LARGE_INTEGER t;
			QueryPerformanceCounter( &t );
			totaltime.QuadPart += t.QuadPart - starttime.QuadPart;
		}
	void show()
		{
			LARGE_INTEGER freq;
			QueryPerformanceFrequency( &freq );
			int r = totaltime.QuadPart * 1000000 / freq.QuadPart;
			DBW("%s:%d.%06d %d",label, r/1000000, r%1000000, counter );
		}
	void clear()
		{
			totaltime.QuadPart = 0;
			counter = 0;
		}
};
#endif

//#define	USE_MTIMES
#if (defined(_Windows) || defined(_WINDOWS_)) && !defined(WINCE) && defined(__cplusplus) && defined(USE_MTIMES)
#define	_mtimes(name,str)	__mtimes name(str)
#define	ex_mtimes(name)	extern __mtimes name
#define	_mtimes_start(name)	name.start()
#define	_mtimes_stop(name)	name.stop()
#else
#define	_mtimes(name,str)
#define	ex_mtimes(name)
#define	_mtimes_start(name)
#define	_mtimes_stop(name)
#endif

// popup検索 debug用 macro //
#if 0
#define	DBP		DBW
#define	DEBUG_POPUP
#else
#define	DBP		(void)
#endif

#define	USE_BM		0	// use bookmark

#endif	/* __PDCONFIG_H */

