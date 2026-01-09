#include	"tnlib.h"
#pragma	hdrstop
#include "dic.h"

HPdicIndex::HPdicIndex( int &error, FileBuf &_file )
	:PdicIndex( error, _file )
{
}

HPdicIndex::~HPdicIndex()
{
}

//返り値：-1 容量不足エラー
//		  上記以外 読み込んだindex数
int HPdicIndex::getIndex( )
{
	if ( !header->index_blkbit )
		return PdicIndex::getIndex();

	nbyte = 0;
	indexnum = 0;
	if (seek_index() == -1)
		return -1;


	unsigned _indexsize = indexsize;

	pmem.lock();
#ifndef SWAP_ENDIAN	// 高速版
	char *rbuf = new char[ indexsize ];
    const char *rbuf_end = rbuf + indexsize;
	if ( file.read( rbuf, indexsize ) > sizeof(t_pbn2) ){
#if FINX
        delete[] indexAllocator;
        indexAllocator = new SimpleAllocator(_indexsize);
        if (!indexAllocator->ok()){
            error = DICERR_INDEXMEMFULL;
            return -1;
        }
#endif
		const char *sp = rbuf;
		while ( 1 ){
			int l = sizeof(t_pbn2) + _kcsbyte1((_kchar*)(sp+sizeof(t_pbn2)));
			if ( *(_kchar*)&sp[sizeof(t_pbn2)] == 0 ){
				break;
			}
			if ( indexnum >= pindexnum ){
				break;
			}
#if FINX
			PINDEX ip = (PINDEX)indexAllocator->alloc(l);
#else
			PINDEX ip = alloc(l);
#endif
			if ( !ip ){
				pmem.unlock();
				error = 2;
				delete[] rbuf;
				return -1;		//メモリ不足
			}
			index[indexnum++] = ip;
			_fmemcpy(ip, sp, l);
			nbyte += l;
			sp += l;
			if ( sp >= rbuf_end )
				break;
		}
	}
	delete[] rbuf;
#else	// 低速版
	char *buf = new char[ _KLENTOBYTE(LWORD) + sizeof(INDEX) ];
	while ( 1 ){
		char *dp = buf;
#ifdef SWAP_ENDIAN
		dp[3] = (char)file.getch();
		dp[2] = (char)file.getch();
		dp[1] = (char)file.getch();
		dp[0] = (char)file.getch();
		dp+=4;
#else
		*dp++ = (char)file.getch();
		*dp++ = (char)file.getch();
		*dp++ = (char)file.getch();
		*dp++ = (char)file.getch();
#endif

		int ct = 4;
		int c;

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
		if (ct == 5)
			break;
		if (nbyte + _KLENTOBYTE(ct) >= indexsize){
			break;
		}

		if ( indexnum >= pindexnum ){
//			PINDEX FAR *ptr = (PINDEX FAR *)pmem.realloc( index, sizeof(PINDEX) * (pindexnum + PINDEX_SLOTNUM) );
			PINDEX FAR *ptr = (PINDEX FAR *)pmem.alloc( sizeof(PINDEX) * (pindexnum + PINDEX_SLOTNUM) );
//			DBW("%08X %08X", index, ptr);
			if ( !ptr ){
				pmem.unlock();
				delete buf;
				error = 2;
				return -1;
			}
			memcpy( ptr, index, sizeof(PINDEX) * pindexnum );
			pmem.free( index );
			pindexnum += PINDEX_SLOTNUM;
			index = ptr;
		}


		int l = sizeof(INDEX) + _kcsbyte((_kchar*)(buf+sizeof(t_pbn2)));
		PINDEX ip = alloc(l);
		if ( !ip ){
			pmem.unlock();
			delete buf;
			error = 2;
			return -1;		//メモリ不足
		}
		index[indexnum++] = ip;
		_fmemcpy(ip, buf, l);
		nbyte += l;
	}
	delete buf;
#endif
	pmem.unlock();
	return indexnum;
}

#if USE_MMI

//返り値：-1 容量不足エラー
//		  上記以外 読み込んだindex数
int HPdicIndex::getIndexMM( )
{
	if ( !header->index_blkbit )
		return PdicIndex::getIndexMM();

	nbyte = 0;
	indexnum = 0;

	__assert(mmf!=NULL);
		
	pmem.lock();

	const char *top = (const char*)mmf->data();

	const char *sp = top;
	while ( 1 ){
		int l = sizeof(t_pbn2) + _kcsbyte1((_kchar*)(sp+sizeof(t_pbn2)));
		if ( *(_kchar*)&sp[sizeof(t_pbn2)] == 0 ){
			break;
		}
		if ( indexnum >= pindexnum ){
			break;
		}
		index[indexnum++] = (INDEX*)sp;
		nbyte += l;		// disk上のbytes
		sp += l;
		if ( sp >= top + indexsize )
			break;
	}
	pmem.unlock();
	__assert(!index16);
	return indexnum;
}

int HPdicIndex::enterEdit()
{
	__assert(mmf!=NULL);

	if (!Has32())
		return PdicIndex::enterEdit();

#ifdef SWAP_ENDIAN
	#error	// not implemented.
#endif

	nbyte = 0;
	pmem.lock();
	for (int i=0;i<indexnum;i++){
		int l = sizeof(t_pbn2) + _kcsbyte1((_kchar*)((char*)index[i]+sizeof(t_pbn2)));
		PINDEX ip = alloc(l);
		if ( !ip ){
			pmem.unlock();
			error = 2;
			return -1;		//メモリ不足
		}
		_fmemcpy(ip, index[i], l);
		index[i] = ip;
		nbyte += l;
	}
	pmem.unlock();
	delete mmf;
	mmf = NULL;
	return 0;
}
#endif

int HPdicIndex::WriteIndex( )
{
#if USE_MMI
	if (mmf)
		return 0;	// not modified
#endif

	if ( !Has32() ){
		return PdicIndex::WriteIndex();
	}

//	if ( !Modified ) return 0;
	// ここではねておきたいが、
	// チェックをするのが大変そうなので、、後ほど

	if (seek_index() == -1) return -1;

	pmem.lock();
	for ( int bn=0;bn<indexnum;bn++){
		file.write( index[bn], sizeof(t_pbn2) + _kcsbyte1(index[bn]->word) );
	}
	if ( indexsize > 0 && nbyte < indexsize){
		file.write("\0\0\0\0", min(indexsize - nbyte, 4u));
	}
	pmem.unlock();
	Modified = false;
	return 0;
}

bool HPdicIndex::Has32()
{
#if USE_MMI
	if (index16)
		return false;
#endif

	for ( int i=0;i<indexnum;i++ ){
		if ( index[i]->block > 0xffff ){
			return true;
		}
	}
	return false;
}

