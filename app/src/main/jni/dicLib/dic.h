#ifndef	__DIC_H
#define	__DIC_H

#include "pddefs.h"
#include "dicdef.h"
#include "dicerr.h"
#include "kstr.h"

// NOFIELD2	: NEWDIC3以上で、Field2 typeを使用しない

// DICNET	: 辞書をネットワーク上で共有する

#ifdef SMALL
#define	NOUPDATEWORD	1	// updateはdeleteとaddで代用
#else
#define	NOUPDATEWORD	0	// updateは高速に
#endif

#ifndef USE_DICFASTMODE
#ifdef SMALL
#define	USE_DICFASTMODE	0
#else
#define	USE_DICFASTMODE	1
#endif
#endif

#include	"pdmem.h"
#include	"rexpgen.h"
#include	"faststr.h"

#ifdef USE_UNIVDIC
#include "univdic.h"
#endif

#ifndef __USE_PACKED
#pragma	pack(push,1)
#endif

typedef unsigned short t_pbn;
typedef unsigned int t_pbn2;

typedef	unsigned short	tfield;		// フィールド長
typedef unsigned int tfield2;

#define	BLK_ERROR		((t_pbn2)(~0))	// エラー

#define	tcomp			uint8_t// 圧縮長
typedef short			tindex;
#define	attrbyte		sizeof(wa_t)

#ifdef EPWING
#define	EVIRTUAL	virtual
#undef EUVIRTUAL
#define	EUVIRTUAL	virtual
#else
#define	EVIRTUAL
#endif

#ifdef USE_UNIVDIC
#define	UVIRTUAL	virtual
#undef EUVIRTUAL
#define	EUVIRTUAL	virtual
#else
#define	UVIRTUAL
#endif

#ifndef EUVIRTUAL
#define	EUVIRTUAL
#endif

/*--------------------------------------*/
/*		   辞書の現在の諸データ			*/
/*--------------------------------------*/
struct HEADER {
	// +00
	_kchar headername[ L_HEADERNAME/sizeof(_kchar) ];	// 辞書ヘッダータイトル
	// +64
	_kchar dictitle[ L_DICTITLE/sizeof(_kchar) ];		// 辞書名
	// +8C
	short version;				// 辞書のバージョン
	unsigned short lword;		// 英単語の最大長
	// +90
	unsigned short ljapa;		// 日本語訳の最大長
	unsigned short block_size;	// １ブロックのバイト数
								// NEWDIC2の場合は、/16
								// NEWDIC3の場合は256固定
	unsigned short index_block;	// インデックスブロック数
								// NEWDIC2以前では、2,048ﾊﾞｲﾄ単位
								// NEWDIC3では、256ﾊﾞｲﾄ単位(block_size単位)
	unsigned short header_size;	// ヘッダーのバイト数
								// NEWDIC3では256単位で増減
	// +98
	unsigned short index_size;	// インデックスのバイト数
								// NEWDIC3では参照してはいけない(index_block * block_sizeで調べる)

	short empty_block;			// 空きブロックの先頭物理ブロック番号（ないときは-1）
								// NEWDIC3では、empty_block2を使用する
	unsigned short nindex;		// インデックスの要素の数
								// NEWDIC3では、nindex2を使用する
	short nblock;				// 使用データブロック数
								// NEWDIC3では、nblock2を使用する
	// +A0
	unsigned int nword;			// 登録単語数

	// +A4
	uint8_t dicorder;	 			// 辞書の順番
	uint8_t dictype;				// 辞書の種別

	uint8_t attrlen;				// 単語属性の長さ
	// +A7
	uint8_t os;					// OS
	// +A8
	int olenumber;				// OLE用シリアル番号
	// +AC
	uint8_t dummy_lid[10];
	// +B6
	uint8_t index_blkbit;			// 0:16bit, 1:32bit
	uint8_t dummy0;
	// +B8
	uint extheader;			// 拡張ヘッダーサイズ
	// +BC
	int empty_block2;			// 空きブロック先頭物理ブロック番号
	// +C0
	int nindex2;				// インデックス要素の数
	// +C4
	int nblock2;				// 使用データブロック数
	// +C8
	uint8_t dummyc0[8];
	// +D0
	unsigned int dummyd0;
	// +D4
	uint8_t dummy00[4];
	// +D8
	uint8_t dicident[8];			// 辞書識別子
	// +E0
	uint8_t derefid[8];			// dereference dic ID
	// +E8
	int generation;
	// +EC
	//uint8_t dummy[4];
	unsigned int lang_proc_id;	// ID of Language Processor. see TLangProc class.
	// +F0
	uint8_t dummyt[12];
	uint8_t checksum[4];
	// +100
	uint8_t dummyx[0x400-0x100];
#ifdef _Windows
	void GetDicType( tnstr &str ) const;
#endif
	uint GetBlockSize( ) const
	{
		return block_size;
	}
	tnstr GetIdStr() const;

	void CalcChecksum(uint8_t *buf);
	void SetChecksum();
	bool ValidateChecksum();
	bool IsXcScheme() const;

	int GetVersion() const		{return version;}
	int GetNumWords() const	// 正確な現在の単語数はdata->GetNumWord()を使用すること
		{ return nword; }
	int GetOrder( ) const 		{ return dicorder; }
	int GetDicID( ) const		{ return dictype; }
	unsigned int GetLangProcId() const	{ return lang_proc_id; }
};

#if !defined(ANSI) && !defined(_MSC_VER) && !defined(UNIX)
#if sizeof(HEADER) != HEADER_SIZE
#if sizeof(HEADER) > HEADER_SIZE
#error	HEADER is too large
#else
#error	HEADER is too small
#endif
#endif
#endif

inline ushort pbnGetBlock( t_pbn b )
{
	return (ushort)(b & 0x0fff);
}
inline ushort pbnGetNum( t_pbn b )
{
	return (ushort)(( ((ushort)b) >> 12 ) + 1);
}
inline t_pbn MakePbn( ushort block, ushort num )	// NEWDIC3では使ってはならない
{
	return (t_pbn)(block | ( ( num - 1 ) << 12 ));
}
inline ushort pbnGetBlock( t_pbn2 b )
{
	return (ushort)((ushort)b & 0x0fff);
}
inline ushort pbnGetNum( t_pbn2 b )
{
	return (ushort)(( (ushort)b >> 12 ) + 1);
}

#ifndef __USE_PACKED
#pragma	pack(pop)
#endif


//カーソル位置保存
struct FINDWORD {
	int lbn;
	t_pbn2 pbn;
	int lp;
	_KChar word1;
	_kchar fword[LWORD+1];
	void clear( );
	FINDWORD(){}
	~FINDWORD(){}
};

// 辞書の新規作成用構造体 //
struct DICCREATEPARAM {
	const tchar *filename;
	const tchar *title;
	short version;
	uint8_t size;
	uint8_t id;
};

#include "filebuf.h"

///////////////////////////
///	PdicObjectクラス	///
///////////////////////////
class PdicObject {
protected:
protected:
	int &error;		//エラーが起きたときその種類の数値が入る
public:
	PdicObject( int &error );
	virtual ~PdicObject(){}
	int GetAttrLen( )	{ return sizeof(wa_t); }
	int GetErrorCode( )	{return error;}
//		void destroy( );

	// 英単語の大文字・小文字同一視用
public:
	int cmpword( const _kchar *str1, const _kchar *str2 )
		{ return ::cmpword( str1, str2 ); }
	int cmpnword( const _kchar *str1, const _kchar *str2, int n )
		{ return ::cmpnword( str1, str2, n ); }
	int _fcmpword( const _kchar *str1, const _kchar *str2 )
		{ return ::_fcmpword( str1, str2 ); }
};

// virtual	: NEWDIC2だけでは普通の関数で、NEWDIC3,ND3ONLYではvirtual

#define	HCALLDATA		((HPdicData*)this)->
#define	PDICINDEX		HPdicIndex
#define	PDICDATA		HPdicData

#include "Dicinx.h"

///////////////////////
///	HPdicIndexクラス///
///////////////////////
// Hyper Pdic Index class
class HPdicIndex : public PdicIndex {
private:
public:
	HPdicIndex( int &error, FileBuf &_file );
	virtual ~HPdicIndex();
protected:
	virtual int getIndex( );
#if USE_MMI
	virtual int getIndexMM( );
	virtual int enterEdit();
#endif
public:
	virtual int WriteIndex( );
	bool Has32();
	t_pbn2 IndexBlockNo( int lbn ) { return index[lbn]->block; }
};

#ifndef __USE_PACKED
#pragma	pack(push,1)
#endif

struct FieldFormat {
	tfield	fieldlen;
	tcomp	complen;
	wa_t	attr;
	_kchar 	word[1];
	//_kchar	japa[];
} __PACKED;

#define	L_FieldHeader	( sizeof( FieldFormat ) - sizeof(_kchar) )
#define	L_FieldLen		( sizeof( tfield ) )


inline FieldFormat *NextField( const FieldFormat *ff )
{
	return (FieldFormat *) ( (const uint8_t*)ff + ff->fieldlen + L_FieldHeader );
}

//	空きブロック構造体
struct EmptyBlockFormat {
	ushort	null;	// 常に０
	ushort	nbn;	// 次の物理ブロック番号
};

struct FieldFormat2 {
	tfield2	fieldlen;
	tcomp	complen;
	wa_t	attr;
	_kchar	word[1];
	//_kchar	japa[];
} __PACKED;

#define	L_FieldHeader2	( sizeof( FieldFormat2 ) - sizeof(_kchar) )
#define	L_FieldLen2		( sizeof( tfield2 ) )

#define	FF2(x)	((FieldFormat2*)(x))

inline FieldFormat2 *NextField2( const FieldFormat2 *ff )
{
	return (FieldFormat2 *) ( (const uint8_t*)ff + ff->fieldlen + L_FieldHeader2 );
}

//	空きブロック構造体
struct EmptyBlockFormat2 {
	ushort	null;	// 常に０
	uint	nbn __PACKED;	// 次の物理ブロック番号
} __PACKED;

#ifndef __USE_PACKED
#pragma	pack(pop)
#endif

struct DivList {
	int divtype;		// 0 : 分割  1 : Grow
	t_pbn2 oldpbn;		// 変更前
	t_pbn2 newpbn;
	ushort oldblknum;
	ushort newblknum;
	_kchar *word;
};

typedef unsigned short t_blknum;

struct DivInfo {
	int diff;		// 増減バイト数(単語圧縮を考えて)
	int nextdiff;	// 挿入単語の次の圧縮長変動分(byte count)
	int complen;	// 挿入単語の圧縮長(charactor count)
	
};

// 空きブロックリンクリスト
// 空きブロックのリンクを相対物理ブロック番号で管理
// TopのEmptyListのoffsetは絶対物理ブロック番号、それ以降の子のoffsetは相対番号
// TopのnextがNULLの場合は空きブロックなし
class EmptyList {
public:
	long offset;
	EmptyList *next;
	int num;
protected:
	int resort;
public:
	EmptyList( EmptyList *next, long offset, int num );
	~EmptyList();
	void Register( t_pbn2 pbn, int blknum );
	int Free( t_pbn2 pbn );
	int Free( t_pbn2 pbn, int blknum );
	void _Free( );
	EmptyList *Search( t_pbn2 pbn, long &offs );
	int HasBlock( )
		{ return next ? 1 : 0; }
	t_pbn2 NewBlock( int num, int *seqblknum=NULL, t_pbn2 *startpbn=NULL );
	int GetSeq( t_pbn2 pbn );
	int Sort();
	void Concat( );
	void Clear();
	void GetInfo( int &numlist, int &numblock );
protected:
	void Check( const _kchar *str );	// for debug
};

#include "Hdicdata.h"


tchar *lnewstr( const tchar * str );

#include "dicixdt.h"

#ifdef EXTHEADER
struct THeaderItem {
	void *data;
	int length;
	THeaderItem();
	~THeaderItem();
};
class THeaderArray : public FlexObjectArray< ::THeaderItem > {
public:
	THeaderArray();
	~THeaderArray();
	int FindTag( const _mchar *tag );
	const void *GetData( int index );
	const void *FindData( const _mchar *tag, int *length );
	void SetData( const _mchar *tag, const void *data, int length );
};
#endif	// EXTHEADER

#define	PDICBASE_SUPER	IndexData

class TPdicBackup {
public:
	virtual void NotifyMod(class PdicBase *) = 0;
};

///////////////////////////////////////////////////
///	PdicBase class
///////////////////////////////////////////////////
class PdicBase : public PDICBASE_SUPER {
typedef PDICBASE_SUPER super;
protected:
	PdicBase();
	virtual void AddMap(const tchar *filename){}
	virtual void RemoveMap(){}
	TPdicBackup *Backup;	// reference pointer
public:
	virtual ~PdicBase();
	class Pdic *GetPdic()
	{
#if defined(__ANDROID__) && !defined(__BORLANDC__)
		return (Pdic*)this;
#else
		return dynamic_cast<Pdic>(this);
#endif
	}

	UVIRTUAL bool CanClose( )
		{ return true; }
	void SetBackup(TPdicBackup *backup)
		{ Backup = backup; }
	inline void NotifyBackup()
		{ if (Backup) Backup->NotifyMod(this); }


	// open/close //
protected:
	int readonly;
public:
	virtual int Open( const tchar * fname, bool _readonly );
	int Open(const tchar *fname)
		{ return Open(fname, false); }
	int OpenRetry( const tchar * fname, bool readonly=false );
	int OpenReadOnly(const tchar *fname)
		{ return Open(fname, true); }
	int Create( const DICCREATEPARAM *dcp );
	static void GenerateDicId(uint8_t *id, const tchar *filename);
	virtual int Create( const tchar *fname, const tchar *dicname, int size, int dictype, int lang_proc_id=-1 );
	int CreateHyper(const tchar *fname, int lang_proc_id=-1)
		{ return Create( fname, _T(""), 0, ___DICID_COMP|___DICID_BOCU1, lang_proc_id); }
	virtual void Close( );
	int ClearAll(bool force_write);
protected:
	void CloseFlush();

	// ODA: on demand access
#if USE_ODA
protected:
	bool TempCloseReq;
	bool TempClosed;
	bool TempROnly;
	bool NeedFullOpen;
	bool ODAProgress;
	struct stat LastFileStat;
	int FileLength;
	unsigned CloseReqTime;	// the request time to close.
	PdicBase *ODANext;
	static PdicBase *ODALink;
	static int ODACloseDelay;
public:
	bool ODAClose(bool request);
	bool ODAReopen();
	bool ODAROnlyOpen();
	inline bool ODACloseReqExpired(unsigned now)
		{ return now - CloseReqTime >= (unsigned)ODACloseDelay; }
	static inline void SetODACloseDelay(int delay)
		{ ODACloseDelay = delay; }
protected:
	bool ODAReopenCommon();
	// ODA Link Management //
public:
	static bool ODACloseTimerProc();
protected:
	static void ODAAddLink(PdicBase *dic);
	static void ODARemoveLink(PdicBase *dic);
#endif	// USE_ODA

	// text duplication dictionary
#if DICTXDUP
protected:
	tnstr TXDName;
	int TXDFileTime;
	bool TXDWriteReq;
	bool TXDUpdating;	// 再入防止用
	int TXDType;
	bool RecordLock;	// record時のlockするか？
public:
	void SetTextDic(const tchar *filename, int dictype);
	int GetTextDicType();
	inline bool TXDEnabled() const
		{ return TXDName.exist(); }
	const tchar *GetTXDName() { return TXDName; }
	const tchar *GetDispName()
		{ return TXDEnabled() ? GetTXDName() : GetFileName(); }
	bool TXDCheckUpdate();
	void SetRecordLock( bool lock ) { RecordLock = lock; }
protected:
	bool TXDUpdate();
	int TXDGetFileTime();
private:
	bool TXDConvert();
#else
public:
	inline const tchar *GetDispName()
		{ return GetFileName(); }
#endif

public:
	// obsolete methods //
	void AllowOldUpdate(){}

	// Headers //
protected:
	HEADER *header;
#if defined(USE_JLINK)
	int linkattr;			// リンクオブジェクト読み出し方法の指定
#endif
	int InitHeader( );
	int ReadHeader();
public:
	int WriteHeader();

	const HEADER *GetHeader( )	{return header;}
	// オープンしないで、ヘッダーだけを読みたい場合
	//	リードオンリィでオープンされ、headerを読み込んだ後、すぐにクローズされる。
	//	既にオープンされていても大丈夫
	//	エラーの場合は、NULLを返す
	const HEADER *ReadHeader( const tchar *filename, unsigned int *size=NULL );

	// オープンしないで、ヘッダー部の変更をする
	//	現在のところ、辞書名の変更のみサポート
	//	返り値はエラー(-1)正常(0)
	//	※オープンされていても大丈夫
	int ChangeHeader( const tchar *filename, const tchar *dictitle );

	// Extended header //
#ifdef EXTHEADER
protected:
	bool ExtHeaderRead;	// ExtHeaderをreadした場合はtrue
	int _WriteExtHeader();
	THeaderArray ExtHeader;
	bool ExtHeaderMod;	// ExtHeaderを変更した場合はtrue
public:
	bool ReadExtHeader( );
	int WriteExtHeader();
	int SizeExtHeader();
	// access functions //
	int ReadExtInteger( const _mchar *tag, int def );
	bool WriteExtInteger( const _mchar *tag, int value );
	bool ReadExtString_( const _mchar *tag, tnstr &s );
	bool WriteExtString_( const _mchar *tag, const tchar *s );
#ifdef _UNICODE
	bool ReadExtString_s( const _mchar *tag, TCharSingle &s );
	bool WriteExtString_s( const _mchar *tag, const char *s );
#endif
#endif

	// File operation //
protected:
	FileBuf file;
#if !defined(SMALL)
	bool auto_write;	//TRUEのときは、ヘッダーとインデックスを書き換えるたびにディスクへ出力する
	bool IsAutoWrite()
		{ return auto_write; }
#endif
public:
	UVIRTUAL bool IsModified()
		{ return file.isModify(); }
	int ModifyCounter() const
		{ return file.modifyCounter(); }
	UVIRTUAL void CancelModify()
		{ file.cancelModify(); }
	const tchar *GetFullFileName()
		{ return file.GetFullFileName(); }
	bool IsOpened() const;
	void Flush(bool closing=false);
	UVIRTUAL const tchar *GetFileName( );

	// find/search //
public:
	UVIRTUAL int Read( const _kchar *word, Japa *japa )
		{ return _read(word, japa); }
	int _read( const _kchar *word, Japa *japa );
	int read( const tchar *word, Japa *japa );

	// Find //
	int _Find( const _kchar * word );		// bsearch + strequ
	int _Find( const _kchar *word, Japa *japa, AllSearchParam *all=NULL);
	int Find( const tchar *word );
	int Find( const tchar *word, Japa *japa, AllSearchParam *all=NULL);
	int FindK(const tchar *kword, Japa *japa, AllSearchParam *all=NULL);

	// Search //
	void InitReadPare( )	{ SetAllSearch( _T(""), SRCH_ALL, NULL, NULL );}
	//int ReadPare( tnstr &word, Japa &japa);

//	UVIRTUAL int NextAllSearch( _KChar &word, Japa *japa, AllSearchParam *all=NULL);
//	UVIRTUAL int PrevAllSearch( _KChar &word, Japa *japa, AllSearchParam *all=NULL);
#if NETDIC || DICTXDUP
	UVIRTUAL bool SetAllSearch( const tchar * word, SrchMode mode, GENERICREXP *jre, AllSearchParam *all=NULL );
	UVIRTUAL int JumpAS_( const tchar *word, bool fBack, bool fSameBack=FALSE, AllSearchParam *all=NULL );
	int JumpAS_k( const _kchar *word, bool fBack, bool fSameBack=FALSE, AllSearchParam *all=NULL );
#endif
	UVIRTUAL void GetFoundJapa( Japa &j, AllSearchParam *all=NULL )
		{ getfjapa( j, all ); }
	UVIRTUAL const _kchar *GetFoundWord() const
		{return all.fw.fword;}
#if MIXDIC
//	UVIRTUAL int NextAllSearch( tnstr &word, Japa *japa, AllSearchParam *all=NULL);
//	UVIRTUAL int PrevAllSearch( tnstr &word, Japa *japa, AllSearchParam *all=NULL);
#endif

	// record/update/erase //
	// Native char. code methods.
	// 以下のmethod(_がついたもの)は他の辞書に反映されなくなるので特殊な場合のみ利用可能
public:
	int _record( const _kchar *word, const Japa &japa, int srchf=1);	//単語の追加
	int _update( const _kchar *word, const Japa &japa, bool reload=false);	//カーソル位置の単語を修正(if reload=false)
	int _erase( const _kchar * word);					//wordを削除
protected:
	int _erase();								//カーソル位置の単語を削除
public:
	UVIRTUAL int _Rename( const _kchar *oldword, const _kchar *newword, int newwordlen );	// 単語名のみの変更
private:	// Override to avoid the compiler warning.
	int _record( const _kchar *word, uint wordlen, const Japa &japa)
		{ return super::_record(word, wordlen, japa); }
	int _update( const _kchar *word, uint wordlen, const Japa &japa)
		{ return super::_update(word, wordlen, japa); }
	int _record_nochk( const _kchar *word, int wordlen, const Japa &japa, int srchf );
	int _update_nochk( const _kchar *word, int wordlen, const Japa &japa );
public:
	virtual int record( const tchar * word, const Japa &japa, int srchf=1 );
	virtual int update( const tchar *word, const Japa &japa );
	virtual int erase( const tchar *word );
	int rename( const tchar *oldword, const tchar *newword);

	UVIRTUAL int Record( const tchar *word, const Japa &japa )
		{ return record(word, japa); }
	UVIRTUAL int Update( const tchar *word, const Japa &japa)
		{ return update(word ,japa); }
	UVIRTUAL int Erase( const tchar *word )
		{ return erase(word); }

#if NETDIC || DICTXDUP
protected:
	void ReadNetDic();
	bool RetryWriteLock( class WriteLockObject &__obj__ );
public:
	int BSearch( const _kchar * word, bool forfast=false, AllSearchParam *all=NULL );	// forfast=trueで、Field2の場合は、databufにロードされない！！
#else
	int BSearch( const _kchar * word, bool forfast=false, AllSearchParam *all=NULL )
		{ return super::_BSearch(word, forfast, all); }
#endif

	// Changing attributes //
protected:
	uint8_t *_get_attr_ptr( const _kchar *word );
public:
	wa_t get_attr( const tchar *word );
	int change_attr(const tchar *word, wa_t attr, wa_t mask );
	int _set_attr( const _kchar *word, int bit );
	int _reset_attr( const _kchar *word, int bit );
	int toggle_attr( const tchar *word, int bit );
	int add_level( const tchar *word, int level, int minlevel=WA_MINLEVEL, int maxlevel=WA_MAXLEVEL );	// level分だけレベルを変更(上限、下限チェック有り)
#if MIXDIC || defined(KMIXDIC)
	int reset_attr( const tchar *word, int bit );
	int set_attr( const tchar *word, int bit );
#endif

#if USE_DICFASTMODE
	// Mode settings //
//		void setNWord(int _nword)		{header->nword = _nword;}
	void SetMode(bool _auto_write, bool _fast_mode);	// open後でないと使用できない！！
	UVIRTUAL void SetFastMode( bool fast )
		{ fast ? SetFastMode() : SetNormalMode(); }
	void SetFastMode( )				{ SetMode( false, true ); }
	void SetNormalMode( )			{ SetMode( true, false ); }
#endif

	// Status Information //
	int GetVersion()				{return header->GetVersion();}
	UVIRTUAL int GetNumWords( )	{return data->GetNumWord();}
	int GetNBlock() 				{ return data->GetNBlock(); }
	uint GetBlockSize()				{ return header->GetBlockSize(); }
	EVIRTUAL int GetDicType()		{ return 0; }	// PDIC

	long GetSize( )					{ return file.get_length(); }
	void SetError( int _error )		{ error = _error; }
	UVIRTUAL int GetErrorCode( )	{ return error; }

#ifndef ND3ONLY
	void SetDivRatio( int ratio )	{ data->SetDivRatio( ratio ); }
#endif
	int GetIndexNum( )				{ return index->GetIndexNum( ); }
	uint GetIndexDiskSize( )		{ return index->getIndexDiskSize( ); }
	uint GetFreeRate( );
	int GetUsage(unsigned int filesize=(unsigned int)-1);

	// 辞書サイズインデックスの取得(NEWDIC3では意味が無い)
#ifndef ND3ONLY
	static int GetDicSizeIndex( int inxsize );
	int GetDicSizeIndex( )			{ return GetDicSizeIndex( header->index_size ); }
#endif	// !ND3ONLY
	const _ktchar *GetAllWord( )	{ return super::GetAllWord(); }

	int GetOrder( )					{ return header->GetOrder(); }
	int GetDicID( )					{ return header->GetDicID(); }
	unsigned int GetLangProcId()	{ return header->GetLangProcId(); }
	int CheckCond( int flag );		// DICF_...に合致するかどうか(return 0 or 1)
	PDICDATA *GetPdicData()			{ return data; }
	UVIRTUAL bool IsReadOnly()		{ return readonly & 1 ? true : false; }		// & 1は古い辞書形式を無視するため

	// OLE Objects //
#if	defined(USE_OLE)
public:
	void SetObjectNumber( int _objnumber );
	void FreeObjects( );			// 参照されていないオブジェクトを削除する
#endif
#if defined(USE_JLINK)
private:
	int iObjectNumber;
public:
	int GetObjectNumber( );
#endif

#if defined(USE_JLINK)
	void SetLinkAttr( int flag )
		{ linkattr = flag; }
	bool SetLinkAttr( int flag, bool f );
	int GetLinkAttr( )
		{ return linkattr; }
	int ReadObject( const tchar *word, t_id id, class JLink **jlink );
	int DeleteObject( const tchar *word, t_id id );
	int UpdateObjectTitle( const tchar *word, t_id id, const tchar *title );
	int _DeleteObject( const _kchar *word, t_id id );
	int _UpdateObjectTitle( const _kchar *word, t_id id, const tchar *title );
#endif	// USE_JLINK

	// Utilities //
	//int Optimize( struct PDDICCHECKPARAM *dcp );
	int EasyOptimize( t_pbn2 *_ipbn, t_pbn2 *_pbn );

#ifdef USE_FILELINK
protected:
	tnstr FileLinkPath;
public:
	void SetFileLinkPath( const tchar *flinkpath )
		{ FileLinkPath = flinkpath; }
	const tchar *GetFileLinkPath( )
		{ return FileLinkPath; }
	bool GetRelFileLinkPath(const tchar *filename, const tchar *commonpath, tnstr &relpath);
#endif
	bool WordLock( const _kchar *word )
#if INETDIC
		;
#else
		{ return true; }
#endif
	void WordUnlock()
#if INETDIC
		;
#else
		{}
#endif
	// Misc.
	bool Renaming;	// rename processing
	static bool HasFileChecksum(const tchar *filename);

	// Idle Proc
protected:
	typedef map<PdicBase*, int> pdic_int_map_t;
	static pdic_int_map_t IdleProcs;
	bool IdleProc();
public:
	static bool IdleProcHandler();
	
	// for Debug //
	bool CheckDataBlock( int lbn );
};

///////////////////////////////////////////////////
///	Pdic class
///////////////////////////////////////////////////

#ifdef USE_EXTDB
#include "dicextdb.h"
#define	PDIC_SUPER_CLASS	PdicExtDBLayer
#else
#define	PDIC_SUPER_CLASS	PdicBase
#endif

class Pdic : public PDIC_SUPER_CLASS {
typedef PDIC_SUPER_CLASS super;
protected:
#ifdef _DEBUG
	static int FuncIn;
#endif
	static map<tnstr,Pdic*> PdicMap;	// filename -> Pdic * map.
	int RefCnt;
	int OpenCnt;
	TMutex ThreadCntMutex;
	int ThreadCnt;
	Pdic();
	~Pdic();
	__override void AddMap(const tchar *filename);
	__override void RemoveMap();
public:
	static tnstr NormalizeFileName(const tchar *name);
protected:
	static Pdic *GetInstance(const tchar *filename);
public:
	static Pdic *CreateInstance(const tchar *filename);
	void AddRef();
	Pdic *Clone();
	void Release();
	virtual int Open( const tchar * fname, bool _readonly );
	virtual void Close( );
	bool ThreadUp();
	void ThreadDown();
};

class auto_Pdic
{
public:
	typedef Pdic element_type;

	//
	// construct/copy/destroy
	//
	auto_Pdic()
	 : the_p(Pdic::CreateInstance(NULL))
	{
	}
	auto_Pdic(Pdic* p)
	 : the_p(p)
	{
	}
	auto_Pdic(const tchar *filename)
		:the_p(Pdic::CreateInstance(filename))
	{}

	auto_Pdic (auto_Pdic& a)
	 :  the_p(a.release()) 
	{}

	auto_Pdic& operator= (auto_Pdic& rhs)  
	{ 
	  reset(rhs.release());
	  return *this;
	}

	~auto_Pdic ()
	{
		if (the_p) the_p->Release();
	}

	//
	// members
	//
	Pdic& operator*  ()  const  { return *the_p;   }
	Pdic* operator-> ()  const  { return the_p;    }
	Pdic* get        ()  const  { return the_p;    }

	Pdic* release    ()  
	{ 
	  Pdic* tmp = the_p;
	  the_p = 0; 
	  return tmp; 
	}

	void reset (Pdic* p = 0)
	{ 
	  if (the_p != p)
	  {
		if (the_p)
			the_p->Release();
		the_p = p;
	  }
	}

	operator Pdic *() { return the_p; }
	bool operator !() { return !the_p; }

private:
	Pdic* the_p;
};

// for Optimization //
#define	MAX_TEMPBUFF	64		// テンポラリバッファの最大数
struct TempBuffData {

	uint8_t *buff;
	t_pbn2 lbn;
	int size;

	TempBuffData( const uint8_t *buf, int size, t_pbn2 lbn );
	~TempBuffData();
	int isValid( )
	{
		return buff != 0;
	}
};

class TempBuff : public FlexObjectArray<TempBuffData> {
public:
	TempBuff():FlexObjectArray<TempBuffData>( MAX_TEMPBUFF ){}
	~TempBuff(){}
	int save( const uint8_t *buf, t_pbn2 _lbn, int blksz );
	uint8_t *load( t_pbn2 lbn, t_pbn2 &loadlbn );
	uint8_t *allLoad( t_pbn2 &lbn );
	int isExist( )	{ return get_num(); }
	t_blknum getblknum( t_pbn2 lbn );
};

// End of optimization //

// 辞書open option flag
#define	DICFLAG_NONE			0x0000
#define	DICFLAG_READONLY		0x0001
#define	DICFLAG_INTERNET		0x0002
#define	DICFLAG_LAN				0x0004
#define	DICFLAG_FASTDB_CREATE	0x0008
#define	DICFLAG_TXDUP			0x0010	// support text duplication dictionary

//汎用関数
long checkdfree(int drive);
Pdic *CreatePdic( const tchar * pathname, const tchar *epwname, const tchar *gconvname, const tchar *flinkpath, int openflag, const tchar *nettempname, int *error, int dictype=0 );
tnstrbuf CreateTxdFileName(const tchar *dicname);

#if MIXDIC && SQUFONT
extern bool wSingleByte;
extern bool jSingleByte;
extern bool eSingleByte;
extern bool pSingleByte;
#endif

#if MIXDIC
inline bool CheckWordLen( const tchar *str, const TKCodeTranslateSet *trans )
	{ return _kcslen(_kwstr( str, trans )) <= LWORD; }
#else
inline bool CheckWordLen( const _kchar *str )
	{ return _kcslen(str) <= LWORD; }
#endif

#if MIXDIC
#define	CHK_WORD_LEN( str ) CheckWordLen( str )
#else
#define	CHK_WORD_LEN( str )
#endif

#include "dicUtil.h"

// debug //
extern int DicLogEnabled;

#endif	// __DIC_H

