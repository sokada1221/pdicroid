#include	"tnlib.h"
#pragma	hdrstop
#include "pdconfig.h"
#ifdef USE_OLD
#include	"ole2s.h"
#endif
#include	"pdtypes.h"
#include "dic.h"
#include "filestr.h"
#include "japa.h"
#if INETDIC
#include "inetdic.h"
#endif
#include "LangProc.h"
#include "LangProcMan.h"
#include "md5.h"
#include "pdstrlib.h"
#if DICTXDUP
#include "dictype.h"
#endif
#include "fileio.h"

//Note:
// 辞書に変更のある処理を行った場合は、
//		generation++;
//	この処理を行うこと


#define	__kwstr(var)	_kwstr(var, GetKCodeTrans())	// for keyword


#define	DATA_BLOCKSIZE	1024

#define	EXTBLOCK	1024	// extheaderのblock単位(byte)

#define	CAST_PDICBASE_PDIC(c)	(Pdic*)(c)

#define	INETDIC_LOCK	0	// INETDICにlockは必要か？

#define	ODA_CLOSEDELAY	300	// [mSec]
#define	__assert_oda_open	__assert(!TempClosed)

#if DICTXDUP
#define	__TXDWriteReq__		if (!TXDWriteReq && TXDEnabled()){ TXDWriteReq = true; IdleProcs[this]++; }
#define	__TXDCheckUpdate__	if (TXDEnabled()){ TXDCheckUpdate(); }
#else
#define	__TXDWriteReq__
#define	__TXDCheckUpdate__
#endif

class TReadWriteLockX : public TReadWriteLock {
typedef TReadWriteLock super;
public:
	void Lock(){ super::WriteLock(); }
	void Unlock(){ super::WriteUnlock(); }
};

#define	DefSearchWriteLock() \
	autolock<TReadWriteLockX> __rwlock(*(TReadWriteLockX*)&SearchSem)

#if NETDIC || DICTXDUP
#if INETDIC_LOCK
#define	DefReadLockObj() \
	ReadLockObject __lock__(NDic);

#define	DefWriteLock( ret ) \
	WriteLockObject __lock__(NDic)
#define	DefWriteLockRetry( ret ) \
	WriteLockObject __lock__(NDic)
#define	DefWriteLockRetryParam( ret, flag ) \
	WriteLockObject __lock__(NDic)
#else	// !INETDIC_LOCK
#define	DefReadLockObj()
#define	DefWriteLock( ret )
#define	DefWriteLockRetry( ret )
#define	DefWriteLockRetryParam( ret, flag )
#endif	// !INETDIC_LOCK

#if INETDIC
/* INETがあるとき */
#define	NetRecord( word, japa ) \
	if ( NetworkMode == NM_INET ){ \
		((InetDic*)NDic)->Record( word, japa ); \
	}
#define	NetUpdate( word, japa ) \
	if ( NetworkMode == NM_INET ){ \
		((InetDic*)NDic)->Update( word, japa ); \
	}
#define	NetErase( word ) \
	if ( NetworkMode == NM_INET ){ \
		((InetDic*)NDic)->Erase( word ); \
	}
#define	NetRename(oldword,newword) \
	if (NetworkMode == NM_INET ){ \
		((InetDic*)NDic)->Rename( oldword, newword ); \
	}
#define	DefOldAttr(x)	wa_t oldattr = x
#define	NetAttr( word, oldattr, attr ) \
	if (NetworkMode == NM_INET){ \
		((InetDic*)NDic)->Attr( word, oldattr, attr ); \
	}
#define	NetLevel( word, attr ) \
	if (NetworkMode == NM_INET){ \
		((InetDic*)NDic)->Level( word, attr ); \
	}
#define	NetMemory( word, attr )	\
	if (NetworkMode == NM_INET){ \
		((InetDic*)NDic)->Memory( word, attr ); \
	}
#else // !INETDIC
/* INETがないとき */
#define	NetRecord( word, japa )
#define	NetUpdate( word, japa )
#define	NetErase( word )
#define	NetRename( oldword, newword )
#define	DefOldAttr( x )
#define	NetAttr( word, oldattr, attr )
#define	NetLevel( word, level )
#define	NetMemory( word, flag )
#endif
int PdicBase::BSearch( const _kchar * word, bool forfast, AllSearchParam *all )
{
	DefReadLockObj();
	if (!ODAROnlyOpen())
		return -1;
	__TXDCheckUpdate__;
	return super::_BSearch( word, forfast, all );
}
bool PdicBase::SetAllSearch( const tchar * word, SrchMode mode, GENERICREXP *regp, AllSearchParam *all )
{
	DefReadLockObj();
	if (!ODAROnlyOpen())
		return false;
	__TXDCheckUpdate__;
	return super::SetAllSearch( word, mode, regp, all );
}
int PdicBase::JumpAS_( const tchar *word, bool fBack, bool fSameBack, AllSearchParam *all)
{
	if (!ODAROnlyOpen())
		return -1;
	__TXDCheckUpdate__;
	return super::JumpAS_(word, fBack, fSameBack, all);
}
int PdicBase::JumpAS_k( const _kchar *word, bool fBack, bool fSameBack, AllSearchParam *all)
{
	if (!ODAROnlyOpen())
		return -1;
	__TXDCheckUpdate__;
	return super::JumpAS_k(word, fBack, fSameBack, all);
}
#else	// !(NETDIC || DICTXDUP)
#define	DefReadLockObj()
#define	DefWriteLock( ret )
#define	DefWriteLockRetry( ret )
#define	NetRecord( word, japa )
#define	NetUpdate( word, japa )
#define	NetErase( word )
#define	NetRename( oldword, newword )
#define	DefOldAttr( x )
#define	NetAttr( word, oldattr, attr )
#define	NetLevel( word, level )
#define	NetMemory( word, flag )
#define	DefWriteLockRetryParam( ret, flag )
#endif


#ifdef SWAP_ENDIAN
static void SwapHeader( HEADER *header );
#endif

#define	quadcopy(dst, src)	*(int*)(dst) = *(int*)(src)
#define	quadequ(q1, q2)		(*(unsigned int*)(q1) == *(unsigned int*)(q2))
struct DigestItem {
	const char *data;
	int length;
};
static void digest(const DigestItem *items, int num, uint8_t *out)
{
	MD5_CTX md5;
	MD5Init( &md5 );
	for (int i=0;i<num;i++){
		MD5Update( &md5, (const unsigned char*)items[i].data, items[i].length);
	}
	MD5Final( &md5 );
	memcpy(out, md5.digest, 16);
}
static void digest(const uint8_t *buf, int size, uint8_t *out)
{
	MD5_CTX md5;
	MD5Init( &md5 );
	MD5Update( &md5, buf, size );
	MD5Final( &md5 );
	memcpy(out, md5.digest, 16);
}
inline void digest(const char *buf, int size, uint8_t *out)
{ digest((const uint8_t*)buf, size, out); }
static void xor_bytes(const uint8_t *src, uint8_t *dst, int size)
{
	for (int i=0;i<size;i++){
		*dst++ ^= *src++;
	}
}

//
// HEADER struct
//
// The size of buf is 4 bytes or more.
void HEADER::CalcChecksum(uint8_t *buf)
{
	MD5_CTX md5;
	MD5Init( &md5 );
	int sumsize = (uint_ptr)((HEADER*)0)->checksum;
	MD5Update( &md5, (unsigned char*)this, sumsize );
	MD5Final( &md5 );
	quadcopy(buf, md5.digest);
}
void HEADER::SetChecksum()
{
	CalcChecksum(this->checksum);
}
bool HEADER::ValidateChecksum()
{
	uint8_t buf[4];
	CalcChecksum(buf);
	return memcmp(buf, this->checksum, sizeof(this->checksum))==0;
}

bool HEADER::IsXcScheme() const
{
	return false;
}

//
// PdicBase class
//
PdicBase::PdicBase()
	:file(( int &)error)
{
	Backup = NULL;
#ifndef SMALL
	auto_write = true;
#endif
	readonly = false;
	header = new HEADER;
#if defined(USE_JLINK)
	linkattr = OLA_NORMAL;
#endif
	Renaming = false;
#if USE_ODA
	TempCloseReq = false;
	TempClosed = false;
	TempROnly = false;
	NeedFullOpen = false;
	ODAProgress = false;
	ODANext = NULL;
#endif
#if DICTXDUP
	TXDFileTime = -1;
	TXDWriteReq = false;
	TXDUpdating = false;
	RecordLock = true;
#endif
}

PdicBase::~PdicBase()
{
	Close();
	if ( header ){
		delete header;
	}
}

/*************************
	辞書の新規作成
*************************/

HEADER _header = {
#if defined(_UNICODE) && !MIXDIC	// for little-endian
	_T("? ======= UNICODE Dictionary for PDIC ======="),
#else
	"             =============== Dictionary for PDIC ===============       　       ",
#endif
	_KTEXT(""),
	DIC_VERSION,
	0, 0, 0, 0,
	HEADER_SIZE,	// header_size
	0,
	-1, 0, 0, 0,
	SK_NORMAL,
#if defined(_UNICODE) && !MIXDIC
	DICID_UNICODE,
#else
	DICID_NORMAL,
#endif
	1			// attrlen
	,OS_DEFAULT		// os
	,0L			// olenumber
	,{0}	// dummy_lid
	,0	// index_blkbit
	,0	// dummy0
	,0	// extheader
	,-1	// empty_block2
	,0	// nindex2
	,0	// nblock2
	,{0,0,0,0,0,0,0,0}
	,0	// update_count or dummyd0
	,{0,0,0,0}	// dummy00
	,{0,0,0,0,0,0,0,0}	// dicident
	,{0}	// derefid
	,0		// generation
	,0		// langprocid
	,{0}	// dummyt
	,{0}	// checksum
	,{0}	// dummyx
};

int PdicBase::Create( const DICCREATEPARAM *dcp )
{
	return Create(dcp->filename, dcp->title, dcp->size, dcp->id);
}

#if 0	// いずれtnlibへ→使っていない？2015.2.15
template <class T>
class auto_ptr_ar : public auto_ptr<T> {
typedef auto_ptr<T> super;
public:
	auto_ptr_ar(T *p=0)
	:super(p){}
	~auto_ptr_ar() { delete[] get(); }
};
#endif	// 1

int PdicBase::Create( const tchar *fname, const tchar *dictitle, int /*dicsize*/, int dictype, int lang_proc_id)
{
	if ( !header ){
		error = 11;
		return -1;
	}
	if (file.create(fname) == -1){
		return -1;
	}

#ifdef EXTHEADER
	ExtHeaderRead = false;
	ExtHeader.clear();
#endif

	/*---- ヘッダーの書込み ----*/
	uint8_t *block = new uint8_t[HEADER_SIZE];
	if ( !block ){
		file.close();
		error = 11;	// メモリ不足
		return -1;
	}

	do {
		short blk_size;
		short index_num;
		blk_size = DATA_BLOCKSIZE;
		index_num = 0;

		memset( block, 0, HEADER_SIZE );
		_header.block_size = blk_size;
	#if defined(_UNICODE) && !MIXDIC
	//	if ( dictype & DICID_UNICODE )
		{
			_header.headername[0] = UNICODE_BYTEORDER;
		}
	#endif
		_header.index_block = (tindex)index_num;
		//_header.dicorder = (uint8_t)dicorder;
		_header.dictype = (uint8_t)dictype;
		_header.lang_proc_id = 
			(lang_proc_id==-1 ? LangProcMan.GetStdLangProcId() : lang_proc_id);
		memset( _header.dictitle, '\0', L_DICTITLE );
		_kcsncpy( _header.dictitle, _kstrn(dictitle), _KBYTETOLEN(L_DICTITLE)-1 );

		GenerateDicId(_header.dicident, fname);

		memcpy(block, &_header, sizeof(HEADER));
		_fmemcpy( header, &_header, sizeof(HEADER));
		file.seek( HEADER_LOC );
		file.write(block, HEADER_SIZE);
	} while (0);

	delete[] block;
	
	if ( super::Open( new HPdicIndex( error, file ), new HPdicData( error, file ), *header ) == -1 ){
		file.close();
		return -1;
	}
	AddMap(fname);
	return InitHeader( );
}

void PdicBase::GenerateDicId(uint8_t *id, const tchar *filename)
{
	srand(clock() + rand() + (uint)(uint_ptr)filename);
	int fname_len = _tcslen(filename);
	for (int i=0;i<8;i++){
		id[i] = (uint8_t)(rand() ^ filename[rand()*fname_len/RAND_MAX]);
	}
}

int PdicBase::Open( const tchar * fname, bool _readonly )
{
	int ret;
	if (IsOpened()){
		error = DICERR_OPEN_NOCREATE;
		return -1;
	}
#ifdef EXTHEADER
	ExtHeaderRead = false;
	ExtHeader.clear();
#endif
#if NETDIC
	if (NetworkMode==NM_INET && readonly){
		NetworkMode = NM_NONE;	// 強制的にOFF
	}
	if ( NetworkMode != NM_NONE ){
		auto_write = true;
		// テンポラリファイル名作成
		if ( !TemporaryName[0] ){
			KeepTemporary = false;
			if ( !GetTemporaryFileName( _T("TD"), TemporaryName ) ){
				error = DICERR_NOTEMP;
				return -1;
			}
		} else KeepTemporary = true;
#if INETDIC
		if ( NetworkMode == NM_INET )
		{
			NDic = new InetDic( CAST_PDICBASE_PDIC(this) );
		}
#endif
		NDic->SetFileName( fname, TemporaryName );

	}
#endif
	readonly = _readonly;
	if ( _readonly ){
		if (file.read_open(fname) == -1){
			goto jerr2;
		}
	} else {
		if (file.Open(fname) == -1){
			goto jerr2;
		}
	}
	if (ReadHeader() == -1){
		goto jerr1;
	}
	if ( !_readonly && (header->GetDicID() & DICID_CRYPT) ){
		error = DICERR_READONLY;
		goto jerr1;
	}
#ifdef USE_BOCU1
	bocu = header->dictype & DICID_BOCU1 ? true : false;
#endif
	if ( super::Open( new HPdicIndex( error, file ), new HPdicData( error, file ), *header ) == -1 ){
		goto jerr1;
	}
	ret = InitHeader( );
#if NETDIC
	if ( NetworkMode != NM_NONE ){
		if (!NDic->Open(readonly)){
			switch (NDic->GetErrorCode()){
				case NDERR_NOTEMP:
					error = DICERR_NOTEMP;
					break;
				case NDERR_INETDICDLL:
					DBW("No InetDic.dll");
					error = DICERR_NODLL;
					break;
				case NDERR_NOURL:
					error = DICERR_NOURL;
					break;
				default:
					__assert__;
					break;
			}
			goto jerr1;
		}
	}
#endif
	if (ret!=-1){ NeedFullOpen = false; }
	return ret;

jerr1:;
	file.close();
jerr2:;
#if NETDIC
	_delete(NDic);
#endif
	return -1;
}

int PdicBase::OpenRetry(const tchar *fname, bool readonly)
{
	int r = Open( fname, readonly );
	if ( r ){
		if ((error==DICERR_READONLY && !readonly)
			|| (error==DICERR_ALREADY_OPEN && this->readonly && !readonly) ){
			return Open( fname, true );	// リードオンリィで再オープン
		}
	}
	return r;
}

void PdicBase::Close( )
{
	if ( !IsOpened() )
		return;

#if DICTXDUP
	if (TXDWriteReq){
		TXDUpdate();
	}
	TXDFileTime = -1;
#endif

#if NETDIC
	__assert(NetworkMode || !NDic);
	if (NDic)
		NDic->Close();
#endif

#if USE_ODA
	if (file.isOpen())
#endif
	{
		CloseFlush();
		super::Close();
	}
#if USE_ODA
	else {
		super::Close();
	}
#endif

#if NETDIC
	if (NDic)
		NDic->SetFileLastDate( file );
#endif

#if USE_ODA
	if (file.isOpen())
#endif
	{
		file.close();
	}

#if NETDIC
	if (NDic){
		delete NDic;
		NDic = NULL;
	}
	if (NetworkMode){
		if ( !KeepTemporary )
			DeleteFile( TemporaryName );
		NetworkMode = NM_NONE;
	}
#endif

#if USE_ODA
	if (TempCloseReq){
		TempCloseReq = false;
		ODARemoveLink(this);
	}
	TempClosed = false;
	TempROnly = false;
#endif

	IdleProcs.erase(this);
}

// 辞書IDなどheader情報は保持して登録語をゼロにする
//TODO: 直にClearできたほうがいいが・・・暇なとき
// force_write : readonlyであっても、最後強制的に書き込み可能でopenする
int PdicBase::ClearAll(bool force_write)
{
	if ( !IsOpened() ){
		error = DICERR_NOT_OPENED;
		return -1;
	}
	__assert_oda_open;
	struct TDicParam {
		bool readonly;
		tnstr filename;
		HEADER header;
	} p;
	p.readonly = readonly;
	p.filename = GetFileName();
	p.header = *header;
#ifdef EXTHEADER
	ReadExtHeader();
	THeaderArray exthdr;
	exthdr.move(ExtHeader);	// backup
#endif
	Close();

#if DICTXDUP
	tnstr txdname = TXDName;	// backup (Create()を呼ぶとまたこの関数が呼ばれて無限loopに)
	TXDName.clear();
	if (Create(p.filename, _t(""), 0, p.header.dictype, p.header.lang_proc_id)){
		DBW("%d: Create error : %d", __LINE__, GetErrorCode());
		goto jerr;
	}
	_kcsncpy( header->dictitle, p.header.dictitle, _KBYTETOLEN(L_DICTITLE)-1 );
	memcpy(header->dicident, p.header.dicident, sizeof(header->dicident));
	Close();
#endif

	if (Open(p.filename, force_write ? false : p.readonly)){
		DBW("%d: Open error : %d", __LINE__, GetErrorCode());
		goto jerr;
	}
#ifdef EXTHEADER
	ReadExtHeader();
	ExtHeader.move(exthdr);
#endif
#if DICTXDUP
	TXDName = txdname;
#endif
	return 0;

jerr:;
#if DICTXDUP
	TXDName = txdname;
#endif
	return -1;
}

//TODO: Flushと整合とること！
// Flush for Close
void PdicBase::CloseFlush()
{
	__assert_oda_open;
	data->Flush();
	data->CloseEmptyList();
	index->Flush();
	bool modified = false;
#if defined(USE_JLINK)
	if (header->olenumber != iObjectNumber){
		header->olenumber = iObjectNumber;
		modified = true;
	}
#endif
#ifdef EXTHEADER
	if (ExtHeaderMod){
		if (WriteExtHeader()==1){
			// ここでindex&headerの書き込みを行った
			return;
		}
	}
#endif
	if (file.isModify() || modified){
		//辞書が修正されていたら
		ShrinkIndex();
		header->generation++;
		WriteHeader();
		if (LogEnabled){
			if (Updated){
				DoBackup();
				Updated = false;
			}
		}
		file.cancelModify();
	}
}

#if USE_ODA
#define	ODA_MAX_RETRY		30
#define	ODA_RETRY_DELAY(n)	(100+(header->dicident[n]&0x3F))
// request:
//	true : requestのみ
//	false : すぐにclose
bool PdicBase::ODAClose(bool request)
{
	if (!file.isOpen() || TempClosed){
		error = DICERR_PARAMETER;
		return false;
	}

	if (NeedFullOpen){
		TempClosed = true;
		return true;
	}

	if (request){
		if (!TempCloseReq){
			CloseReqTime = GetTickCount();
			TempCloseReq = true;
			ODAAddLink(this);
		}
		//DBW("%08X:ODARCloseReq", this);
		return true;
	}
	if (TempCloseReq){
		TempCloseReq = false;
		ODARemoveLink(this);
	}

	if (!TempROnly){
		CloseFlush();
	}

	file.close();

	// get file info //
	int fd;
	for (int i=0;;i++){
		fd = _topen(file.GetFileName(), O_RDONLY);
		if (fd>=0){
			fstat(fd, &LastFileStat);
			FileLength = filelength(fd);
			close(fd);
			break;
		}
		if (i>=ODA_MAX_RETRY){
			FileLength = -1;
			break;
		}
		Sleep(ODA_RETRY_DELAY(0));
	}

	TempROnly = false;
	TempClosed = true;
	//DBW("%08X:ODARClose", this);
	return true;
}
bool PdicBase::ODAReopen()
{
	if (TempCloseReq){
		TempCloseReq = false;
		ODARemoveLink(this);
	}

	if (!TempClosed && !TempROnly && !NeedFullOpen){
		return true;
	}

	if (TempROnly){
		if (!readonly){
			file.close();
		}
		TempROnly = false;
	} else {
		if (!ODAReopenCommon())
			return false;
	}

	if (!file.isOpen()){
		int ret;
		tnstr filename(file.GetFileName());
		for (int i=0;;i++){
			if (readonly){
				ret = file.read_open(filename);
			} else {
				ret = file.Open(filename);
			}
			if (ret>=0)
				break;
			if (i>ODA_MAX_RETRY){
				DBW("Open error@ODAReopen : %d", file.GetErrorCode());
				if (!TempClosed){
					// bad temporary closed.
					TempClosed = true;
					FileLength = -1;
				}
				return false;
			}
			Sleep(ODA_RETRY_DELAY(1));
		}
	}

	TempClosed = false;
	//DBW("%08X:ODAReopen", this);
	return true;
}
bool PdicBase::ODAROnlyOpen()
{
#if DICTXDUP
	if (TXDUpdating)
		return true;
#endif

	if (TempCloseReq){
		TempCloseReq = false;
		ODARemoveLink(this);
	}

	if ((TempROnly || !TempClosed) && !NeedFullOpen){
		return true;
	}

	if (!ODAReopenCommon())
		return false;

	if (file.isOpen()){
		// ODAReopenCommonでfull openされた
	} else {
		DWORD start = GetTickCount();
		for (int i=0;;i++){
			tnstr filename(file.GetFileName());
			int ret = file.read_open(filename);
			if (ret>=0)
				break;
			if (i>ODA_MAX_RETRY){
				DBW("Open error@ODAROnlyOpen : %d %d %d", file.GetErrorCode(), GetTickCount() - start, i);
				error = DICERR_OPEN_NOCREATE;
				return false;
			}
			Sleep(ODA_RETRY_DELAY(2));
		}
		TempROnly = true;
	}
	TempClosed = false;
	//DBW("%08X:ODAROnlyOpen", this);
	return true;
}
bool PdicBase::ODAReopenCommon()
{
	int fd;
	int len;
	struct stat sb;

	if (!NeedFullOpen){
		for (int i=0;;i++){
			fd = _topen(file.GetFileName(), O_RDONLY);
			if (fd>=0)
				break;
			if (i>ODA_MAX_RETRY){
				DBW("Open erorr : %d : %ws", file.GetErrorCode(), file.GetFileName());
				error = DICERR_OPEN_NOCREATE;
				return false;	// file locked/deleted??
			}
			Sleep(ODA_RETRY_DELAY(3));
		}

		fstat(fd, &sb);
		len = filelength(fd);
		close(fd);
		//len = 0;	// for debug
	}
	if (NeedFullOpen
		|| FileLength!=len
		|| LastFileStat.st_mtime!=sb.st_mtime){
		DBW("Change length or date - full open: %d", ODAProgress);
		if (ODAProgress)
			return false;
		ODAProgress = true;
		bool ret = true;
		{
			Close();
			tnstr filename(file.GetFileName());	// the current file.filename will be destroyed at the following Open() method.
			for (int i=0;;i++){
				if (Open(filename, readonly)>=0)
					break;
				if (i>ODA_MAX_RETRY){
					NeedFullOpen = true;
					ret = false;		// full open error
					DBW("ODA Open retry error!! : %ws, %d", file.GetFileName(), error);
					break;
				}
				Sleep(ODA_RETRY_DELAY(4));
			}
		}
		ODAProgress = false;
		TempROnly = false;
		return ret;
	}
	return true;
}
// ODA Link Management //
PdicBase *PdicBase::ODALink = NULL;
int PdicBase::ODACloseDelay = 500;	// delay time [mSec]
// return:
//	false : no more items (can stop timer)
//	true  : some items exist in links.
bool PdicBase::ODACloseTimerProc()
{
	if (!ODALink)
		return false;
	unsigned now = GetTickCount();
	do {
jretry:;
		PdicBase *p = ODALink;
		while (p){
			if (p->ODACloseReqExpired(now)){
				if (p->ODAClose(false))
					goto jretry;
			}
			p = p->ODANext;
		}
		break;
	} while (0);
	return ODALink ? true : false;
}
void PdicBase::ODAAddLink(PdicBase *dic)
{
	if (ODALink){
		dic->ODANext = ODALink;
		ODALink = dic;
	} else {
		ODALink = dic;
	}
}
void PdicBase::ODARemoveLink(PdicBase *dic)
{
	if (ODALink==dic){
		ODALink = dic->ODANext;
		dic->ODANext = NULL;
		return;
	}

	PdicBase *p = ODALink;
	while (p){
		if (p->ODANext==dic){
			p->ODANext = dic->ODANext;
			dic->ODANext = NULL;
			return;
		}
		p = p->ODANext;
	}
}

#endif	// USE_ODA

#if DICTXDUP
#include "DicConv.h"

static const _mchar TAG_TXD_SIZE[] = _MTEXT("pdic-txd-size");
static const _mchar TAG_TXD_TIME[] = _MTEXT("pdic-txd-time");
static const _mchar TAG_TXD_TYPE[] = _MTEXT("pdic-txd-type");

void PdicBase::SetTextDic(const tchar *filename, int dictype)
{
	TXDName = filename;
	if (dictype){
		if (TXDType!=dictype){
			TXDType = dictype;
			WriteExtInteger(TAG_TXD_TYPE, TXDType);
		}
	}
}

int PdicBase::GetTextDicType()
{
	if (TXDType==0)
		TXDType = ReadExtInteger(TAG_TXD_TYPE, 0);
	return TXDType;
}

// text -> dic
bool PdicBase::TXDCheckUpdate()
{
	if (!TXDEnabled() || !IsOpened()){
		return true;
	}
	if (TXDUpdating)
		return true;

	if (TXDFileTime==-1 && !TempClosed){
		TXDFileTime = ReadExtInteger(TAG_TXD_TIME, 0);
	}

	int ft = TXDGetFileTime();
	if (ft==TXDFileTime){
		return true;
	}

	TXDUpdating = true;
	bool ret = TXDConvert();
	if (ret){
		WriteExtInteger(TAG_TXD_TIME, ft);
		TXDFileTime = ft;
		Flush();
	}
	TXDUpdating = false;
	return ret;
}
// dic -> text
bool PdicBase::TXDUpdate()
{
	bool ret = false;
	TDicConverter dconv;
	if (TempClosed){
		DBW("TXDUpdate : TempClosed");
		//TODO: 何もやらないで大丈夫？ここにはこないはず？
	} else {
		TXDUpdating = true;
		if (dconv.ConvertFromPdic(GetPdic(), TXDName, GetTextDicType())){
			TXDWriteReq = false;
			IdleProcs[this]--;
			TXDFileTime = TXDGetFileTime();
			WriteExtInteger(TAG_TXD_TIME, TXDFileTime);
			ret = true;
		}
		TXDUpdating = false;
	}
	return ret;
	
}
int PdicBase::TXDGetFileTime()
{
	return (int)FileTime(TXDName);
}
bool PdicBase::TXDConvert()
{
	// Convert the text file
	TDicConverter dconv;
	if (TempClosed){
		// dictionary closed physically
		if (!dconv.ConvertToPdic(TXDName, GetFileName(), GetTextDicType()))
			return false;
		TXDWriteReq = false;
		//NeedFullOpen = true;
	} else {
		// dictionary opened physically
		bool p_readonly = readonly;
		if (ClearAll(true))
			return false;
		if (!dconv.ConvertToPdic(TXDName, GetPdic(), GetTextDicType()))
			return false;
		TXDWriteReq = false;
		Close();
		tnstr filename = GetFileName();
		Open(filename, p_readonly);
	}
	SetTextDic(TXDName, 0);
	return true;
}
#endif	// DICTXDUP

const tchar *PdicBase::GetFileName()
{
	return file.GetFileName( );
}


/////////////////////////////////////////////////////
// Headers
/////////////////////////////////////////////////////
// ヘッダー情報に基づく初期化、_open(),Create()共通の初期化
int PdicBase::InitHeader( )
{
#if defined(USE_JLINK)
	iObjectNumber = header->olenumber;
#endif
#ifndef SMALL
	SetMode( TRUE, FALSE );	// 通常モード
#endif
	InitReadPare();
	return 0;
}

int PdicBase::ReadHeader( )
{
	__assert_oda_open;
	if (file.seek(HEADER_LOC) == -1){
		return -1;
	}
	header->header_size = 0;
	file.read( header, sizeof(HEADER));		//&header
	if ( DE_SWAP(header->header_size) != HEADER_SIZE ){
		error = DICERR_NOTPDIC;
		return -1;
	}
#ifdef SWAP_ENDIAN
	SwapHeader( header );
#endif
#if 0
	if (!header->ValidateChecksum()){
		// header corrupted?
		//TODO: 正式版ではコメントを外す
		error = DICERR_HEADERSUM;
		return -1;
	}
#else
	__assert(header->ValidateChecksum());
#endif
	uint8_t ver1 = HHI_BYTE(header->version);
#if defined(ND4ONLY)
	if ( ver1 != (DIC_VERSION>>8) )
#endif
	{
		error = DICERR_DICVERSION;
		return -1;	//辞書のバージョンが違う
	}
	if ( ((header->dictype & ~DICID_OPTION) != (DICID_DEFAULT&~DICID_OPTION))
#if defined(_UNICODE) && !MIXDIC
		|| (header->headername[0] != UNICODE_BYTEORDER)
#endif
		)
	{
//		DBW(":%X", header->dictype);
		if (!(file.isReadOnly() && (header->dictype&DICID_TREE))){
			// readonlyでDICID_TREEならOK
			error = DICERR_NOT_SUPPORT;
			return -1;	// サポートされていない辞書形式
		}
	}
	if ( header->os != OS_DEFAULT ){
		error = DICERR_NOT_SUPPORT;
		return -1;	// サポートされていないOS
	}
#if defined(USE_DICORDER)
	if ( header->dicorder > SK_DESCENDING )
#else
	if ( header->dicorder != SK_NORMAL )
#endif
	{
		error = DICERR_NOT_SUPPORT;
		return -1;	// サポートされていない順序
	}
	return 0;
}

int PdicBase::WriteHeader( )
{
	__assert_oda_open;
#if 0
	header->nindex = (tindex)index->GetIndexNum();
	header->nblock = (tindex)data->GetNBlock();
	header->empty_block = (tindex)data->GetEmptyBlock();
#endif
	header->nword = data->GetNumWord();
	if (header->nblock < header->nindex)
		header->nblock = header->nindex;
	header->index_block = (ushort)(( index->GetIndexSize() + header->block_size - 1 ) / header->block_size);
	header->nindex2 = index->GetIndexNum();
	header->nblock2 = data->GetNBlock();
	header->empty_block2 = data->GetEmptyBlock();
	header->index_blkbit = (uint8_t)index->Has32();
#if defined(USE_JLINK)
	header->olenumber = iObjectNumber;
#endif

	header->SetChecksum();

	if (file.seek(HEADER_LOC) == -1){
		return -1;
	}
#ifdef SWAP_ENDIAN
	HEADER _header;
	memcpy( &_header, header, sizeof(HEADER) );
	SwapHeader( &_header );
	file.write( &_header, sizeof(HEADER) );
#else
	file.write( header, sizeof(HEADER) );
#endif
	return 0;
}

#ifdef	PDICW
const HEADER FAR *PdicBase::ReadHeader( const tchar *filename, unsigned int *size )
{
	if ( IsOpened() )
		return header;

	if ( file.read_open( filename ) == -1 ){
		return NULL;
	}
	if ( ReadHeader() == -1 ){
		file.close();
		return NULL;
	}
	if (size){
		*size = file.get_length();
	}
	file.close();
	return header;
}
#endif

#ifdef SWAP_ENDIAN
static void SwapHeader( HEADER *header )
{
	int i;
#ifdef _UNICODE
	for ( i=0;i<sizeof(header->headername)/sizeof(tchar);i++ ){
		header->headername[i] = DE_SWAP(header->headername[i]);
	}
	for ( i=0;i<sizeof(header->dictitle)/sizeof(tchar);i++ ){
		header->dictitle[i] = DE_SWAP(header->dictitle[i]);
	}
#endif
	SWAP_SELF(header->version);
	SWAP_SELF(header->lword);
	SWAP_SELF(header->ljapa);
	SWAP_SELF(header->block_size);
	SWAP_SELF(header->index_block);
	SWAP_SELF(header->header_size);
	SWAP_SELF(header->index_size);
	SWAP_SELF(header->empty_block);
	SWAP_SELF(header->nindex);
	SWAP_SELF(header->nblock);
	SWAP_SELF(header->nword);
	SWAP_SELF(header->olenumber);
	SWAP_SELF(header->lid_word);
	SWAP_SELF(header->lid_japa);
	SWAP_SELF(header->lid_exp);
	SWAP_SELF(header->lid_pron);
	SWAP_SELF(header->lid_other);
	SWAP_SELF(header->extheader);
	SWAP_SELF(header->empty_block2);
	SWAP_SELF(header->nindex2);
	SWAP_SELF(header->nblock2);
}
#endif
#ifndef ND4ONLY
#ifdef SWAP_ENDIAN
#error Not yet making
#endif
#endif

#ifdef EXTHEADER
THeaderItem::THeaderItem()
{
	data = NULL;
	length = 0;
}
THeaderItem::~THeaderItem()
{
	if (data)
		delete[] (char*)data;
}
THeaderArray::THeaderArray()
{
}
THeaderArray::~THeaderArray()
{
}
int THeaderArray::FindTag( const _mchar *tag )
{
	int i;
	for ( i=0;i<get_num();i++ ){
		if (!_mcscmp(tag,(_mchar*)(*this)[i].data)){
			// find
			return i;
		}
	}
	return -1;
}
// tagと一致するデータを探す
// データが文字列であった場合、return valueはNULL終端文字列としてそのまま扱える
const void *THeaderArray::FindData( const _mchar *tag, int *length )
{
	int index = FindTag( tag );
	if (index==-1)
		return NULL;
	void *top = (*this)[index].data;
	tag = (_mchar*)top;
	for(;*tag;){
		tag = _mnext(tag);
	}
	tag++;
	int datalen = (*this)[index].length - (int)((char*)tag-(char*)top);
	((short*)((char*)tag+datalen))[0] = '\0';	// NULL終端
	if (length){
		*length = datalen;
	}
	return tag;
}
void THeaderArray::SetData( const _mchar *tag, const void *data, int length )
{
	int index = FindTag( tag );
	if (index!=-1){
		delete[] (char*)(*this)[index].data;
	} else {
		add( new THeaderItem );
		index = get_num()-1;
	}
	int tagbytes = _mcsbyte1(tag);
	THeaderItem &item = (*this)[index];
	item.data = new char[tagbytes+length+2];
	item.length = tagbytes+length;
	memcpy(item.data,tag,tagbytes);
	memcpy((char*)item.data+tagbytes, data, length );
	((short*)((char*)item.data + tagbytes + length))[0] = '\0';	// NULL終端
}
// ReadExtHeader //
bool PdicBase::ReadExtHeader( )
{
	__assert_oda_open;
	if (ExtHeaderRead) return true;

	ExtHeaderMod = false;
	file.seek(header->header_size);
	int extsize = header->extheader;
	bool ret = false;
	if (extsize>0){
		for(;;){
			unsigned short size;
			if (file.read(&size,sizeof(short))<0){
				error = DICERR_READWRITE;
				goto jend;
			}
			if (size==0)
				break;
			extsize -= size-sizeof(short);
			if (extsize<0){
				// size error
#ifdef _DEBUG
				DBW("ReadExtHeader::extsize error");
#endif
				break;
			}
			uint8_t *buf = new uint8_t[size+sizeof(short)];	// あとで利用しやすいようにNULL終端用の領域を確保
			if (!buf){
				error = DICERR_MEMORY;
				goto jend;
			}
			if (file.read(buf,size)<0){
				error = DICERR_READWRITE;
				goto jend;
			}
			THeaderItem *item = new THeaderItem;
			item->data = buf;
			item->length = size;
			ExtHeader.add( item );
		}
	}
	ret = true;
	ExtHeaderRead = true;
jend:;
	return ret;
}
// return value : extheaderのバイト単位の厳密なサイズ
int PdicBase::_WriteExtHeader()
{
	__assert_oda_open;
	if (!ExtHeaderRead) return 0;	// operation error
	if (ExtHeader.get_num()==0){
		ExtHeaderMod = false;
		return 0;	// no extheader
	}

	int total = 0;
	unsigned short size;
	file.seek(header->header_size);
	for(int i=0;i<ExtHeader.get_num();i++ ){
		THeaderItem &item = ExtHeader[i];
		size = (unsigned short)item.length;
#ifdef _DEBUG
		if (size!=item.length){
			DBW("WriteExtHeader::size mismatch");
		}
#endif
		if (file.write( &size, sizeof(short) )<0){
			error = DICERR_READWRITE;
			return -1;
		}
		file.write( item.data, item.length );
		total += 2 + size;
	}
	size = 0;
	file.write(&size,sizeof(short));
	total += 2;
	if (total & (EXTBLOCK-1)){
		// null padding
		size = EXTBLOCK-(total&(EXTBLOCK-1));
		char *buf = new char[size];
		memset(buf,0,size);
		file.write(buf,size);
		delete[] buf;
	}
	ExtHeaderMod = false;
	//NotifyBackup();	// backupはuser data保護のためにあるから、ExtHeaderにユーザー入力情報がない限りはbackup対象にしない
	return total;
}
// 現在のExtHeaderのサイズを求める
int PdicBase::SizeExtHeader()
{
	if (!ExtHeaderRead) return 0;

	int total = 0;
	unsigned short size;
	for(int i=0;i<ExtHeader.get_num();i++ ){
		THeaderItem &item = ExtHeader[i];
		size = (unsigned short)item.length;
		total += 2 + size;
	}
	return total == 0 ? 0 : total+2;
}
// extheaderの書き込みによってindex部が移動する場合は、
// index部の書き込みも行う
// ただし、auto_write=falseの場合はindex&headerのdisk更新はしない
// return value :
//	-1 : error
//	 0 : no index rewrite
//   1 : rewrite index and header
int PdicBase::WriteExtHeader()
{
	__assert_oda_open;
	if (!ExtHeaderRead) return 0;	// no operation

	int size = SizeExtHeader();
	uint newextheader = (size + EXTBLOCK-1)&~(EXTBLOCK-1);

	int ret = 0;
	if (newextheader!=header->extheader){
		int diffsize = newextheader - header->extheader;
		if (ChangeIndexOffset( diffsize/DATA_BLOCKSIZE )){
			header->extheader = newextheader;
			if (auto_write){
				ShrinkIndex();
				WriteHeader();
				ret = 1;
			}
		} else {
			//TODO: error
			// どうする？
			return -1;
		}
	}

	return _WriteExtHeader() >= 0 ? ret : -1;
}
int PdicBase::ReadExtInteger( const _mchar *tag, int def )
{
	ReadExtHeader();

	int length;
	const char *data = (const char*)ExtHeader.FindData( tag, &length );
	if (data&&length>0){
		return atoi(data);
	} else {
		return 0;
	}
}
bool PdicBase::WriteExtInteger( const _mchar *tag, int value )
{
	if (!ReadExtHeader())
		return false;

	char data[20];
	itoa(value,data,10);
	ExtHeader.SetData( tag, data, strlen(data));

	ExtHeaderMod = true;
	return WriteExtHeader()!=-1;
}
bool PdicBase::ReadExtString_( const _mchar *tag, tnstr &s )
{
	if (!ReadExtHeader())
		return false;

	int length;
	const _mchar *data = (const _mchar*)ExtHeader.FindData( tag, &length );
	if (data&&length>0){
		s = _kmstr(data);
	}
	return true;
}
bool PdicBase::WriteExtString_( const _mchar *tag, const tchar *s )
{
	if (!ReadExtHeader())
		return false;

#if MIXDIC
	_kmstrdef( __s, s );
#else
	#define	__s	s
#endif

	ExtHeader.SetData( tag, (const _mchar*)__s, _mcsbyte(__s) );
	ExtHeaderMod = true;
	return WriteExtHeader()!=-1;
}
#if defined(_UNICODE) && !defined(__ANDROID__)	// Androidで必要？
bool PdicBase::ReadExtString_s( const _mchar *tag, TCharSingle &s )
{
	if (!ReadExtHeader())
		return false;

	int length;
	const _mchar *data = (const _mchar*)ExtHeader.FindData( tag, &length );
	if (data&&length>0){
		s = __mstr(data);
	}
	return true;
}
bool PdicBase::WriteExtString_s( const _mchar *tag, const char *s )
{
	if (!ReadExtHeader())
		return false;

	ExtHeader.SetData( tag, s, strlen(s) );
	ExtHeaderMod = true;
	return WriteExtHeader()!=-1;
}
#endif
#endif	// EXTHEADER

bool PdicBase::IsOpened() const
{
	return file.isOpen()
#if USE_ODA
		|| TempClosed
#endif
	;
}

void PdicBase::Flush(bool closing)
{
	__assert_oda_open;
	if (TempClosed) return;
	data->Flush();
	index->Flush();
#if defined(USE_JLINK)
	header->olenumber = iObjectNumber;
#endif
//  ShrinkIndex();

	WriteHeader();
	data->WriteEmptyBlocks();	//TODO: 常にwrite？
}

/////////////////////////////////////////////////////
// find/search
/////////////////////////////////////////////////////

// 大文字・小文字同一視しないので注意！
// 戻り値
//	0 : 見つからなかった
//  1 : 見つかった
//  -1: エラー
int PdicBase::_read( const _kchar *word, Japa *japa )
{
	DefReadLockObj();
	int r = BSearch( word );
	if ( r != 1 )
		return r;
	if ( !_kcscmp( word, getfword( ) ) ){
		if ( japa )
			getfjapa( *japa );
		return 1;
	}
	return 0;
}

int PdicBase::read( const tchar *word, Japa *japa )
{
	return _read( __kwstr(word), japa );
}

// 0 : 見つからない
// -1: エラー
// 1 : 見つかった
int PdicBase::_Find( const _kchar *word )
{
	if ( BSearch( word ) == -1 ) return -1;
	return strequ( word, __kwstr(getfword()) );
}

int PdicBase::_Find( const _kchar *word, Japa *japa, AllSearchParam *all)
{
	if (BSearch(word, false, all)==-1) return -1;
	if (!strequ(word, all?all->fw.fword:getfword())){
		return 0;
	}
	if (japa){
		getfjapa(*japa, all);
	}
	return 1;
}

// Is this deprecated.?
int PdicBase::Find( const tchar *word )
{
	return _Find( __kwstr(word) );
}

int PdicBase::Find( const tchar *word, Japa *japa, AllSearchParam *all)
{
	return _Find(__kwstr(word), japa, all);
}

// keywordのみでの比較
int PdicBase::FindK(const tchar *kword, Japa *japa, AllSearchParam *all)
{
	SetAllSearch(kword, (SrchMode)(SRCH_HEAD|SRCH_WORD), NULL, all);
	_KChar word;
	for (;;){
		int ret = NextAllSearch_k(word, japa, all);
		if (ret==AS_FOUND)
			return 1;
		if (ret==AS_CONTINUE)
			continue;
		return 0;
	}
}

/////////////////////////////////////////////////////
// read/update/erase
/////////////////////////////////////////////////////

int PdicBase::_record( const _kchar * word, const Japa &japa, int srchf )
{
	switch ( readonly ){
		case 1:
			error = DICERR_READONLY_WRITE;
			return -1;
		case 2:
			// 1995.9.4 条件付きで登録可能に
			if (_kcslen( word ) > LWORD){
				error = DICERR_WRITE_OLD;
				return -1;
			}
			break;
	}
	int wordlen = _kcslen(word);
	if ( !wordlen || wordlen > LWORD ){
		error = DICERR_LONGWORD;
		return -1;
	}
	if (!ODAReopen())
		return -1;

#if DICTXDUP
	if (RecordLock)
#endif
	{
		DefSearchWriteLock();
		DefWriteLockRetry( return -1 );
		return _record_nochk( word, wordlen, japa, srchf );
	}
#if DICTXDUP
	else {
		return _record_nochk( word, wordlen, japa, srchf );
	}
#endif
}
int PdicBase::_record_nochk( const _kchar *word, int wordlen, const Japa &japa, int srchf )
{
	if (srchf
#if NETDIC
		|| NetworkMode
#endif
		){
		if (BSearch(word) == -1){
			return -1;
		}
		if ( strequ(word, getfword() ) ){
#if NOUPDATEWORD
			if ( super::_erase() == -1 )
				return -1;
			if ( BSearch(word) == -1 )
				return -1;
			if ( super::_record( word, 0, japa ) == -1 )
				return -1;
#else
			if (super::_update(word, 0, japa) == -1){
				return -1;
			}
#endif
			NetUpdate( word, &japa );
		} else {
			if (super::_record(word, wordlen, japa) == -1){
				return -1;
			}
			NetRecord( word, &japa );
		}
	} else {
		if (super::_record(word, wordlen, japa) == -1)
		{
			return -1;
		}
	}
#ifndef SMALL
	if (auto_write)
#endif
	{
		Flush();
	}
	NotifyBackup();
	__TXDWriteReq__
	return 0;
}

//カーソル位置の単語の修正
int PdicBase::_update( const _kchar *word, const Japa &japa, bool reload )
{
	switch ( readonly ){
		case 1:
			error = DICERR_READONLY_WRITE;
			return -1;
		case 2:
			if (_kcslen( word ) > LWORD){
				error = DICERR_WRITE_OLD;
				return -1;
			}
			break;
	}
	int wordlen = _kcslen(word);
	if ( wordlen > LWORD ){
		error = 37;
		return -1;
	}
	if (!ODAReopen())
		return -1;

	if ( reload ){
		if (BSearch(word) == -1)
			return -1;
		if ( !strequ(word, getfword() ) )	// new word
			return _record( word, japa );
	}

#if DICTXDUP
	if (RecordLock)
#endif
	{
		DefSearchWriteLock();
		DefWriteLockRetryParam( return -1, reload );
		return _update_nochk( word, wordlen, japa );
	}
#if DICTXDUP
	else {
		return _update_nochk( word, wordlen, japa );
	}
#endif
}

int PdicBase::_update_nochk( const _kchar *word, int wordlen, const Japa &japa )
{
#if NOUPDATEWORD
	if ( super::_erase() == -1 )
		return -1;
	if ( BSearch(word) == -1 )
		return -1;
	if ( super::_record( word, wordlen, japa ) == -1 )
		return -1;
#else
	if ( super::_update(word, wordlen, japa) ){
		return -1;
	}
#endif
	NetUpdate( word, &japa );
#ifndef SMALL
	if (auto_write)
#endif
	{
		Flush();
	}
	NotifyBackup();
	__TXDWriteReq__
	return 0;
}

int PdicBase::_erase( const _kchar * word )
{
	switch ( readonly ){
		case 1:
			error = DICERR_READONLY_WRITE;
			return -1;
#if 0	// 1995.9.4 削除可能に
		case 2:
			error = DICERR_WRITE_OLD;
			return -1;
#endif
	}
	if (!ODAReopen())
		return -1;
	DefSearchWriteLock();
	DefWriteLockRetry( return -1 );

	if (BSearch(word) == -1) return -1;
	if ( !strequ( word, getfword() ) )
		return 0;		// 正常終了でいい？？？？(bug fix 1994.3.3)
	if (_erase()!=0)
		return -1;
	NotifyBackup();
	__TXDWriteReq__
	return 0;
}

int PdicBase::_erase( )
{
	switch ( readonly ){
		case 1:
			error = DICERR_READONLY_WRITE;
			return -1;
#if 0	// 1995.9.4 削除可能に
		case 2:
			error = DICERR_WRITE_OLD;
			return -1;
#endif
	}
	if (!ODAReopen())
		return -1;
	DefWriteLock( return -1 );	// retryはできない
	if (super::_erase() == -1){
		return -1;
	}
#ifndef SMALL
	if (auto_write)
#endif
	{
		Flush();
	}
	if (!Renaming){
		NetErase( getfword() );
		NotifyBackup();
		__TXDWriteReq__
	}
	return 0;
}

// 単語名のみの変更
int PdicBase::_Rename( const _kchar *oldword, const _kchar *newword, int newwordlen )
{
	switch ( readonly ){
		case 1:
			error = DICERR_READONLY_WRITE;
			return -1;
	}
	if (!newwordlen)
		newwordlen = _kcslen(newword);
	if (newwordlen>LWORD){
		error = DICERR_LONGWORD;
		return -1;
	}
	if (!ODAReopen())
		return -1;
	DefSearchWriteLock();
	DefWriteLockRetry( return -1 );

	Renaming = true;
	int ret = super::_Rename( oldword, newword, newwordlen );
	Renaming = false;
	if (ret){
		return -1;
	}
	NetRename( oldword, newword );
	NotifyBackup();
	__TXDWriteReq__
	return 0;
}

int PdicBase::record( const tchar * word, const Japa &japa, int srchf )
{
	if (LogEnabled){
		Updated = true;
		diclog(_t("record : %s /// %s / %s"), word, japa.japa.c_str(), japa.exp.c_str(), japa.pron.c_str());
	}
	return _record(__kwstr(word),japa,srchf); 
}

int PdicBase::update( const tchar *word, const Japa &japa )
{
	if (LogEnabled){
		Updated = true;
		diclog(_t("update : %s /// %s / %s"), word, japa.japa.c_str(), japa.exp.c_str(), japa.pron.c_str());
	}
	return _update( __kwstr(word), japa, true );
}

int PdicBase::erase( const tchar *word )
{
	if (LogEnabled){
		Updated = true;
		diclog(_t("erase : %s"), word);
	}
	return _erase( __kwstr(word) );
}

int PdicBase::rename( const tchar *oldword, const tchar *newword)
{
	return _Rename( __kwstr(oldword), __kwstr(newword), 0 );
}

/////////////////////////////////////////////////////
// Changing attributes
/////////////////////////////////////////////////////

//返り値：セットできれば 1
int PdicBase::change_attr(const tchar *word, wa_t attr, wa_t mask )
{
	if ( IsReadOnly() ){
		error = DICERR_READONLY_WRITE;
		return -1;
	}
	if (!ODAReopen())
		return -1;
	DefSearchWriteLock();
	DefWriteLockRetry( return -1 );

#if MIXDIC
	_kwstrdef( __word, word );
#else
	#define	__word	word
#endif
	uint8_t *p = _get_attr_ptr( __word );
	if ( !p ) return 0;
	wa_t newattr = (wa_t)(attr | ( *p & ~mask ));
	if (newattr != *p){
		DefOldAttr( *p );
		*p = newattr;
		if ( data->write(all.fw.pbn) == -1)
			return -1;
		NetAttr(__word,oldattr,newattr);
		generation++;
	}
	return 1;
}

#if MIXDIC
int PdicBase::set_attr( const tchar *word, int bit )
	{ return _set_attr( __kwstr(word), bit ); }
#endif
//返り値：セットできれば 1
int PdicBase::_set_attr( const _kchar *word, int bit )
{
	if ( IsReadOnly() ){
		error = DICERR_READONLY_WRITE;
		return -1;
	}
	if (!ODAReopen())
		return -1;
	DefSearchWriteLock();
	DefWriteLockRetry( return -1 );

	uint8_t *p = _get_attr_ptr( word );
	if ( !p ) return 0;
	wa_t newattr = (wa_t)(*p | (wa_t)bit);
	if (newattr != *p){
		DefOldAttr( *p );
		*p = newattr;
		if (data->write(all.fw.pbn) == -1)
			return -1;
		NetAttr(word,oldattr,newattr);
		generation++;
		NotifyBackup();
		__TXDWriteReq__
	}
	return 1;
}

#if MIXDIC
int PdicBase::reset_attr( const tchar *word, int bit )
	{ return _reset_attr( __kwstr(word), bit ); }
#endif
//返り値：リセットできれば 1
int PdicBase::_reset_attr( const _kchar *word, int bit )
{
	if ( IsReadOnly() ){
		error = DICERR_READONLY_WRITE;
		return -1;
	}
	if (!ODAReopen())
		return -1;
	DefSearchWriteLock();
	DefWriteLockRetry( return -1 );

	uint8_t *p = _get_attr_ptr( word );
	if ( !p ) return 0;
	wa_t newattr = (wa_t)(*p & (wa_t)~bit);
	if (newattr != *p){
		DefOldAttr( *p );
		*p = newattr;
		if (data->write(all.fw.pbn) == -1)
			return -1;
		NetAttr(word,oldattr,newattr);
		generation++;
		NotifyBackup();
		__TXDWriteReq__
	}
	return 1;
}

//返り値：反転できれば 1
int PdicBase::toggle_attr( const tchar *word, int bit )
{
	if ( IsReadOnly() ){
		error = DICERR_READONLY_WRITE;
		return -1;
	}
	if (!ODAReopen())
		return -1;
	DefSearchWriteLock();
	DefWriteLockRetry( return -1 );

#if MIXDIC
	_kwstrdef( __word, word );
#else
	#define	__word	word
#endif
	uint8_t *p = _get_attr_ptr( __word );
	if ( !p ) return 0;
	wa_t newattr = (wa_t)(*p ^ (wa_t)bit);
	if (newattr != *p){
		DefOldAttr( *p );
		*p = newattr;
		if (data->write(all.fw.pbn) == -1)
			return -1;
		NetAttr(__word,oldattr,newattr);
		generation++;
		NotifyBackup();
		__TXDWriteReq__
	}
	return 1;
}

// level分だけ増減させる
int PdicBase::add_level( const tchar *word, int level, int minlevel, int maxlevel )
{
	if ( IsReadOnly() ){
		error = DICERR_READONLY_WRITE;
		return -1;
	}
	if (!ODAReopen())
		return -1;
	DefSearchWriteLock();
	DefWriteLockRetry( return -1 );

#if MIXDIC
	_kwstrdef( __word, word );
#else
	#define	__word	word
#endif
	uint8_t *p = _get_attr_ptr( __word );
	if ( !p ) return 0;

	int l = (*p & WA_LEVELMASK);
	l += level;
	if ( l > maxlevel )
		l = maxlevel;
	if ( l < minlevel )
		l = minlevel;
	wa_t newattr = (wa_t)( (*p & ~WA_LEVELMASK) | l );
	if (newattr != *p){
		*p = newattr;
		if ( data->write(all.fw.pbn) == -1)
			return -1;
		NetLevel(__word,l);
		generation++;
		NotifyBackup();
		__TXDWriteReq__
	}
	return 1;
}
uint8_t *PdicBase::_get_attr_ptr( const _kchar *word )
{
	if ( BSearch(word) == -1 ) return NULL;

	if ( _kcscmp( word, all.fw.fword ) )
		return NULL;
	TDataBuf &databuf = *data->GetDataBuf();
	return ((uint8_t*)databuf.GetDataPtr( all.fw.lp ) + databuf.GetFieldHeaderSize() - 1);
}
wa_t PdicBase::get_attr( const tchar *word )
{
	uint8_t *p = _get_attr_ptr( __kwstr(word) );
	return p ? *(wa_t*)p : 0;
}

/////////////////////////////////////////////////////
// Mode settings
/////////////////////////////////////////////////////
#ifndef SMALL
// Force auto_write to TRUE when Network mode.
void PdicBase::SetMode(bool _auto_write, bool _fast_mode)
{
	if ( !auto_write && _auto_write )	// 1998.10.7 added
	{
		Flush();
	}
#if NETDIC
	if ( NetworkMode != NM_NONE ){
		_auto_write = true;
	}
#endif
	auto_write = _auto_write;
	data->SetFastMode(_fast_mode);
}
#endif

/////////////////////////////////////////////////////
// Status Information
/////////////////////////////////////////////////////

#if 0
uint PdicBase::GetFreeRate( )
{
	return (0xFFFFu - header->index_block) * 100L / 0xFFFFu;
}
#endif

int PdicBase::GetUsage(unsigned int filesize)
{
	if (filesize==(unsigned int)-1){
		if (!file.isOpen())
			return -1;
		filesize = file.get_length();
	}
	int index_usage = (int)(header->index_block+(0xFFFF/100-1)) * 100 / 0xFFFF;
	int disk_usage = (((uint)filesize)>>8)*100 / (0x80000000u>>8);
	return max(index_usage, disk_usage);
}


/////////////////////////////////////////////////////
// OLE Objects
/////////////////////////////////////////////////////

#if !defined(SMALL)

#ifdef USE_OLE
// 指定IDのオブジェクトを読み込む
// *jlinkにはnewしたオブジェクトが入るので、呼出側で必ず削除すること！！
// 0 : 見つからない
// 1 : 見つかった
// -1: エラー
int PdicBase::ReadObject( const tchar *word, t_id id, JLink **jlink )
{
	*jlink = NULL;

	Japa japa;
	int r = read( word, &japa );
	__assert_oda_open;
	int i;
	if ( r == -1 )
		return -1;
	if ( r == 1 ){
		// 同じIDオブジェクトを探す
		i = japa.jlinks.SearchID( id );
		if ( i != -1 ){
			// 同じIDオブジェクトを発見！
			*jlink = japa.jlinks[i].Clone( CAST_PDICBASE_PDIC(this) );
			return 1;	// 成功
		}
	}
	return 0;
}
#endif	// USE_OLE

#ifdef USE_JLINK
// 指定IDのオブジェクトを削除
// 0 : 見つからない
// 1 : 削除できた
// -1: エラー
int PdicBase::DeleteObject( const tchar *word, t_id id )
{
	return _DeleteObject(__kwstr(word), id);
}
int PdicBase::_DeleteObject( const _kchar *word, t_id id )
{
	if (!ODAReopen())
		return -1;
	DefSearchWriteLock();
	DefWriteLockRetry( return -1 );

	Japa japa;
	int r = _read( word, &japa );
	__assert_oda_open;
	int i;
	if ( r == -1 )
		return -1;
	if ( r == 1 ){
		// 同じIDオブジェクトを探す
		i = japa.jlinks.SearchID( id );
		if ( i != -1 ){
			// 同じIDオブジェクトを発見！
			japa.jlinks.del( i );
			if ( super::_update( word, 0, japa ) == -1 ){
				return -1;
			}
			return 1;	// 成功
		}
	}
	return 0;
}

int PdicBase::UpdateObjectTitle( const tchar *word, t_id id, const tchar *title )
{
	return _UpdateObjectTitle(__kwstr(word), id, title);
}
// 指定IDのオブジェクトのタイトルを変更
// 0 : 見つからない
// 1 : 削除できた
// -1: エラー
int PdicBase::_UpdateObjectTitle( const _kchar *word, t_id id, const tchar *title )
{
	if (!ODAReopen())
		return -1;
	DefSearchWriteLock();
	DefWriteLockRetry( return -1 );

	Japa japa;
	int r = _read( word, &japa );
	__assert_oda_open;
	int i;
	if ( r == -1 )
		return -1;
	if ( r == 1 ){
		// 同じIDオブジェクトを探す
		i = japa.jlinks.SearchID( id );
		if ( i != -1 ){
			// 同じIDオブジェクトを発見！
			japa.jlinks[i].SetTitle( title );
			if ( super::_update( word, 0, japa ) == -1 ){
				return -1;
			}
			return 1;	// 成功
		}
	}
	return 0;
}
#endif

#ifdef USE_OLE
void PdicBase::FreeObjects( )
{
	//****
//	if ( oledoc )
//		oledoc->FreeObjects( );
}
#endif	// USE_OLE

#ifdef USE_JLINK
bool PdicBase::SetLinkAttr( int flag, bool f )
{
	bool r = linkattr & flag ? true : false;
	if ( f ){
		linkattr |= flag;
	} else {
		linkattr &= ~flag;
	}
	return r;
}
#endif


#endif

#ifdef PDICW
// OR条件
// DICF_WRITEだけAND条件にした(1997.9.7)
// 本当はすべてAND条件にするか、ORとAND２つの関数を用意したいのだが・・・
int PdicBase::CheckCond( int flags )
{
	if ( flags == -1 ) return 1;
	if ( IsReadOnly() ){
		if ( flags & DICF_WRITE ) return 0;
	}
	if ( (flags & DICF_WRITE) && IsReadOnly() ){
		return 0;
	}
	if ( (flags & DICF_NEWDIC) ){
		return 1;
	}
	if ( (flags & DICF_READONLY) && IsReadOnly() ){
		return 1;
	}
	return (flags & DICF_WRITE) ? 1 : 0;
}
#endif

/////////////////////////////////////////////////////
// Utilities
/////////////////////////////////////////////////////

#ifndef SMALL
int PdicBase::EasyOptimize( t_pbn2 *_ipbn, t_pbn2 *_pbn )
{
	if ( IsReadOnly() ){
		error = DICERR_READONLY_WRITE;
		return -1;
	}

	__assert_oda_open;
	int r;
	r = super::EasyOptimize( _ipbn, _pbn );
	if ( r == 1 ){
		index->WriteIndex();
		data->WriteEmptyBlocks( );
		WriteHeader();
	}
	return r;
}
#endif

#ifdef USE_FILELINK
#include "assexec.h"
bool PdicBase::GetRelFileLinkPath(const tchar *filename, const tchar *commonpath, tnstr &relpath)
{
	tnstr dicpath = _GetFilePath( GetFullFileName() );
	tnstr dicflink = GetFileLinkPath();
	return ::GetRelFileLinkPath(relpath, filename, dicpath, dicflink, commonpath);
}

#endif

#if 0
void PdicBase::SetObjectNumber( int _objnumber )
{
#if NETDIC
	if ( NetworkMode ){
		DefWriteLockRetry( return -1 );
		iObjectNumber = _objnumber;
		WriteHeader();
	} else
#endif
	{
		iObjectNumber = _objnumber;
	}
}
#endif
#if defined(USE_JLINK)
int PdicBase::GetObjectNumber( )
{
#if NETDIC
	if ( NetworkMode ){
		DefWriteLockRetry( return iObjectNumber );
		iObjectNumber++;
		WriteHeader( );
		return iObjectNumber-1;
	} else
#endif
	{
		return iObjectNumber++;
	}
}
#endif	// USE_JLINK

#if INETDIC
bool PdicBase::WordLock( const _kchar *word )
{
	if (NetworkMode == NM_NONE)
		return true;
	return NDic->WordLock( word );
}
void PdicBase::WordUnlock()
{
	if (NetworkMode == NM_NONE)
		return;
	NDic->WordUnlock();
}
#endif	// INETDIC

bool PdicBase::HasFileChecksum(const tchar *filename)
{
	lsize_t size = filesize( filename );
	return (size > 0) && ((size % 256) == 16);
}

PdicBase::pdic_int_map_t PdicBase::IdleProcs;

bool PdicBase::IdleProc()
{
#if DICTXDUP
	if (TXDWriteReq){
		TXDUpdate();
	}
#endif
	return true;
}

bool PdicBase::IdleProcHandler()
{
	bool done = true;
	foreach(IdleProcs, it, pdic_int_map_t){
		if (it->second){
			if (!it->first->IdleProc())
				done = false;
		}
	}
	return done;
}

/////////////////////////////////////////////////////////////////////////////////
//	debug 専用 辞書チェック!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/////////////////////////////////////////////////////////////////////////////////

#if 0	// for debug
// dataはすでに読込み済みであること
bool PdicBase::CheckDataBlock( int lbn )
{
	unsigned int u, fu;
	FieldFormat *cp = NULL;
	char *tcp;			// temporary char ptr
	int nlen;
	char wbuf[LWORD+1];
	char fwbuf[LWORD+1];
	int lw;			/* 見出語長計算用	*/
	int lj;			/* 日本語長計算用	*/
	int ldata;			// 平均データ長計算用(計算中は tfieldとt_blknumを除いた値)
						// 平均格納率＝(ldata + (t_blknum)*index数 + (L_FieldHeader)*単語数) / 全ブロック数
	int l;

	int alen;
	const char *p;
	int n;
	uchar uc;
	uchar attr;
	int i;
	const char *sp;

	lw = 0;
	lj = 0;
	ldata = 0;

	int blksz = 0;
//	for (;;)
	{
//		if ( cp == NULL )
		{
			fwbuf[0] = '\0';

			blksz = getCurBlockSize();
			cp = data->GetTopDataPtr();
			fu = data->GetTopLoc();
			index->indexWord(wbuf, lbn);
#if !defined(NOFIELD2)
			if ( data->IsField2() ){
				nlen = FF2(cp)->fieldlen;
				tcp = FF2(cp)->word;
				// index->wordとのチェック
				if ( tcp[0] && strcmp( tcp, wbuf ) ) goto f2err1;
				// サイズチェック
				u = fu = data->GetTopLoc();
				if ( u + L_FieldHeader2 + nlen + sizeof(tfield2) > blksz ) goto f2err2;	// block削除
				lw += strlen(tcp);
				while (*tcp) tcp++;
				tcp++;
				const char *jp = tcp+1;
				if (*tcp & WA_EX ){
					tcp++;
					while ( *tcp ) tcp++;
					lj += FP_DIFF(tcp,jp);
				} else {
					lj += FP_DIFF(NextField2(FF2(cp)),jp);
				}
				ldata += nlen + 4;	// 4はL_FieldHeader2,tfield2の増加分(ごまかし(^^;)
				nlen = 0;	// for next
				goto skip4f2;
			}
#endif
			nlen = cp->fieldlen;
			tcp = cp->word;
			if (strcmp( tcp, wbuf) != 0){
	f2err1:
				DBW("インデックスとデータブロックが一致していません\n");
				DBW("pbn:[%d] lbn:[%d] Index:[%s] <> Block:[%s]",
					index->IndexBlockNo( lbn ), lbn, wbuf, (LPSTR)tcp );
				return false;
			}
		}
		u = FP_DIFF(cp, data->getDataBuf());
		if (u > blksz){
	f2err2:
			DBW("Warning:ブロックの終端が見つかりません。lbn:[%d] off:[%d]",lbn, fu );
			return false;
		}
		fu = u;
		if ( nlen > blksz - ( L_FieldHeader + 1 ) ){
			DBW( "Fatal:見出語＋日本語訳が長すぎます[%d > %d]", nlen, blksz-L_FieldHeader-1 );
			return false;
		}
		alen = cp->complen;
		if ( alen > LWORD-2){
			DBW( "Fatal:圧縮文字数[a=%d]が大きすぎます。", (uint)alen );
			return false;
		}
		p = cp->word;
		//strcpyのためのチェック
		l = alen + strlen(p);
		if (l > LWORD){
			DBW( "Fatal:見出語が長すぎます[%d > %d]", l, LWORD );
			return false;
		}
		strcpy(wbuf + alen, p);
//		eprintf("%s"EOL"\r", wbuf);
		cp = (FieldFormat*)(p + nlen);
		strcpy(fwbuf, wbuf);
		//単語の中身を検査
		n = 0;
		while ( ( uc = *p ) != '\0' ){
			n++;
			p++;
		}
		if (n > nlen){
			DBW( "Fatal:単語が登録された長さより長すぎます[%d > %d]", n, nlen);
			return false;
		}
		lw += n;
		p++;
		n = nlen - n;
		attr = *p;
		p += GetAttrLen();
		n -= GetAttrLen();
		n--;

///////////日本語の検査/////////////////////////////////////////////////////////////
/*		この時点での有効な変数
		p		: 日本語訳の先頭（属性は除く）
		n		: 残りのフィールド数（WA_EX文も含む）
		lw		: 圧縮時の見出語の長さ
		lj		: 日本語訳の長さ
		ldata	: 全データ長
																				*/
////////////////////////////////////////////////////////////////////////////////////
		sp = p;
		for (i=0;i<n;i++){
			uc = (uchar)*p;
			if ( uc == 0 )
				break;
			p++;
		}
		lj += FP_DIFF( p, sp );	// 日本語訳長
		if ( attr & WA_EX ){
			p++;
			i++;
			for ( ;i<n;i++ ){
				uchar uc = (uchar)*p;
				if ( uc & JT_BYTE || uc & JT_COMP ){
						p++;
						i++;
						t_jtb c = *(t_jtb*)p;
						p += c + sizeof(t_jtb);
						i += c + sizeof(t_jtb)-1;	// -1はforループのi++分
						continue;
				}
				if ( (uchar)*p == (uchar)JT_END ){
					p++;
					i++;
					break;
				}
				const char *pp = p;
				while ( *p ) p++;	// テキストの場合は読み飛ばす
				i += FP_DIFF( p, pp );
				p++;
			}
		}
		if ( i != n ){
			DBW( "Warning:日本語訳終端が異常です.lbn:[%d]", lbn );
			return false;
		}
		ldata += nlen;
		nlen = cp->fieldlen;
skip4f2:	// for NEWDIC3
		if ( !nlen ){
			cp = NULL;		/* ブロックの終端 */
		}
jmp:;
	}
	return true;
}

#endif	// for debug

#define	__funcin	MapMutex.Lock()
#define	__funcout	MapMutex.Unlock()
map<tnstr,Pdic*> Pdic::PdicMap;
static TMutex MapMutex;

Pdic::Pdic()
{
	RefCnt = 1;
	OpenCnt = 0;
	ThreadCnt = 0;
}
Pdic::~Pdic()
{
	if (IsOpened()){
		RemoveMap();
	}
}

//Note: thread unsafe
void Pdic::AddMap(const tchar *filename)
{
	__funcin;
	tnstr namekey = NormalizeFileName(filename);
	//DBW("add:%ws:%d", namekey.c_str(), PdicMap.count(namekey));
	if (PdicMap.count(namekey)){
		Pdic *pdic = PdicMap[namekey];
		if (pdic!=this){
			// 同じファイル名で、異なるinstance
			// CreateInstance()時のファイル名とOpen()のファイル名が異なっている場合
			// 両方ともreadonlyでopenしているため問題は無いと思うが。。
			DBW("WARN:diff obj: namekey=%ws %08X", namekey.c_str(), pdic);
			//__assert__;
		}
	} else {
		PdicMap[namekey] = this;
	}
	OpenCnt++;
	__funcout;
}

//Note: thread unsafe
void Pdic::RemoveMap()
{
	__funcin;
	__assert(IsOpened());
	tnstr namekey = NormalizeFileName(file.GetFileName());
	//DBW("remove:%ws:%d", namekey.c_str(), PdicMap.count(namekey));
	//__assert(PdicMap.count(namekey));
	if (PdicMap.count(namekey)==0){
		DBW("WARN:PdicMap.count(%ws) not found", namekey.c_str());
	} else {
		if (PdicMap[namekey] == this){
			PdicMap.erase(namekey);
		}
	}
	__funcout;
}


tnstr Pdic::NormalizeFileName(const tchar *name)
{
	// normalize filename.
	tnstr nfilename;
	if ( !makename( nfilename, name, NULL ) ){
		nfilename = name;
	}
#ifdef _Windows
	CharLower(nfilename.c_str());
#endif
	return nfilename;
}

//Note: thread unsafe
Pdic *Pdic::GetInstance(const tchar *filename)
{
	if (!filename){
		// 必ず作成できるがOpen()時に失敗する可能性がある
		return NULL;
	}
	__funcin;
	tnstr name = NormalizeFileName(filename);
	if (PdicMap.find(name)!=PdicMap.end()){
		Pdic *pdic = PdicMap[name];
		pdic->AddRef();
		__funcout;
		return pdic;
	}
	__funcout;
	return NULL;
}

Pdic *Pdic::CreateInstance(const tchar *filename)
{
	Pdic *pdic = GetInstance(filename);
	if (pdic)
		return pdic;
	return new Pdic();
}
void Pdic::AddRef()
{
	RefCnt++;
}
Pdic *Pdic::Clone()
{
	AddRef();
	return this;
}
void Pdic::Release()
{
	__assert(RefCnt>0);
	RefCnt--;
	if (RefCnt<=0){
		delete this;
	}
}

int Pdic::Open( const tchar * fname, bool _readonly )
{
	if (OpenCnt){
		// Already opened.
		if (readonly && !_readonly){
			// readonlyを!readonlyでopenしようした場合→error
			error = DICERR_ALREADY_OPEN;
			return -1;
		}
		//TODO: EPWINGとnon-EPWING check
		OpenCnt++;
		return 0;	// OK
	}
	int ret = super::Open(fname, _readonly);
	if (ret)
		return ret;	// error
	AddMap(fname);
	return 0;
}
//TODO: close状態で何度もcloseされるとout!!
void Pdic::Close( )
{
	if (OpenCnt>0){
		OpenCnt--;
	}
	if (OpenCnt>0)
		return;
	if ( !IsOpened() )
		return;

	if (!ODAProgress){
		//Note: 2016.3.8
		// ThreadCnt>0のときに辞書が更新された場合、必ずclose/openが必要。
		// そのため、ODAProgress時はThreadCntを見ないで進める
		for (int i=0;i<20;i++){
			if (ThreadCnt<=0)
				break;
			Sleep(100);
		}
		if (ThreadCnt>0){
			DBW("ThreadCnt timeout!!");
#if __DBG
			for (int i=0;i<ThreadIds.size();i++){
				DBW("Id# %08X", ThreadIds[i]);
			}
#endif
		}
	}

	//Note:	ここで例外(map内)が発生する場合、辞書をcloseせずにアプリを終了する場合に発生する
	RemoveMap();
	super::Close();
}

// Should check the returned value.
bool Pdic::ThreadUp()
{
	ThreadCntMutex.Lock();
	ThreadCnt++;
	ThreadCntMutex.Unlock();
	if (OpenCnt==0){
		ThreadDown();
		return false;	// failed to lock
	}
	return true;
}
void Pdic::ThreadDown()
{
	__assert(ThreadCnt>0);
	if (ThreadCnt<=0){
		DBW("ThreadDown - ThreadCnt=%d!!", ThreadCnt);
		return;
	}
	ThreadCntMutex.Lock();
	ThreadCnt--;
	ThreadCntMutex.Unlock();
}

TempBuffData::TempBuffData( const uint8_t *buf, int _size, t_pbn2 _lbn )
	:size( _size )
	,lbn( _lbn )
{
	buff = new uint8_t[ _size ];
	if ( buff ){
		_fmemcpy( buff, buf, (uint)_size );
	}
}

TempBuffData::~TempBuffData()
{
	if ( buff )
		delete[] buff;
}

int TempBuff::save( const uint8_t *buf, t_pbn2 _lbn, int blksz )
{
	if ( get_num() >= MAX_TEMPBUFF )
		return 0;
	TempBuffData *tbd = new TempBuffData( buf, blksz, _lbn );
	if ( !tbd )
		return 0;
	if ( !tbd->isValid() ){
		delete tbd;
		return 0;
	}
	if ( add( tbd ) == -1 ){
		delete tbd;
		return 0;
	}
	return 1;
}

uint8_t *TempBuff::load( t_pbn2 _lbn, t_pbn2 &loadlbn )
{
	for ( int i=0;i<get_num();i++ ){
		TempBuffData &tbd = (*this)[i];
		if ( tbd.lbn == _lbn ){
			loadlbn = tbd.lbn;
			uint8_t *buf = new uint8_t[ tbd.size ];	// メモリ不足の場合はどうしよう？？
			if ( buf ){
				_fmemcpy( buf, tbd.buff, (uint)tbd.size );
				del( i );
			}
			return buf;
		}
	}
	return NULL;
}

uint8_t *TempBuff::allLoad( t_pbn2 &lbn )
{
	if ( get_num() ){
		lbn = (*this)[0].lbn;
		return load( lbn, lbn );
	}
	return NULL;
}

t_blknum TempBuff::getblknum( t_pbn2 _lbn )
{
	for ( int i=0;i<get_num();i++ ){
		TempBuffData &tbd = (*this)[i];
		if ( tbd.lbn == _lbn ){
			return *(t_blknum*)tbd.buff & ~FIELDTYPE;
		}
	}
	return 0;
}

#ifdef EPWING
#include "pdepwing.h"
#endif

// Pdicをopenするための共通処理 //
Pdic *CreatePdic( const tchar * pathname, const tchar *epwname, const tchar *gconvname, const tchar *flinkpath, int openflag, const tchar *nettempname, int *error, int dictype )
{
	Pdic *dic;

#if DICTXDUP
	tnstrbuf pdicname;
#endif

#ifdef EPWING
	if ( epwname && epwname[0] ){
		dic = PDEPWing::CreateInstance(pathname);
		((PDEPWing*)dic)->SetEPWingFileName( epwname );
		((PDEPWing*)dic)->SetGConvFileName( gconvname );
	} else
#endif
	{
		dic = Pdic::CreateInstance(pathname);
	}
	if ( !dic || !dic->GetHeader() ){
		if ( dic ){
			*error = DICERR_NOMEMORY;	// memory error
			dic->Release();
		}
		return NULL;
	}
	int r;

#if DICTXDUP
jretry:;
#endif

#ifdef USE_FILELINK
	if (flinkpath){
		dic->SetFileLinkPath( flinkpath );
	}
#endif

#if INETDIC
	dic->SetNetworkMode(
#if INETDIC
		(openflag&DICFLAG_INTERNET) ? NM_INET : 
#endif
		NM_NONE
		);
#endif

#if NETDIC
	if ( nettempname )
		dic->SetTemporaryName( nettempname );
#endif

#ifdef USE_EXTDB
	if ((openflag & DICFLAG_FASTDB_CREATE)
#ifdef _DEBUG
		|| !_tcsncmp(GetFileName(pathname), _t("extdb"), 5)
#endif
	){
		dic->SetExtDBMode(DICEXT_FASTDB_CREATE, true);
	}
#endif

	// 2008.3.20 Open() -> OpenRetry()
	r = dic->OpenRetry(pathname, openflag & DICFLAG_READONLY ? true : false);
	if ( r ){
		*error = dic->GetErrorCode();
#if DICTXDUP
		if (openflag & DICFLAG_TXDUP){
			int err = dic->GetErrorCode();
			if (err==DICERR_OPEN_CREATE && dictype){
				goto jtxdup1;
			}
			if (err==DICERR_NOTPDIC){
				if (dictype!=0 || TDicConverter::SupportedType(pathname)){
				jtxdup1:
					pdicname = CreateTxdFileName(pathname);
					Pdic *txdic = Pdic::CreateInstance(pdicname);
					if (!txdic){
						*error = DICERR_NOMEMORY;	// memory error
						return NULL;
					}
					bool err_if_fail = false;
					if (txdic->Open(pdicname, false)){
						// open error
						err = txdic->GetErrorCode();
						switch (err){
							case DICERR_OPEN_CREATE:
								break;
							default:
								goto jexist;
						}
						if (txdic->CreateHyper(pdicname)){
							// error
							*error = err;
						} else {
							err_if_fail = true;
						}
					}
				jexist:;
					// Pdic exists
					if (dictype==0){
						dictype = GetDicFileType(pathname);
					}
					txdic->SetTextDic(pathname, dictype);
					if (!txdic->TXDCheckUpdate()){
						if (err_if_fail){
							txdic->Close();
							txdic->Release();
							goto jerr;
						}
					}
					txdic->Close();
					if (txdic!=dic){
						dic->Release();
						dic = txdic;
					}
					pathname = pdicname;
					goto jretry;
				}
			}
		}
#endif
jerr:;
		dic->Release();
		return NULL;
	}
	return dic;
}

tnstrbuf CreateTxdFileName(const tchar *dicname)
{
	tnstr ndicname = Pdic::NormalizeFileName(dicname);
	tnstr path = _GetFilePath(ndicname);

	tnstrbuf txd;
	txd << GetTempPath();
	if (path.exist()){
		MD5_CTX md5;
		MD5Init( &md5 );
		MD5Update( &md5, (unsigned char*)path.c_str(), path.byte());
		MD5Final( &md5 );

		tchar hex[8+1];
		bintohex(hex, md5.digest, 4);
		txd << hex << _t("_") << GetFileName(dicname) << _t(".dic");
	} else {
		DBW("Do not use this pattern!!");
		txd << GetFileName(dicname) << _t(".dic");
	}
	return txd;
}

