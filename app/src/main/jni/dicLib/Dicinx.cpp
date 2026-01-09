#include	"tnlib.h"
#pragma	hdrstop
#include "dic.h"
#include "dicdbg.h"

#define	MAX_INDEXNUM	0x10000	// depends on the header structure

#if USE_MMI
#define	__enterEdit__	{ if (mmf && enterEdit()<0) return -1; }
#else
#define	__enterEdit__
#endif

PdicIndex::PdicIndex( int &error, FileBuf &_file)
	:file(_file)
	,PdicObject( error )
{
	index = 0;
	nbyte = 0;
	indexnum = 0;
	indexsize = 0;
	pindexnum = 0;
	Modified = false;
#if USE_MMI
	mmf = NULL;
	index16 = NULL;
#endif
#if FINX
	indexAllocator = NULL;
#endif
}

PdicIndex::~PdicIndex()
{
#if FINX
	delete indexAllocator;
#endif
}

int PdicIndex::Open( const HEADER FAR *_header )
{
	header = _header;
#ifndef _Windows
	if ( pmem.init(4) == -1 ){
		error = 2;
		return -1;
	}
#endif
	pmem.lock();
	indexsize = header->index_block * header->block_size;
	offset = header->header_size + header->extheader;
	pindexnum = header->nindex2;
	if ( pindexnum > MAX_INDEX3 )
		pindexnum = PINDEX_SLOTNUM;
	if ( pindexnum == 0 )
		pindexnum = PINDEX_SLOTNUM;
//	index = (PINDEX FAR *)pmem.alloc((uint)((long)sizeof(PINDEX) * PINDEX_SLOTNUM));
	index = (PINDEX FAR *)pmem.alloc( (uint)((long)sizeof(PINDEX) * pindexnum) );
	if ( !index ){
		goto jmemerr;
	}

#if USE_MMI
	mmf = new TMmfMap(file.GetFileHandle());
	if (!mmf)
		goto jmemerr;
	if (mmf->map(offset, indexsize, true)){
		// succeeded.
		if (getIndexMM()>=0){
			goto jok;
		}
	}
	delete mmf;
	mmf = NULL;
#endif
	
//	pindexnum = PINDEX_SLOTNUM;
	nbyte = 0;
	indexnum = 0;
	if ( getIndex( ) == -1){
		pmem.free( index );
		goto jmemerr;
	}
jok:
	pmem.unlock();
	Modified = false;
	return 0;
jmemerr:
	error = DICERR_INDEXMEMFULL;
jerr:
#if USE_MMI
	if (mmf){
		delete mmf;
		mmf = NULL;
	}
	index16 = NULL;
#endif
	if (index){
		pmem.free(index);
		index = NULL;
	}
	pmem.finish();
	pindexnum = 0;
	return -1;
}

void PdicIndex::Close( )
{
	pmem.lock();
	if (file.isOpen()){
		Flush();
	}
#if	!defined(DOS)
	// Windowsの場合は、finish()のみでOK
	// Now it is not so.
	// Windows version uses same allocation as DOS.
	if ( index ){
		for (int i=0;i<indexnum;i++){
//			DBW("i=%d:%08X\n",i,index[i]);
			dealloc(i);
		}
		pmem.free( index );
	}
#else
	if ( !pmem.useems && index ){
		// EMSを使っていない場合は、farfreeする必要があるため
		for (int i=0;i<indexnum;i++){
			dealloc(i);
		}
		pmem.free( index );
	}
#endif
#if USE_MMI
	if (mmf)
		delete mmf;
	index16 = NULL;
#endif
	indexnum=0;
	index = NULL;
	pindexnum = 0;
	pmem.finish();
}

bool PdicIndex::Flush()
{
	// assert(file.isOpen())
	if (!IsModified()) return false;

	WriteIndex();
	return true;
}

#if FINX
void PdicIndex::dealloc( int lbn )
{
	PINDEX p = index[lbn];
	if (indexAllocator && indexAllocator->is(p)){
	} else {
		pmem.free(p);
	}
}
#endif

//返り値：-1 容量不足エラー
//		  上記以外 読み込んだindex数
int PdicIndex::getIndex( )
{
	nbyte = 0;
	indexnum = 0;
	if (seek_index() == -1)
		return -1;

#if FINX
	unsigned _indexsize = indexsize;
#endif

	pmem.lock();
#ifndef SWAP_ENDIAN
	// 高速版
	char *rbuf = new char[ indexsize ];
	const char *rbuf_end = rbuf + indexsize;
	if ( file.read( rbuf, indexsize ) > sizeof(t_pbn) ){
#if FINX
		delete[] indexAllocator;
		const unsigned delta = header->nindex2 * (sizeof(t_pbn2) - sizeof(t_pbn));
		indexAllocator = new SimpleAllocator(_indexsize + delta);
		if (!indexAllocator->ok()){
			error = DICERR_INDEXMEMFULL;
			return -1;
		}
#endif
		const char *sp = rbuf;
		while ( 1 ){
			int l = sizeof(t_pbn) + _kcsbyte1((_kchar*)(sp+sizeof(t_pbn)));
			if ( *(_kchar*)&sp[sizeof(t_pbn)] == 0 ){
				break;
			}
			if ( indexnum >= pindexnum ){
				break;
			}
#if FINX
			PINDEX ip = (PINDEX)indexAllocator->alloc(l+2);
#else
			PINDEX ip = alloc(l+2);
#endif
			if ( !ip ){
				pmem.unlock();
				error = DICERR_INDEXMEMFULL;
				delete[] rbuf;
				return -1;		//メモリ不足
			}
			index[indexnum++] = ip;
#if 0
			t_pbn tp;
			memcpy( &tp, sp, sizeof(t_pbn) );
			*(t_pbn2*)ip = tp;
#else
			*(t_pbn2*)ip = (t_pbn2)_alGetUShort((t_pbn*)sp);
#endif
			_fmemcpy(((char*)ip)+sizeof(t_pbn2), sp+sizeof(t_pbn), l-sizeof(t_pbn));
			nbyte += l;		// disk上のbytes
			sp += l;
			if ( sp >= rbuf_end )
				break;
		}
	}
	delete[] rbuf;
#else
	char *buf = new char[ sizeof(INDEX) + _KLENTOBYTE(LWORD) ];
	while ( 1 ){
		char *dp = buf;
		int c = file.getch();
		if ( c == -1 )
			break;
#ifdef SWAP_ENDIAN
		dp[1] = (char)c;	//
		dp[0] = (char)file.getch();
		dp+=2;
#else
		*dp++ = (char)c;	//
		*dp++ = (char)file.getch();
#endif
		*(*(short**)&dp)++ = 0;

		int ct = 2;

		do {
			if ( (c = file.getch()) == -1 ){
				break;
			}
#if defined(_UNICODE) && !MIXDIC && !defined(KMIXDIC)
#ifdef SWAP_ENDIAN
			c = MAKEWORD( file.getch(), c );
#else
			c = MAKEWORD( c, file.getch() );
#endif
#endif
			*(*(_kchar**)&dp)++ = (_kchar)c;
			ct++;
		} while ( c );
		if (ct == 3)
			break;
		if (nbyte + _KLENTOBYTE(ct) >= indexsize){
			break;
		}

		if ( indexnum >= pindexnum ){
			PINDEX FAR *ptr = (PINDEX FAR *)pmem.realloc( index, sizeof(PINDEX) * (pindexnum + PINDEX_SLOTNUM) );
			if ( !ptr ){
				pmem.unlock();
				delete[] buf;
				error = 2;
				return -1;
			}
			pindexnum += PINDEX_SLOTNUM;
			index = ptr;
		}

		int l = sizeof(INDEX) + _kcsbyte((_kchar*)(buf+sizeof(t_pbn2)));
		PINDEX ip = alloc(l);
		if ( !ip ){
			pmem.unlock();
			delete[] buf;
			error = DICERR_INDEXMEMFULL;
			return -1;		//メモリ不足
		}
		index[indexnum] = ip;
		_fmemcpy(ip, buf, l);
		nbyte += l - 2		// t_pbn2とt_pbnの差分
		;
		indexnum++;
	}
	delete[] buf;
#endif
	nbyte += (sizeof(t_pbn2) - sizeof(t_pbn)) * indexnum;	// memory上の占有量に変換
	pmem.unlock();
	return indexnum;
}

#if USE_MMI
//返り値：-1 容量不足エラー
//		  上記以外 読み込んだindex数
int PdicIndex::getIndexMM( )
{
	nbyte = 0;
	indexnum = 0;

	__assert(mmf!=NULL);

	index16 = (const INDEX16**)pmem.alloc( (uint)((long)sizeof(INDEX16*) * pindexnum) );
	if (!index16){
		error = DICERR_MEMORY;
		return -1;
	}
		
	pmem.lock();

	const char *top = (const char*)mmf->data();

	const char *sp = top;
	while ( 1 ){
		int l = sizeof(t_pbn) + _kcsbyte1((_kchar*)(sp+sizeof(t_pbn)));
		if ( *(_kchar*)&sp[sizeof(t_pbn)] == 0 ){
			break;
		}
		if ( indexnum >= pindexnum ){
			break;
		}
		index16[indexnum++] = (INDEX16*)sp;
		nbyte += l;		// disk上のbytes
		sp += l;
		if ( sp >= top + indexsize )
			break;
	}
	nbyte += (sizeof(t_pbn2) - sizeof(t_pbn)) * indexnum;	// memory上の占有量に変換
	pmem.unlock();
	//index16 = (const INDEX16**)index;
	return indexnum;
}
int PdicIndex::enterEdit()
{
	if (!index16)
		return 0;	// already entered.

#ifdef SWAP_ENDIAN
	#error	// not implemented.
#endif

	pmem.lock();
	for (int i=0;i<indexnum;i++){
		int l = sizeof(t_pbn) + _kcsbyte1((_kchar*)((char*)index16[i]+sizeof(t_pbn)));
		PINDEX ip = alloc(l+2+1);
		if ( !ip ){
			pmem.unlock();
			error = DICERR_INDEXMEMFULL;
			return -1;		//メモリ不足
		}
		index[i] = ip;
		index[i]->block = index16[i]->block;
		_fmemcpy(index[i]->word, index16[i]->word, l-sizeof(t_pbn));
	}
	pmem.unlock();
	delete mmf;
	mmf = NULL;
	index16 = NULL;
	return indexnum;
}
#endif	// USE_MMI

//	wordをindexから探索し、該当箇所の論理ブロック番号を与える
//	一致しなかった場合は、小さめの論理ブロック番号を与える
int PdicIndex::indexSearch( const _kchar *word )
{
	if ( indexnum == 0 )		// Bug fix. 1994/08/13
		return 0;

	pmem.lock();

	int left = 0;
	int right = indexnum;
	int mid;
	do {
		mid = (left + right) /2;
		int k = _fcmpword(word, indexWord(mid));	// Windows版でlstrcmp()に置き換えてはいけない
												// lstrcmp()はstrcmp()と完全互換ではない
		if (k == 0){
			pmem.unlock();
			return mid;
		}
		if (k < 0){
			right = mid;
		} else {
			left = mid + 1;
		}
	} while (left < right);
	pmem.unlock();
	if (left == 0)
		return 0;
	return left-1;
}

#if !defined(SMALL)
//#ifdef USE_TREE
// 本当の挿入点を求める
int PdicIndex::indexSearch2( const _kchar *word )
{
	if ( indexnum == 0 )		// Bug fix. 1994/08/13
		return 0;

	pmem.lock();

	int left = 0;
	int right = indexnum;
	int mid;
	do {
		mid = (left + right) /2;
		int k = _fcmpword(word, indexWord(mid));	// Windows版でlstrcmp()に置き換えてはいけない
												// lstrcmp()はstrcmp()と完全互換ではない
		if (k == 0){
			pmem.unlock();
			return mid;
		}
		if (k < 0){
			right = mid;
		} else {
			left = mid + 1;
		}
	} while (left < right);
	pmem.unlock();
	return left;
}
//#endif
#endif

#ifndef _Windows
void PdicIndex::indexWord(char *buf, int lbn)
{
	pmem.lock();
	lfstrcpy(buf, index[lbn]->word);
	pmem.unlock();
}

t_pbn2 PdicIndex::indexBlock(int lbn)
{
	pmem.lock();
	t_pbn2 r = index[lbn]->block;
	pmem.unlock();
	return r;
}

void PdicIndex::setIndexBlock(int lbn, t_pbn2 pbn)
{
	pmem.lock();
	index[lbn]->block = pbn;
	pmem.unlock();
	Modified = true;
}
#endif

#ifndef	PDOLDCVT

//	論理ブロック番号lbnを空きブロックに登録する
// override
int PdicIndex::delIndex(int lbn)
{
	__enterEdit__;

	pmem.lock();

	nbyte -= sizeof(INDEX) + _kcsbyte(index[lbn]->word);
	dealloc(lbn);

	indexnum--;
	memmove( &index[lbn], &index[lbn+1], (indexnum-lbn)*sizeof(PINDEX) );
#if 0
	for (i=lbn;i<indexnum+1;i++){
		index[i] = index[i+1];
	}
#endif

	if ( indexnum < pindexnum - PINDEX_SLOTNUM && pindexnum > PINDEX_SLOTNUM ){
		// shrink
		index = (PINDEX FAR *)pmem.realloc( index, sizeof(PINDEX) * (pindexnum - PINDEX_SLOTNUM) );
		pindexnum -= PINDEX_SLOTNUM;
	}

	pmem.unlock();

	Modified = true;
	return 0;
}

//	単語だけを変更する
// 順番が変わるような変更の場合は、renIndexSortを利用する
int PdicIndex::renIndex(int lbn, const _kchar *word)
{
	__enterEdit__;

	pmem.lock();

	int ll = sizeof(INDEX) + _kcsbyte(index[lbn]->word);	//解放するindexの大きさ
	int l = sizeof(INDEX) + _kcsbyte(word);	//新しいindexの大きさ
	if ( nbyte + l - ll >= indexsize ){	//辞書の容量チェック
		pmem.unlock();
		error = 3;
		return -1;
	}
	PINDEX ip = alloc(l);
	if ( !ip ){
		pmem.unlock();
		error = 2;
		return -1;
	}

	t_pbn2 pbn = index[lbn]->block;
	nbyte -= ll;
	dealloc(lbn);

	nbyte += l;

	index[lbn] = ip;
	_kcscpy(ip->word, word);
	ip->block = pbn;

	pmem.unlock();
	Modified = true;
	return 0;
}
#ifdef USE_TREE
int PdicIndex::renIndexSort(int lbn, const _kchar *word)
{
	__enterEdit__;

	int r = renIndex( lbn, word );
	if ( r != 0 ) return r;

	pmem.lock();

	int i;
	// 前隣と比較
	i = lbn-1;
	while ( i > 0 ){
		if ( cmpword( index[i]->word, index[lbn]->word ) <= 0 )
			break;
		i--;
	}
	if ( i != lbn-1 ){
		PINDEX p = index[lbn];
		memmove( &index[i+2], &index[i+1], sizeof(PINDEX)*(lbn-i-1) );
		index[i+1] = p;
	} else {
		// 後ろ隣と比較
		i = lbn+1;
		while ( i < indexnum ){
			if ( cmpword( index[lbn]->word, index[i]->word ) <= 0 )
				break;
			i++;
		}
		if ( i != lbn+1 ){
			PINDEX p = index[lbn];
			memmove( &index[lbn], &index[lbn+1], sizeof(PINDEX)*(i-lbn-1) );
			index[i-1] = p;
		}
	}

	pmem.unlock();
	Modified = true;
	return 0;
}
#endif

//	インデックスの要素lbnに物理ブロック番号pbn、見出語wordを追加する。
//	返り値：登録された論理ブロック番号（-1のときは、メモリ不足エラー）
int PdicIndex::addIndex(int lbn, t_pbn2 pbn, const _kchar *word)
{
	__enterEdit__;

	pmem.lock();

	int l = sizeof(INDEX) + _kcsbyte(word);
	if (nbyte + l >= indexsize){		//辞書の容量チェック
		pmem.unlock();
		error = 3;
		return -1;
	}
	if ( indexnum >= pindexnum ){
		PINDEX FAR *ptr = (PINDEX FAR *)pmem.realloc( index, sizeof(PINDEX) * (pindexnum + PINDEX_SLOTNUM) );
		if ( !ptr ){
			pmem.unlock();
			error = 2;
			return -1;
		}
		pindexnum += PINDEX_SLOTNUM;
		index = ptr;
	}
	PINDEX ip = alloc(l);
	if ( !ip ){	//メモリ容量エラー
		pmem.unlock();
		error = 2;
		return -1;
	}
	nbyte += l;
	memmove( &index[lbn+1], &index[lbn], (indexnum-lbn)*sizeof(PINDEX) );
	index[lbn] = ip;
	_kcscpy( ip->word, word );
	ip->block = pbn;
	indexnum++;
	pmem.unlock();
	Modified = true;
	return 0;
}

#endif	// PDOLDCVT

// override
int PdicIndex::WriteIndex( )
{
#if USE_MMI
	if (index16)
		return 0;	// not modified
#endif
//	if ( !Modified ) return 0;
	// ここではねておきたいが、
	// チェックをするのが大変そうなので、、後ほど

	int bn;

	if (seek_index() == -1) return -1;

	pmem.lock();
	for (bn=0;bn<indexnum;bn++){
#ifdef SWAP_ENDIAN
		short sh = DE_SWAP(index[bn]->block);
		file.write( &sh, sizeof(t_pbn) );	// 16bit
#else
		file.write( &index[bn]->block, sizeof(t_pbn) );	// 16bit
#endif
#ifdef SWAP_ENDIAN
		for ( int i=0;i<_kcslen(index[bn]->word)+sizeof(_kchar);i++ ){
			_kchar c = DE_SWAP(index[bn]->word[i]);
			file.write( &c, sizeof(_kchar) );
		}
#else
		file.write( index[bn]->word, _kcsbyte1(index[bn]->word) );
#endif
	}
	if ( indexsize > 0 && nbyte < indexsize){
		file.write("\0\0\0\0", min(indexsize - nbyte, 4u));
	}
	pmem.unlock();
	Modified = false;
	return 0;
}

//新規登録可能な見出語語数を求める
//NEWDIC3の場合は正確ではない
unsigned PdicIndex::getRemain( )
{
	if ( indexsize < nbyte - sizeof(INDEX) )
		return 0;
	return indexsize - nbyte - sizeof(INDEX);
}

// 物理ブロック番号から論理ブロック番号を求める
int PdicIndex::searchLbn( t_pbn2 pbn )
{
	pmem.lock();
	for ( int i=0;i<indexnum;i++ ){
		if ( indexBlock(i) == pbn ){
			pmem.unlock( );
			return i;
		}
	}
	pmem.unlock();
	return -1;
}

// only NEWDIC3
void PdicIndex::GrowIndex( int num )
{
#if USE_MMI
	__assert(!index16);
#endif

	Modified = true;

	indexsize += header->block_size*num;

	DD( DD_SETINDEXBLOCK, num );

	// 物理ﾌﾞﾛｯｸ番号の変更
	for ( int i=0;i<indexnum;i++ ){
		index[i]->block -= num;
	}
}
bool PdicIndex::CanGrowIndex(int num) const
{
	return indexsize/header->block_size + num < MAX_INDEXNUM;
}
// numブロック分だけ後へずらす
// indexsizeは変わらない
void PdicIndex::MoveIndex( int num )
{
	Modified = true;
	offset += header->block_size*num;
	// 2013.3.1 ↓の処理がなかったため、辞書が壊れていた
	// データが存在するときに、ExtHeaderを拡張すると壊れる
	// 物理ﾌﾞﾛｯｸ番号の変更
	for ( int i=0;i<indexnum;i++ ){
		index[i]->block -= num;
	}
}
// numブロック分だけ前へずらし、ずらした分だけindexsizeを増やす
void PdicIndex::MoveGrowIndex( int num )
{
	Modified = true;
	offset -= header->block_size*num;
	indexsize += header->block_size*num;
}
#if 0
unsigned PdicIndex::GetIndexBytesOnDisk()
{
	if ( Has32() ){
		return nbyte;
	} else {
		return nbyte - (sizeof(t_pbn2) - sizeof(t_pbn)) * indexnum;
	}
}
#endif

