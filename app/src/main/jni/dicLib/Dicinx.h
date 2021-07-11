#ifndef __Dicinx_h
#define	__Dicinx_h

#define	FINX	1	// fast index

#if USE_MMI
#include "mmf.h"
#endif

struct INDEX {
	t_pbn2 block;	// NEWDIC3ではここがBLK_ERRORだと32bitの物理indexと判断する
	_kchar word[1];
};

struct INDEX16 {
	t_pbn block;
	_kchar word[1];
};

typedef INDEX *PINDEX;

#define	INDEX_SLOTSIZE	(sizeof(PINDEX)*MAX_INDEX)

///////////////////////
///	PdicIndexクラス	///
///////////////////////
//	マルチアクセス制御
//	インデックス番号を長く保持しないこと
#define	PINDEX_SLOTNUM	256
class PdicIndex : public PdicObject {
protected:
	unsigned offset;
	unsigned indexsize;		// ディスク上の最大インデックスサイズ
	const HEADER *header;
	uint nbyte;			// INDEXのディスク上の占有容量
						// NEWDIC3以上の時は、memory上での占有量であるため、
						// 本当のdisk上の容量を得るときは、GetIndexBytesOnDisk()を呼ぶ
						// しかし、NEWDIC3ではnbyteをどの場合においても32bit indexとして計算し、
						// 実際のdisk上の容量より多く扱っている(diskを余分に消費する)
	int indexnum;		//使用中のインデックス数
	PdicMem pmem;					//INDEX用のメモリ領域
	FileBuf &file;
	bool Modified;
protected:
	PINDEX alloc( int l )			//インデックス用のメモリ取得
		{ return (PINDEX)pmem.alloc(l); }
	void dealloc( int lbn )				//インデックスのメモリ解放
#if FINX
		;
#else
		{ pmem.free( index[lbn] ); }
#endif
	PINDEX *index;
	int pindexnum;		// index配列のメモリ割り当て数
#if USE_MMI
	TMmfMap *mmf;
	const INDEX16 **index16;
#endif
#if FINX
	class SimpleAllocator *indexAllocator;
#endif
protected:
	int seek_index( )
		{ return file.seek( offset ); }
	virtual int getIndex( );				//要返り値チェック
#if USE_MMI
	virtual int getIndexMM( );
	virtual int enterEdit();
#else
	inline int enterEdit() { return 0; }
#endif

public:
	PdicIndex( int &error, FileBuf &_file );
	virtual ~PdicIndex();
	FileBuf &GetFile() const
		{ return file; }

	int Open( const HEADER *header );
	virtual void Close( );
	bool Flush();

	unsigned GetOffset()
		{ return offset; }
#ifdef EXTHEADER
	void ChangeOffset( int _offset )
		{ offset = _offset; }
#endif
	int GetIndexNum( )	{return indexnum;}
	unsigned GetIndexSize( ) {return indexsize;}
	unsigned GetIndexBytesOnDisk( )		// NEWDIC3では若干時間がかかるので注意
		;
	virtual int WriteIndex( );

	int indexSearch( const _kchar * word );
	int indexSearch2( const _kchar * word );

#ifdef _Windows
#if USE_MMI
	inline t_pbn2 indexBlock( int lbn )
		{ return index16 ? index16[lbn]->block : index[lbn]->block; }
#else
	inline t_pbn2 indexBlock( int lbn )
		{ return index[lbn]->block; }
#endif
	void indexWord( _kchar *buf, int lbn )
		{ _kcscpy( buf, index[lbn]->word ); }
	void setIndexBlock(int lbn, t_pbn2 pbn)
		{ index[lbn]->block = pbn; Modified=true; }
#else
	t_pbn2 indexBlock( int lbn );
	void indexWord( _kchar *buf, int lbn );
	void setIndexBlock(int lbn, t_pbn2 pbn);

#endif

#if USE_MMI
	inline const _kchar *indexWord( int lbn )
		{ return index16 ? index16[lbn]->word : index[lbn]->word; }
#else
	inline const _kchar *indexWord( int lbn )
		{ return index[lbn]->word; }
#endif

	int delIndex(int lbn);
	int renIndex(int lbn, const _kchar * word);	//要返り値チェック
	int renIndexSort( int lbn, const _kchar *word );	// sort付きrename
	int addIndex(int lbn, t_pbn2 pbn, const _kchar * word);	//要返り値チェック
	int searchLbn( t_pbn2 pbn );			// 物理ブロック番号から論理ブロック番号を求める
	unsigned getRemain( );

	// Utilityで使用する関数		//
	unsigned int getIndexDiskSize(void)	{return nbyte;}

	void GrowIndex( int num );
	bool CanGrowIndex(int num) const;
	void MoveIndex( int num );
	void MoveGrowIndex( int num );

	bool IsModified() const
		{ return Modified; }
};

#if FINX
class SimpleAllocator {
	char *buffer;
	unsigned size;
	char *curp;
	unsigned used;
public:
	SimpleAllocator(unsigned _size)
		:size(_size)
	{
		buffer = (char*)malloc(size);
		curp = buffer;
		used = 0;
	}
	~SimpleAllocator()
	{
		if (buffer) free(buffer);
	}
	bool ok() const
	{
		return buffer != NULL;
	}
	void *alloc(unsigned reqsize)
	{
		if (reqsize > size - used){
			return NULL;
		}
		char *ret = curp;
		curp += reqsize;
		used += reqsize;
		return ret;
	}
	bool is(const void *ptr) const
	{
		return ptr >= buffer && ptr < curp;
	}
};
#endif

#endif
