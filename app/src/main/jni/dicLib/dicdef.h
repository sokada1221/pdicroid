#ifndef	__DICDEF_H
#define	__DICDEF_H

#include "pdconfig.h"

// NEWDIC5/NEWDIC51専用
// UNICODEはBOCU専用

// UNICODE,ANSI混合tricky辞書
#ifndef MIXDIC
#define	MIXDIC	0
#endif

#ifndef USE_ODA
#define	USE_ODA	1
#endif

#ifndef USE_MMI
#define	USE_MMI	0
#endif

#ifndef DICMT
#define	DICMT	1	// multi thread available
#endif

#ifdef DLL
#define	DICTXDUP	0	// text duplication dictionary
#else
#define	DICTXDUP	1	// text duplication dictionary
#endif

#define	NEWDIC
#define	NEWDIC2
#define	NEWDIC3
#define	NEWDIC4
#define	ND4ONLY

#if !defined(_UNICODE) && MIXDIC
#error MIXDIC can be used when _UNICODE.
#endif

#ifdef CPBTEST	// for C++Builder test
#define	CMD32
#define	NOUSEDLL
#endif

#if defined(UNICODE)
//#define	NEWDIC4UNI
#endif

#ifdef ANSI
#undef __WIN32__
#endif

// compressed dictionary can be handled with read-only
#ifndef RO_COMP
#define	RO_COMP	0
#endif

// Alignment
#if defined(WINCE)
#define	NEEDALIGN	1
#else
#define	NEEDALIGN	0
#endif

#ifdef _Windows
#define	MEMMAPDIC	0
//#define	MEMMAPDIC	1		// 数十MBを超えるとassignできなくなる？？
#else
#define	MEMMAPDIC	0
#endif

// 64 bit support //
#if 0
typedef __int64 __pdc64;
#else
typedef int __pdc64;
#endif

#define	MAKE64(hi, lo)		((((__pdc64)(hi))<<32) | (lo))

/************************************************************/
/*	On Demand Access Revision 0.1							*/
#ifndef DIC_ODA
#define	DIC_ODA		0	// default=off
#endif

/************************************************************/
/*	Memory Mapped Index										*/
#ifndef DIC_MMI
#define	DIC_MMI		0	// default=off
#endif

/************************************************************/
/*	Multi-Access Revision 0.1								*/
//		通常の処理が行えるのは１つだけ
//		それ以外はヘッダーの読取りのみ

//注意：NETWORKを定義してもMACCSは有効になりません。
//		マルチタスク環境で、なおかつネットワークにつなぐ場合は、
//		両方定義してください。

#if	defined(USE_REXP) || defined(USE_JRE) || defined(USE_VWX) || defined(USE_REGEXPP) || defined(USE_BREGONIG)
#define	USE_REGEXP
#endif

// 再検索方法の指定
//	No_Markを定義すると旧方式のマーク式はしない。
//	再検索はbsearchによって行われる

#ifdef	No_Mark
#define	USE_MARK	0
#else
#define	USE_MARK	0	//1
#endif

#include	"defs.h"

#ifdef SWAP_ENDIAN
#define	DE_SWAP_SHORT(x)	( ((unsigned short)((x)<<8)) | ((unsigned char)((x)>>8)) )
#define	DE_SWAP_LONG(x)		( DE_SWAP_SHORT(x) | DE_SWAP_SHORT((x)>>16) )
inline short DE_SWAP( short x ) { return (short)DE_SWAP_SHORT( x ); }
inline long DE_SWAP( long x ) { return DE_SWAP_LONG( x ); }
inline unsigned short DE_SWAP( unsigned short x ) { return (unsigned short)DE_SWAP_SHORT( x ); }
inline unsigned long DE_SWAP( unsigned long x ) { return (unsigned long)DE_SWAP_LONG( x ); }
inline char DE_SWAP( char x ) { return x; }
inline unsigned char DE_SWAP( unsigned char x ) { return x; }
#define	SWAP_SELF(var)		var = DE_SWAP(var)
#else
#define	DE_SWAP_SHORT(x)	(x)
#define	DE_SWAP_LONG(x)		(x)
#define	DE_SWAP(x)			(x)
#define	SWAP_SELF(var)
#endif

/************************************************************/
// Alignment countermeasurement
#if defined(NEEDALIGN)
#ifdef SWAP_ENDIAN
inline unsigned short _alGetUShort( const unsigned short *sp )
	{ return (((unsigned short)((unsigned char*)sp)[0])<<8) | ((unsigned char*)sp)[1];}
inline unsigned long _alGetULong( const unsigned long *sp )
	{ return (((unsigned long)((unsigned char*)sp)[0])<<24) | (((unsigned long)((unsigned char*)sp)[1])<<16)
			| (((unsigned long)((unsigned char*)sp)[2])<<8) | (((unsigned long)((unsigned char*)sp)[3])) ;}
inline void _alSetUShort( unsigned short *dp, unsigned short v )
	{ ((unsigned char*)dp)[0] = (unsigned char)(v>>8); ((unsigned char*)dp)[1] = (unsigned char)v; }
inline void _alSetULong( unsigned long *dp, unsigned long v )
	{
		((unsigned char*)dp)[0] = (unsigned char)(v>>24); ((unsigned char*)dp)[1] = (unsigned char)(v>>16);
		((unsigned char*)dp)[2] = (unsigned char)(v>>16); ((unsigned char*)dp)[3] = (unsigned char)v;
	}
#else
inline unsigned short _alGetUShort( const unsigned short *sp )
	{ return *(unsigned char*)sp | (((unsigned short)(((unsigned char*)sp)[1]))<<8);}
inline unsigned int _alGetULong( const unsigned int *sp )
	{ return (((unsigned int)(((unsigned char*)sp)[3]))<<24) | (((unsigned int)(((unsigned char*)sp)[2]))<<16)
			| (((unsigned int)(((unsigned char*)sp)[1]))<<8) | (((unsigned int)(((unsigned char*)sp)[0])));}
inline void _alSetUShort( unsigned short *dp, unsigned short v )
	{ ((unsigned char*)dp)[0] = (unsigned char)v; ((unsigned char*)dp)[1] = (unsigned char)(v>>8); }
inline void _alSetULong( unsigned int *dp, unsigned int v )
	{
		((unsigned char*)dp)[0] = (unsigned char)v; ((unsigned char*)dp)[1] = (unsigned char)(v>>8);
		((unsigned char*)dp)[2] = (unsigned char)(v>>16); ((unsigned char*)dp)[3] = (unsigned char)(v>>24);
	}
#endif
#else	// !defined(NEEDALIGN)
inline unsigned short _alGetUShort( const unsigned short *sp )
	{ return *sp; }
inline unsigned int _alGetUShort( const unsigned int *sp )
	{ return *sp; }
inline void _alSetUShort( unsigned short *dp, unsigned short v )
	{ *dp = v; }
inline void _alSetULong( unsigned int *dp, unsigned int v )
	{ *dp = v; }
#endif

#ifdef	_Windows
#define	MAXDICNAME	256
#define	FLEN		MAXDICNAME
#else
#define FLEN	60
#endif

#define	MAX_OPENDIC		40		// 最大オープン可能ファイル数

#define	L_HEADERNAME	100		// ヘッダー部文字列長
#define	L_DICTITLE		40		// 辞書タイトル名長

// HEADER用macros //
#define	HLOW_BYTE(x)	((uint8_t)(x))
#define	HHI_BYTE(x)		((x)>>8)

#ifdef NEWDIC51
#define	LWORD	1024	// 見出し語の最大長
#define	LWORD_COMPLEN	255	// 見出し語の最大圧縮長
#define	LJAPA	0x40000	// 日本語訳の最大長
#define	LJAPA1	LJAPA	// 属性長１を除いた長さ（NEWDICでは属性長を考えない）
#define	LPRON	1000	// 発音記号の最大長
#define	LEXP	0x40000	// 用例の最大長
#define	LJALL	(LJAPA+LPRON+LEXP)	// japa.cppのテキスト読込みで使用
#define	LMAX	LEXP	// 以上の長さで最も長いもの
#else
#define	LWORD	248		// 見出し語の最大長
#define	LWORD_COMPLEN	247	// 見出し語の最大圧縮長
#define	LJAPA	10000	// 日本語訳の最大長
#define	LJAPA1	LJAPA	// 属性長１を除いた長さ（NEWDICでは属性長を考えない）
#define	LPRON	1000	// 発音記号の最大長
#define	LEXP	100000	// 用例の最大長
#define	LJALL	(LJAPA+LPRON+LEXP)	// japa.cppのテキスト読込みで使用
#define	LMAX	LEXP	// 以上の長さで最も長いもの
#endif
#define	NMAXREF	10		// 参照項目最大数
#ifdef NEWDIC51
#define	DIC_VERSION		DIC_VERSION51
#else
#define	DIC_VERSION		DIC_VERSION5
#endif

#define INDEX_BLOCKSIZE		2048			// インデックス基本ブロック長(NEWDIC2以前)
#define	MAX_INDEX_BLOCK		20				// 最大インデックスブロック数(NEWDIC2以前)
#define MAX_INDEX			(4096-256)		// インデックスの要素の数(NEWDIC2以前)
#define	MAX_PBN				4094			// 最大物理ブロック数(NEWDIC2以前)
#define	MAX_PBN2			0x200000		// 最大物理ブロック数(最大2Gに制限 - 理論上は2Tまで大丈夫)
#define	MAX_INDEX3			(32767*256)		// NEWDIC3最大インデックス数
#define	MAX_BLOCKNUM		0x7fff			// 最大連結ブロック数
#define	MAX_FIELD1SIZE		0xffff			// 最大Field1サイズ
#define	MAX_FIELD1JAPA		(MAX_FIELD1SIZE - LWORD - L_FieldHeader-sizeof(tfield)-1)
#define HEADER_SIZE			1024			// ほか、dic.cpp:DATA_BLOCKSIZE, dic.cpp:EXTBLOCKを参照
#define HEADER_LOC			0

// BOCUなどのcompressionからwchar_tに解凍されたときの最大長（文字単位）
#ifdef DIC_BOCU
#define	_BOCUtoWLEN(x)		((x)<<1)
#else
#define	_BOCUtoWLEN(x)		(x)
#endif

#define	LWORD_X				_BOCUtoWLEN(LWORD)

/*---- 辞書のVERSION定数 ----*/
#define	DIC_VERSION1	0x200	// for DOS(NEWDIC)
#define	DIC_VERSION2	0x300	// NEWDIC2
#define	DIC_VERSION3	0x400	// Hyper(NEWDIC3)
#define	DIC_VERSION4	0x500	// Hyper II
#define	DIC_VERSION5	0x600	// Hyper III
#define	DIC_VERSION51	0x60A	// Hyper III+
#define	DIC_VERSION6	0x700	// undefined

/*------- 辞書の種別 -------*/
#define	DICID_NORMAL	0x00			// 通常の辞書(PDIC v5用辞書 属性長は1バイトに固定）
#define	DICID_COMP		0x01			// 圧縮対応(LHA方式)
#define	DICID_BOCU1		0x08			// BOCU-1による圧縮を行う（Unicode使用時のみ）
#define	DICID_UNICODE	0x10			// UNICODE(UTF-16)体系の辞書
#define	DICID_CRYPT		0x40			// 暗号化
#define	DICID_TREE		0x80			// ツリービュー用辞書

#ifdef USE_TREE
#  define	DICID_DEFAULT	(DICID_COMP|DICID_TREE)
#  define	DICID_OPTION	(DICID_COMP)
#else // !USE_TREE

#if defined(USE_COMP) || RO_COMP
#  define	___DICID_COMP	(DICID_COMP)
#else
#  define	___DICID_COMP	(0)
#endif
#ifdef USE_BOCU1
#  define	___DICID_BOCU1	(DICID_BOCU1|___DICID_COMP)
#else
#  define	___DICID_BOCU1	(0)
#endif

#if defined(DIC_UTF16)
#  define	DICID_DEFAULT	(DICID_UNICODE|___DICID_BOCU1)
#  define	DICID_OPTION	(___DICID_COMP)
#elif defined(DIC_UTF8) || defined(DIC_BOCU)
#  define	DICID_DEFAULT	(___DICID_BOCU1)
#  define	DICID_OPTION	(___DICID_COMP)
#else	// 
#  define	DICID_DEFAULT	(0)
#  define	DICID_OPTION	(___DICID_COMP)
#endif

#endif	// !USE_TREE


// ソート順
#define	SK_NORMAL		0				// コード順
#define	SK_IGNCASE		1				// 大文字・小文字同一視順
#define	SK_DICORDER		2				// 辞書順
#define	SK_DESCENDING	3				// 降順

#define	MAX_TAGNAME	40

/*------ 単語属性定数 ------*/
typedef unsigned char	wa_t;
typedef unsigned char	t_exattr;
#define	WA_NORMAL		0x00
#define	WA_LEVELMASK	0x0f			// 単語難易度用マスク（0:なし 1-15:レベル）
#define	WA_EX			0x10			// 拡張日本語訳フラグ
#define	WA_MEMORY		0x20
#define	WA_JEDIT		0x40
//#define	WA_REF			0x80

#define	WA_MINLEVEL		0
#define	WA_MAXLEVEL		15
#define	WA_LEVELNUM		16

// 品詞,文語頻度順位,口語順位

/*------ 日本語訳種別 ------*/
// JT_...の最後には '\0' が付加される(これに次のJT_...が続く）
// ただし、JT_BYTEが立っている場合は付加されない
// JT_PRON,JT_REF,JT_LINK,JT_BYTE,JT_COMPはNEWDIC2以降で使用
#define	JT_EXP		0x01				// 用例部
#define	JT_PRON		0x02				// 発音記号部
#define	JT_REF		0x03				// 参照リンク(削除)
#define	JT_LINK		0x04				// リンクデータ（直接取込まない)(JT_BYTEは必ず付ける)
#define	JT_EPWING	0x05
#define	JT_BYTE		0x10				// バイトカウントによるデータ格納（バイナリーデータ可能）
										// 続くt_jtbはサイズ
typedef	unsigned short t_jtb;			// JT_BYTEのサイズ型
typedef unsigned int t_jtb2;			// JT_BYTE for NEWDIC3
#define	t_noc		uint8_t				// 非圧縮部のサイズ型
typedef unsigned int t_id;				// オブジェクトIDの型
#define	t_jlink		uint8_t				// JL_...

#define	JT_COMP		0x40				// データは圧縮されている(JT_BYTEが通常付く)
#define	JT_END		0x80				// このフラグだけ立っていれば終了

// 圧縮ポリシー
#define	CP_NOCOMP	0x00				// 圧縮はしない
#define	CP_COMP		0x01				// 圧縮する
#define	CP_COMP1	0x01				// なるべく圧縮しない
#define	CP_COMP2	0x03				// なるべく圧縮する
#define	CP_TEXT		0x10				// テキスト(JT_EXP,JT_PRON)も圧縮する
#define	CP_MINLEN	1024				// 圧縮条件の最低長

#define	CP_USEFIELD2	0x100			// 内部作業用

#define	OBJTITLE_MAX	80				// オブジェクトタイトル最大長

// OS
#define	OS_DOS			0x00				// DOS,Windows,OS/2,Unicode...
#define	OS_MAC			0x01				// Mac
#define	OS_UNIX_SJIS	0x02
#define	OS_UNIX_EUC		0x03
#define	OS_UNIX_JIS		0x04
#define	OS_UTF8			0x10
#define	OS_BOCU			0x20		// BOCU encoding

#if defined(DIC_BOCU)
#  define	OS_DEFAULT		OS_BOCU
#elif defined(DIC_UTF8)
#  define	OS_DEFAULT		OS_UTF8
#elif defined(DOS) || defined(_Windows) || defined(_UNICODE)
#  define	OS_DEFAULT		OS_DOS
#elif defined(UNIX)
#  define	OS_DEFAULT		OS_UNIX_EUC
#else
#  error	OS_DEFAULT must be defined
#endif

#if !defined(DEF_SRCHMODE)
#define	DEF_SRCHMODE
enum SrchMode {
	//検索対象フラグ
	SRCH_WORD	= 0x01,		//単語の検索		(SRCH_WORDとSRCH_JAPAはOR可能)
	SRCH_JAPA	= 0x02,		//日本語訳の検索
	SRCH_ALL	= 0x04,		//全検索
	SRCH_HIDE	= 0x08,		//隠し単語の検索(辞書Ver.1.10以降)
	SRCH_MEMORY	= 0x0c,		//必須単語の検索(辞書Ver.1.10以降)
	SRCH_JEDIT	= 0x10,		//修正単語の検索
	SRCH_EXP	= 0x20,		// 用例検索
	SRCH_LEVEL	= 0x40,		// 単語レベル検索
	SRCH_PRON	= 0x80,		// 発音記号検索（未対応）
	SRCH_Q		= 0x100,	// ?単語検索
	SRCH_TITLE	= 0x200,	// オブジェクトタイトル検索
	SRCH_FILENAME = 0x400,	// ファイルリンクファイル名
	SRCH_FILECONT = 0x02000000,	// ファイル内容検索(requires xdoc2txt)

	//検索モードフラグ
	SRCH_IGN	= 0x0800,	//大文字・小文字の区別
	SRCH_HEAD	= 0x1000,	//検索文字が英語部の先頭

#if !defined(_Windows)
	// 以下、要VWX.COM
//	SRCH_WFLAG	= 0x800,	//ワード検索
	SRCH_REGEXP	= 0x2000,	//正規表現検索
	SRCH_ZENHAN	= 0x4000,	//全角半角同一視
	SRCH_KANA	= 0x8000,	//ひらがな・カタカナ同一視
#else
	// 以下、JRE.DLL
	SRCH_REGEXP	= 0x2000,	//正規表現検索
	SRCH_ZENHAN	= 0x4000,	//全角半角同一視
	SRCH_KANA	= 0x8000,	//ひらがな・カタカナ同一視
	SRCH_FUZZY	= 0x10000,	// 曖昧検索
	#define	USE_SRCH_FUZZY
	SRCH_WORDSRCH = 0x20000,	// 単語検索
#endif
	SRCH_NOT	= 0x00040000,	// 否定
#if !defined(_UNICODE) && !MIXDIC
	SRCH_WORD_S = 0x00100000,	// 英単語はシングルバイト文字
	SRCH_JAPA_S = 0x00200000,	// 日本語訳はシングルバイト文字
	SRCH_EXP_S  = 0x00400000,	// 用例はシングルバイト文字
//    SRCH_PRON_S = 0x800000,	// 発音記号は常にシングルバイト
#endif
	SRCH_FAST = 0x01000000,		// 高速全文検索
	//SRCH_FILECONT = 0x02000000,	// see above
#ifdef ANSI
	SRCH_dummy = 0xffffffff
#endif
};
#endif	// !DEF_SRCHMODE

#define	SRCH_TARGET_MASK	0x020007ff	// 検索対象マスク

#ifdef USE_REGEXP
#define	SRCH_EXTMASK	(SRCH_REGEXP)
#else
#define	SRCH_EXTMASK	0
#endif

#if defined(USE_JRE) || defined(USE_REXP)
// 拡張検索フラグ ( for DDE of PDIC for Windows )
#define	SRCH_JRE	0x10000000L
#define	SRCH_REXP	0x20000000L
#endif


class Japa;

// AllSearch用定数
#define	AS_CONTINUE		0
#define	AS_FOUND		1
#define	AS_ERROR		-1
#define	AS_END			-2
#define	AS_STOP			-3
#define	AS_NONE			-4
#define	AS_RUNNING		-5

// DdeDicMan::outflagのフラグ定数
#define	OF_WORD		0x01
#define	OF_JAPA		0x02
#define	OF_EXP		0x04
#define	OF_LEVEL	0x08
#define	OF_PRON		0x10
#define	OF_MEMORY	0x20
#define	OF_JEDIT	0x30

// 出力先
#define	OD_NONE		0x00
#define	OD_CLIPBD	0x01
#define	OD_FILE		0x02
#define	OD_POPUP	0x04
#define	OD_CHAR		0x08
#define	OD_BROWSER	0x10
#define	OD_DEBUG	0x80

// リンクオブジェクト読み出し方法の指定
#define	OLA_NORMAL		0x0000
#define	OLA_NOTLINK		0x0001		// 再リンクしない
#define	OLA_NOTREADOLE	0x0002		// OLEは読み出ししない
#define	OLA_NOTREADFILE	0x0004		// ファイルリンクは読み出ししない
#define	OLA_NOTREADBMP	0x0008		// 画像データは読み出ししない
#define	OLA_NOTREADVOICE 0x0010		// Voice
#define	OLA_NOTREADEPWING	0x0020	// EPWing link
#define	OLA_NOTREADOBJ	0x003e		// リンクオブジェクトは読み出ししない(マスク）

// - typedefs ----------------------------------------------------------------
#include "dicmix.h"

#if defined(_Windows) || defined(DOS)
#  define	__PACKED		/* unnecessary for x86 */
#else
#  define	__PACKED		__attribute__ ((packed))	/* for GCC */
#  define	__USE_PACKED
#endif

#ifndef __USE_PACKED
#pragma	pack(push,1)
#endif

// - JT_LINK ----------------------------------------------------------------
// 非圧縮時 //
struct JLinkStruct {
	t_exattr jtype;		// JT_LINK
#ifdef NEWDIC4UNI
	uint8_t	reserved;
#endif
	t_jtb	size;		// size = sizeof(JLinkStruct) - sizeof(jtype) - sizeof(size)
	t_jlink	jlink;		// JL_....
	t_id	id __PACKED;			// ID
	tchar	title[1];	// title
//	tchar	data[];
	tchar *GetDataPtr( )
		{ return (tchar*)(((tchar*)this) + sizeof(JLinkStruct) ); }
};
// 圧縮時 //
struct JLinkStructC {
	t_exattr	jtype;		// JT_LINK | JT_COMP
#ifdef NEWDIC4UNI
	uint8_t	reserved;
#endif
	t_jtb	size;
	t_noc	nocomplen;	// = sizeof(jlink) + sizeof(t_id) + sizeof(title)
	t_jlink	jlink;
	t_id	id __PACKED;
	tchar	title[1];
//	AR_HEADER	arheader;
//	tchar	compdata[];
//	tchar *GetDataPtr( )
//		{ return (tchar*)(((tchar*)this) + sizeof(JLinkStruct) ); }
};

// 非圧縮時 //
struct JLinkStruct2 {
	t_exattr 	jtype;		// JT_LINK
#ifdef NEWDIC4UNI
	uint8_t	reserved;
#endif
	t_jtb2	size;		// size = sizeof(JLinkStruct) - sizeof(jtype) - sizeof(size)
	t_jlink	jlink;		// JL_....
	t_id	id __PACKED;			// ID
	tchar	title[1];	// title
//	tchar	data[];
	tchar *GetDataPtr( )
		{ return (tchar*)(((tchar*)this) + sizeof(JLinkStruct) ); }
};
// 圧縮時 //
struct JLinkStructC2 {
	t_exattr	jtype;		// JT_LINK | JT_COMP
#ifdef NEWDIC4UNI
	uint8_t	reserved;
#endif
	t_jtb2	size;
	t_noc	nocomplen;	// = sizeof(jlink) + sizeof(t_id) + sizeof(title)
	t_jlink	jlink;
	t_id	id __PACKED;
	tchar	title[1];
//	AR_HEADER	arheader;
//	tchar	compdata[];
//	tchar *GetDataPtr( )
//		{ return (tchar*)(((tchar*)this) + sizeof(JLinkStruct) ); }
};

#ifndef __USE_PACKED
#pragma	pack(pop)
#endif

#ifdef OLE2
#define PDICOBJPREFIX			((LPTSTR)"A#")
#define PDICOBJCLONE			((LPTSTR)"B#")
#define PDICOBJTEMP				((LPTSTR)"C#")
#else
#define PDICOBJPREFIX          ((LPTSTR)"PDICW #")
#define PDICOBJCLONE           ((LPTSTR)"PDICW1#")
#define PDICOBJTEMP            ((LPTSTR)"PDICW2#")
#endif

#define	DICF_OLDDIC		0x0001	// 古いバージョンの辞書
#define	DICF_READONLY	0x0002	// リードオンリーの辞書
#define	DICF_NEWDIC		0x0004	// 現バージョンの辞書
#define	DICF_WRITE		0x0008	// 書換可能辞書

#if defined(PDICW) || defined(CMD32)
#define	GENERICREXP	GenericRexp
class GenericRexp;
#else	// !PDICW && !CMD32
#define	GENERICREXP	RegExp
class RegExp;
#endif


#ifdef USE_NETWORK
#define	CHECK_NETWORK(dic)	(dic.Flags & MPF_NETWORK) && (!dic[0].IsReadOnly()) &&
#define	CHECK_NETWORK2	(Flags & MPF_NETWORK) && (!sdic[0].IsReadOnly()) &&
#else
#define	CHECK_NETWORK(dic)	(!dic[0].IsReadOnly()) &&
#define	CHECK_NETWORK2	(!sdic[0].IsReadOnly()) &&
#endif

#if defined(USE_DICORDER)
int cmpword( const _kchar *str1, const _kchar *str2 );
int cmpnword( const _kchar *str1, const _kchar *str2, int n );
#else
inline int _fcmpword( const _kchar *str1, const _kchar *str2 )
{
	return _kcscmp( str1, str2 );
}
inline int cmpword( const _kchar *str1, const _kchar *str2 )
{
	return _kcscmp( str1, str2 );
}
inline int cmpnword( const _kchar *str1, const _kchar *str2, int n )
{
	return _kcsncmp( str1, str2, n );
}
#endif	// !defined(USE_DICORDER)

#define	DICERR_OPEN				1
#define	DICERR_INDEX_MEM		2
#define	DICERR_INDEX_NUM_FULL	3
#define	DICERR_DATA_DISK		4
#define	DICERR_READWRITE		5
#define	DICERR_VERSION			6
#define	DICERR_DRIVE			7
#define	DICERR_OPEN_ALREADY		8
#define	DICERR_OPEN_READONLY	9
#define	DICERR_OPEN_TOOMANY		10
#define	DICERR_MEMORY			11
#define	DICERR_DUPOPEN			12
#define	DICERR_TYPE				13
#define	DICERR_CREATE			14
#define	DICERR_DIVMULTI			15

#endif	// __DICDEF_H

