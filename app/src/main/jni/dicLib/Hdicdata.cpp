#include	"tnlib.h"
#pragma	hdrstop
#include "dic.h"
#include	"japa.h"
#include "dicdbg.h"

//#define	AVE_LD	64		// 平均データ長(なるべく大きめに) for blocksize=256
#define	AVE_LD	(128)		// 平均データ長(なるべく大きめに) for blocksize=1024,4096

// 分割の基準を定める
int GetNd( int Nr )
{
	// Nd table
	// Nd : Nr		Nd : Nr		Nd : Nr
	//  0 : 0		 8 : 2048	16 : 1,048,576
	//  1 : 2		 9 : 4608	17 : 2,228,224
	//  2 : 8		10 : 10240	18 : 4,718,592
	//  3 : 24		11 : 22528	19 : 9,961,472
	//  4 : 64		12 : 49152	20 : 20,971,520
	//  5 : 160		13 : 106496
	//  6 : 384		14 : 229376
	//  7 : 896		15 : 491520
	const int NdMin = 8;
	const int NdMax = 20;
	int Nd;
	for ( Nd=NdMin;Nd<=NdMax;Nd++ ){
		if ( Nr < Nd * (1<<Nd) ){
			break;
		}
	}
	//return Nd*32;	// 経験による
	return Nd*32;	// 経験による for blocksize=1024
	//return Nd*32*16;	// 経験による for blocksize=4096
}

HPdicData::HPdicData( int &error, FileBuf &_file)
	:PdicData( error, _file )
{
	DicDec = NULL;
	TempBuffer = NULL;
	TempBufferSize = 0;
}

HPdicData::~HPdicData()
{
	delete[] TempBuffer;
}

int HPdicData::Open( const HEADER *header, class IndexData *inxdat )
{
	int r = super::Open( header, inxdat );
	if (r) return r;

	return 0;
}

#ifdef SWAP_ENDIAN
// 先頭のblock num以外をswap
void SwapData( uint8_t *databuf )
{
#if !defined(NEWDIC4) || !defined(NOFIELD2) || defined(USE_COMP)
#line
#error	NEWDIC4、NoField2、非圧縮のみ対応
#endif
	// Field1 typeのみ
	FieldFormat *field = (FieldFormat*)(databuf + sizeof(t_blknum));
	while ( field->fieldlen ){
		SWAP_SELF( field->fieldlen );	// fieldlen
		FieldFormat *next = NextField( field );
#if defined(UNICODE)
		wa_t attr = field->attr;
		// 見出語スキップ
		_kchar *p = field->word;
		while ( *p ){
			SWAP_SELF(*p);
			p++;
		}
		p++;	// NULL
		// 日本語訳スキップ
		if ( attr & WA_EX ){
			while ( *p ){
				SWAP_SELF(*p);
				p++;
			}
			p++;	// NULL
			uint8_t *src = (uint8_t*)p;
			for(;;){
				uint8_t jt = *src++;
#ifdef NEWDIC4UNI
				src++;	// reserved
#endif
				if ( jt & JT_END )
					break;
				if ( jt & JT_BYTE ){
					// 未定義
					uint jtb = *(t_jtb*)src;
				} else {
					p = (_kchar*)src;
					while ( *p ){
						SWAP_SELF(*p);
						p++;
					}
					p++;	// NULL
					src = (uint8_t*)p;
				}
			}
		} else {
			while ( p < (_kchar*)next ){
				SWAP_SELF(*p);
				p++;
			}
		}
#else
#line
#error ANSIでのswapは未対応
#endif
		field = next;
	}
}
#endif


int HPdicData::_write_databuf(t_pbn2 pbn)
{
	int blknum = *(t_blknum*)getDataBuf();
	int datasize = blocksize * ( blknum ? (blknum & ~FIELDTYPE) : 1 );
#if defined(SWAP_ENDIAN)
	uint8_t *__databuf = new uint8_t[ datasize ];
	memcpy( __databuf, databuf, datasize );
	SwapData( __databuf );
	if (file_write( pbn, __databuf, datasize ) < 0)
#else
	if (file_write( pbn, getDataBuf(), datasize ) < 0)
#endif
	{
#if defined(SWAP_ENDIAN)
		delete[] __databuf;
#endif
		return -1;
	}
#if defined(SWAP_ENDIAN)
	delete[] __databuf;
#endif
	return 0;
}
#if MEMMAPDIC
int HPdicData::read(t_pbn2 pbn)
{
	databuf.setDataPbn(pbn);
	return 0;
}
#endif

#if !MEMMAPDIC
//TODO: 2007.8.18 PdicData::read()に統合できないか？
//TODO: 2007.8.18 それによりTDataBuf::setLastPbn()を無くしたい
int HPdicData::read(t_pbn2 pbn, TDataBuf *_databuf, bool fit_size)
{
	TDataBuf &databuf = _databuf ? *_databuf : this->databuf;

	if (databuf.isLastPbn(pbn)){		// 読みだし＆修正後、同じブロックの読みだしを行わないことを前提としている
		return 0;
	}
#if USE_DICFASTMODE
	if (!databuf.flush()){
		return -1;
	}
#endif

	TAutoLock lock(Mutex);
	t_blknum _blknum;
	int r = file_read(pbn, &_blknum, sizeof(t_blknum));
	if ( r < sizeof(t_blknum) ){
		error = DICERR_READWRITE;	// 読み取りエラー・ファイルサイズ以降から読もうとした？
		return -1;
	}
#ifdef SWAP_ENDIAN
	SWAP_SELF( _blknum );
#endif
	uint8_t *br;
	if (fit_size){
		br = databuf.alloc(pbn, _blknum);
	} else {
		br = databuf.alloc_opt(pbn, _blknum);
	}
	if (!br){
		error = DICERR_NOMEMORY;		//メモリ不足
		return -1;
	}
	//*(t_blknum*)databuf.getDataBuf() = _blknum;
	t_blknum alloc_blknum = (_blknum==0 ? 1 : _blknum);		// 空きブロック対策
	int blksz = blocksize * (alloc_blknum & ~FIELDTYPE);
	{
		r = file.read( databuf.getDataBuf() + sizeof(t_blknum), blksz - sizeof(t_blknum) );
		if (r != blksz-sizeof(t_blknum)){
			error = DICERR_READWRITE;
			return -1;
		}
	}
#if defined(SWAP_ENDIAN)
	SwapData( databuf.getDataBuf() );
#endif
	databuf.setLastPbn(pbn);
	databuf.resetWrite();	// 2011.5.10 追加(databuf.alloc(xxx)で不要なwritefが立ってしまうため
	return 0;
}
bool HPdicData::ReallocTempBuffer(unsigned nsize)
{
	if (TempBufferSize<nsize){
		delete[] TempBuffer;
		TempBuffer = new char[nsize];
		if (!TempBuffer)
			return false;
		TempBufferSize = nsize;
	}
	return true;
}
#endif	// !MEMMAPDIC

/*======================================================================*/
/*uint8_t *PdicData::recSearch()											*/
/*======================================================================*/
/*databufからswordに該当する位置（ポインタ）を探す（bufは１ブロック）	*/
/*正確にはレコード長のポインタを指す									*/
/*一致した場合は１つ前の位置。											*/
/*																		*/
/*wpは圧縮参照用単語で、戻ると該当する単語の完全な形を保有する			*/
/*buf[0]は'\0'のときもあるので注意！（つまり、終端）					*/
// ブロックの最後であった場合は 0 を返します
// このとき、fw.fwordは""となっています
/*======================================================================*/
int HPdicData::recSearch( const _kchar *sword, _kchar *wbuf, FINDWORD &fw, TDataBuf &databuf)
{
#if 1	// 非最適化バージョン
	_kchar wpp[LWORD+1];
	_kchar *wp1 = wpp;

	_kcscpy(wpp, wbuf);
				//ここがネック
				//キャッシュを効かせて、辞書の新規マージをした場合、
				//Pdic2::record()でbsearch()が89%,
				//bsearch()中recSearch()がかなりの割合を占める。
	int l=0;
	int fieldlen;
	FieldFormat *buf = ((TDataBuf&)databuf).GetTopDataPtr( );
#ifndef NOFIELD2
	if ( databuf.isField2() ){
//		fieldlen = FF2(buf)->fieldlen;
		if ( cmpnword( sword, wpp, LWORD ) <= 0 ){
			// 先頭
			fw.lp = sizeof(t_blknum);
			fw.word1.clear();
			_kcscpy( fw.fword, wbuf );
			return 1;
		} else {
			// 最後
			buf = (FieldFormat*)NextField2(FF2(buf));
			fw.lp = FP_DIFF( buf, databuf.getDataBuf() );
			fw.word1.set( wpp );
			fw.fword[0] = '\0';
			return 0;
		}
#if 0
		while ( (fieldlen=FF2(buf)->fieldlen) != 0 )
		{
			strcpy(wbuf, wpp);			/* １つ前の単語をセーブ */
			strcpy( wpp + FF2(buf)->complen, FF2(buf)->word );
			if (cmpnword(sword, wpp, LWORD) <= 0){
				break;
			}
			l = fieldlen + L_FieldHeader2;
			((uint8_t*)buf += l);
		}
#endif
	} else
#endif
	{
#if 1	// fast version
		//Note: ここを処理した後は、wppを直接参照せず、wp1を使用すること
		int ll = _kcsbyte1( wpp );
		wp1 = wpp;	// equivalent to wbuf
		_kchar *wp2 = wbuf;	// equivalent to wpp
		while ( (fieldlen=buf->fieldlen) != 0 ){
			// swap mp1/mp2
			_kchar *wpt = wp1;
			wp1 = wp2;
			wp2 = wpt;
			// wp1 : wpp
			// wp2 : wbuf

			ll = _kcsbyte1(buf->word);
			memcpy(wp1, wp2, _KLENTOBYTE(buf->complen));
			memcpy(wp1+buf->complen, buf->word, ll);
			ll += _KLENTOBYTE(buf->complen);	// ll is the length of wpp, which includes '\0'
#if 0	// for debug
			{
				_kstrdef(s,wp1,false);
				DBW("wp1=%ws", (const tchar*)s);
			}
#endif
			if (ll-1>LWORD){	// -1 means the above code (ll = _kcsbytes1(buf->word)) includes one byte for null terminator.
				error = DICERR_CORRUPTED;
				__assert__;
				return -1;
			}
			//TODO: buf->complenを使用して比較する部分を減らすことができるはず(swordの長さがあるのでちょっと注意)
#ifdef USE_DICORDER
			if (cmpnword(sword, wp1, LWORD) <= 0)
#else
			if (_kcsncmp(sword, wp1, _KBYTETOLEN(ll)) <= 0)	//TODO: 2012.3.11 null terminatorを含むllを指定する意味はあるのか？
#endif
			{
				// wp2 is wbuf
				break;
			}
			l = fieldlen + L_FieldHeader;
			//*(uint8_t**)&buf += l;
			buf = (FieldFormat*)((uint8_t*)buf + l);
		}
		if (wp2!=wbuf)
			_kcscpy(wbuf, wp2);
#else	// not fast version
		int ll = _kcsbyte1( wpp );
		while ( (fieldlen=buf->fieldlen) != 0 ){
#if 0
			_kcscpy(wbuf, wpp);			/* １つ前の単語をセーブ */
			_kcscpy(wpp + buf->complen, buf->word);
#else
			memcpy( wbuf, wpp, ll );
			ll = _kcsbyte1(buf->word);
			memcpy( wpp + buf->complen, buf->word, ll );
			ll += _KLENTOBYTE(buf->complen);	// ll is the length of wpp, which includes '\0'
#endif
#ifdef USE_DICORDER
			if (cmpnword(sword, wpp, LWORD) <= 0)
#else
			if (_kcsncmp( sword, wpp, _KBYTETOLEN(ll) ) <= 0)
#endif
			{
				break;
			}
			l = fieldlen + L_FieldHeader;
			*(uint8_t**)&buf += l;
		}
#endif
	}

	// 1996.12.31
	// pへわざわざ代入している理由が分からない
	// bufを直接利用

	if ( !l ){
		// データが無い場合または先頭
		fw.lp = sizeof(t_blknum);
		fw.word1.clear();
	} else {
		fw.lp = FP_DIFF( buf, databuf.getDataBuf() );
		if ( fieldlen ){
			fw.word1.set( wbuf );
		} else {
			// 一番最後の場合(1996.12.25)
			fw.word1.set(wp1);
		}
	}

	if ( fieldlen ){
		_kchar *dp = fw.fword;
		memcpy( dp, wbuf, _KLENTOBYTE(buf->complen) );
		dp += buf->complen;
		_kcscpy( dp, buf->word );	// 見出語のみコピー
		return 1;
	} else {
		fw.fword[0] = '\0';
		return 0;
	}
#else	// 最適化バージョン(あまり速くなっていないような気がする・・・)
		//*+++ まだ動作確認をしていないため、本公開禁止
	_kchar wpp[LWORD+1];

	_kcscpy(wpp, wbuf);

	FieldFormat *buf = GetTopDataPtr( );
#ifndef NOFIELD2
	if ( databuf.isField2() ){
		if ( cmpnword( sword, wpp, LWORD ) <= 0 ){
			// 先頭
			fw.lp = sizeof(t_blknum);
			fw.word1.clear();
			_kcscpy( fw.fword, wbuf );
			return 1;
		} else {
			// 最後
			buf = (FieldFormat*)NextField2(FF2(buf));
			fw.lp = FP_DIFF( buf, databuf );
			fw.word1.set( wpp );
			fw.fword[0] = '\0';
			return 0;
		}
	} else
#endif
	{
		fw.word1.clear();
		int ll = _kcsbyte1( wpp );
		while ( buf->fieldlen != 0 ){
			memcpy( wbuf, wpp, ll );
			ll = _kcsbyte1(buf->word);
			memcpy( wpp + buf->complen, buf->word, ll );
			ll += _KLENTOBYTE(buf->complen);	// ll is the length of wpp, which includes '\0'
#ifdef USE_DICORDER
			if (cmpnword(sword, wpp, LWORD) <= 0)
#else
#if defined(UNICODE) && !MIXDIC && !defined(KMIXDIC)
			if ( _tcsncmp( sword, wpp, _KBYTETOLEN(ll) ) <= 0 )
#else
			if ( memcmp( sword, wpp, ll ) <= 0 )
#endif
#endif
			{
				break;
			}
			*(uint8_t**)&buf += buf->fieldlen + L_FieldHeader;
		}
	}

	// 1996.12.31
	// pへわざわざ代入している理由が分からない
	// bufを直接利用

	fw.lp = FP_DIFF( buf, databuf );
	if ( fw.lp > sizeof(t_blknum) ){
		if ( buf->fieldlen ){
			fw.word1.set( wbuf );
		} else {
			// 一番最後の場合(1996.12.25)
			fw.word1.set( wpp );
		}
	}

	if ( buf->fieldlen ){
		_kcscpy( fw.fword, wpp );
		return 1;
	} else {
		fw.fword[0] = '\0';
		return 0;
	}
#endif
}

#if !defined(SMALL)
//*+++ 動作未確認
// 準備：
// 呼び出し側で dataの先頭にseekしてあること！
int HPdicData::recSearchDirect( const _kchar *sword, _kchar *wbuf, FINDWORD &fw, TDataBuf &databuf )
{
	_kchar wpp[LWORD+1];

	_kcscpy(wpp, wbuf);

	FieldFormat fft;

#ifndef NOFIELD2
	if ( databuf.isField2() ){
//		fieldlen = FF2(buf)->fieldlen;
		if ( cmpnword( sword, wpp, LWORD ) <= 0 ){
			// 先頭
			fw.lp = sizeof(t_blknum);
			fw.word1.clear();
			_kcscpy( fw.fword, wbuf );
			return 1;
		} else {
			// 最後
			file_seekcur( sizeof(t_blknum) );
			FieldFormat2 ff2;
			file_read( &ff2, sizeof(ff2) );
			fw.lp = sizeof(t_blknum) + L_FieldHeader2 + ff2.fieldlen;
			fw.word1.set( wpp );
			fw.fword[0] = '\0';
			return 0;
		}
	} else
#endif
	{
		fw.word1.clear();
	
		file_seekcur( sizeof(t_blknum) );
		fw.lp = sizeof(t_blknum);
		int ll = _kcsbyte1( wpp );
		for (;;){
			file_read( &fft, L_FieldHeader );
			if ( fft.fieldlen == 0 )
				break;
			memcpy( wbuf, wpp, ll );
			file_read( wpp + fft.complen, _KLENTOBYTE(LWORD - fft.complen) );
			ll = _kcsbyte1(wpp);	// ll is the length of wpp, which includes '\0'
#ifdef USE_DICORDER
			if (cmpnword(sword, wpp, LWORD) <= 0)
#else
#if defined(UNICODE) && !MIXDIC && !defined(KMIXDIC)
			if ( _tcsncmp( sword, wpp, _KBYTETOLEN(ll) ) <= 0 )
#else
			if ( memcmp( sword, wpp, ll ) <= 0 )
#endif
#endif
			{
				break;
			}
			file_seekcur( fft.fieldlen - _KLENTOBYTE(LWORD - fft.complen ) );
			fw.lp += fft.fieldlen + L_FieldHeader;
		}
	}

	if ( fw.lp > sizeof(t_blknum) ){
		if ( fft.fieldlen ){
			fw.word1.set( wbuf );
		} else {
			// 一番最後の場合
			fw.word1.set( wpp );
		}
	}

	if ( fft.fieldlen ){
		_kcscpy( fw.fword, wpp );	// 見出語のみコピー
		return 1;
	} else {
		fw.fword[0] = '\0';
		return 0;
	}
}
#endif

// Field1のみ対応(Field2はEraseBlockで)
// 戻り値=1 : Shrinkを行った
int HPdicData::eraseWord( t_pbn2 pbn, int loc )
{
	_kchar *_dp;
	int clen;
	FieldFormat *dp = databuf.GetDataPtr( loc );
	FieldFormat *p, *sp;
	sp = NextField( dp );
	if ( sp->fieldlen ){
		p = sp;
		while ( p->fieldlen ){			//ブロックの最後を検索
			p = NextField( p );
		}
		int a = dp->complen;	//削除する単語の圧縮情報(charactor count)
		int b = sp->complen;	//次の単語の圧縮情報(charactor count)
		if (a < b){
			dp->fieldlen = (tfield)(sp->fieldlen + _KLENTOBYTE(b - a));
			dp->complen = (tcomp)a;	// 削除後の単語の圧縮情報
			_dp = dp->word + b-a;
		} else {
			dp->fieldlen = sp->fieldlen;
			dp->complen = (tcomp)b;	// 削除後の単語の圧縮情報
			_dp = dp->word;
		}

		clen = FP_DIFF( p, sp->word ) /* + sizeof( tfield ) */;
#ifdef NEWDIC4
		((char*)dp->word)[-1] = ((char*)sp->word)[-1];	// 属性のコピー
#endif
		memmove( _dp, sp->word, clen );
	} else {
		// ブロックの最後の単語を削除する場合
		clen = 0;
		_dp = (_kchar*)dp;
	}
	int remainsz = blocksize * databuf.getBlockNum() - (FP_DIFF( _dp, getDataBuf()) + clen);
	memset( (uint8_t*)_dp + clen, 0, remainsz );
	// Shrink
	// remainszはGetBlockNum()==1のとき、remainsz <= blocksize - sizeof(t_blknum)
	// つまり、ShrinkBlockでeraseされることはない
	remainsz -= sizeof(tfield);	// 終端
	if ( remainsz >= blocksize ){
		ShrinkBlock( pbn, (t_blknum)(databuf.getBlockNum() - remainsz / blocksize) );
		write( pbn );
		nword--;
		return 1;
	}
	nword--;
	return 0;
}

//pbnを空きブロック化
int HPdicData::eraseBlock( t_pbn2 pbn )
{
	if ( !emptylist ){
		OpenEmptyList( );
	}
	emptylist->Register( pbn, databuf.getBlockNum() );
	return 0;
}

// numfld:
//	= num + fieldtype
// oldblknumからnumに拡張
// databufを使用しない(if dataset is zero)
// 自力のGrowも可能,Growができない場合、新規ブロックの確保も行える(戻り値をチェック!)
// メンバ変数のblknumの変更は呼び出し側が行うこと！！
// datasetが1であると現在のdatabufを拡張し、適切な処理を行う
// Field1,Field2兼用(ただし、numfldはFIELDTYPEとorをとっておくこと)
t_pbn2 HPdicData::GrowBlock( t_pbn2 pbn, t_blknum oldnumfld, t_blknum numfld, int dataset )
{
	t_pbn2 newpbn;
	t_blknum oldnum = oldnumfld & ~FIELDTYPE;
	t_blknum num = numfld & ~FIELDTYPE;

#if !MEMMAPDIC
	uint8_t *_databuf = NULL;
	if ( dataset ){
		_databuf = new uint8_t[ blocksize * num ];
		if ( !_databuf ){
			error = DICERR_NOMEMORY;	// メモリ不足
			return BLK_ERROR;
		}
	}
#endif

	if (!Flush()){
		return BLK_ERROR;
	}

	if ( pbn + oldnum == nblock ){
		// 最終ブロックである場合、自力Grow
		if ( file_seek_alloc( pbn + num ) != -1 ){
			if (databuf.writeBlockNum(pbn, numfld)){
				nblock += num - oldnum;
				newpbn = pbn;
				goto normal;
			}
		}
		// seekできない場合、ディスクいっぱいであると考え、空きブロックから取得
	} else {
		if ( !emptylist ){
			OpenEmptyList( );
		}
		if ( emptylist->HasBlock() ){
			// 次のブロックが空きブロックである場合、自力Growできるか調べる
			int snum = emptylist->GetSeq( pbn + oldnum );
			if ( snum >= 1 ){
				// 空きブロックにあったか？
				if ( snum >= num - oldnum ){
					emptylist->Free( pbn + oldnum, num - oldnum );
					databuf.writeBlockNum(pbn, numfld);
					newpbn = pbn;
					goto normal;
				}
				// 空きブロック＋辞書の終端であったか？
				if ( pbn + oldnum + snum == nblock ){
					if ( seek( pbn + num ) != -1 ){
						if (databuf.writeBlockNum(pbn, numfld)){
							emptylist->Free( pbn + oldnum, (t_blknum)snum );
							nblock += num - oldnum - snum;
							DD( DD_SETNBLOCK, nblock );
							newpbn = pbn;
							goto normal;
						}
					}
				}
			}
		}
	}

	if ( !emptylist ){
		OpenEmptyList();
	}
	newpbn = emptylist->NewBlock( num );

	if ( newpbn == BLK_ERROR ){
		if ( nblock >= MAX_PBN2 ){
			error = DICERR_INDEXFULL;	// インデックス数不足
#if !MEMMAPDIC
			if ( _databuf ) delete[] _databuf;
#endif
			return BLK_ERROR;
		}
		databuf.resetWrite();
		databuf.invalidate();
		newpbn = nblock;
		if ( file_seek_alloc( newpbn + num ) == -1 ){
			error = DICERR_DATAFULL;
#if !MEMMAPDIC
			if ( _databuf ) delete[] _databuf;
#endif
			return BLK_ERROR;
		}
		databuf.writeBlockNum(newpbn, numfld);
		nblock += num;
		DD( DD_SETNBLOCK, nblock );
	}
	emptylist->Register( pbn, oldnum );

normal:

	if ( dataset ){
		//TODO: 統合できる？
#if MEMMAPDIC
		databuf.grow(pbn, newpbn, oldblknum, num);
#else
		memcpy( _databuf, getDataBuf(), oldnum * blocksize );
		memset( _databuf + oldnum*blocksize, 0, (num-oldnum)*blocksize);
		databuf.setDataBuf(_databuf, numfld, newpbn);
#endif	// !MEMMAPDIC
	}

	return newpbn;
}

t_pbn2 HPdicData::newBlock( int numtype )
{
	if (!Flush()) return BLK_ERROR;
	int f = 0;
	if ( !emptylist ){
		OpenEmptyList();
	}
	t_pbn2 newpbn = emptylist->NewBlock( numtype & ~FIELDTYPE );
	if ( newpbn == BLK_ERROR ){
		if ( nblock >= MAX_PBN2 ){
			error = DICERR_INDEXFULL;	// インデックス数不足
			return BLK_ERROR;
		}
		f = 1;
		newpbn = nblock;
	}
	if ( databuf.getBlockNumType() != numtype ){
		if (!databuf.alloc(newpbn, numtype)){
			error = DICERR_NOMEMORY;	// メモリ不足
			return BLK_ERROR;
		}
	}

//	file_seek(newpbn);
	memset((char*)databuf.GetTopDataPtr(), 0, blocksize*(numtype & ~FIELDTYPE)-sizeof(t_blknum));
#if 1	// 高速化のため#else以下のほうがいいが、うまくいかない
		// 原因：ここでfile.write()したものが、次のread()でうまくいっていない模様
		// 辞書種類の変更で確かめてみると良い
	if ( write(newpbn) == -1 || !Flush( ) ){
		error = DICERR_DATAFULL;
		return BLK_ERROR;	// 恐らくディスク容量不足エラー
	}
#else
	// ブロック数の書き込み
	if ( file_write( newpbn, &numtype, sizeof(t_blknum) ) == -1 ){
		error = DICERR_DATAFULL;
		return BLK_ERROR;
	}
	// 確保ブロックの最終ブロックの終端に'\0'を書き込む
	if ( file_write( newpbn + (numtype & ~FIELDTYPE) - 1, "\0", sizeof(uint8_t) ) == -1 ){
		error = DICERR_DATAFULL;
		return BLK_ERROR;
	}
#endif
	if ( f ){
		nblock += (numtype & ~FIELDTYPE);
		DD( DD_SETNBLOCK, nblock );
	}
	return newpbn;
}

#if 1
// 任意の場所から連続num個探す
t_pbn2 HPdicData::NewBlock( int num )
{
	if ( !emptylist ){
		OpenEmptyList( );
	}
	int seqblknum;
	t_pbn2 newpbn = emptylist->NewBlock( num, &seqblknum );
	if ( newpbn != BLK_ERROR ){
		if ( seqblknum == num ){
			// 空きﾌﾞﾛｯｸから得られた
			goto normal;
		} else if ( newpbn + seqblknum == nblock ){
			// 終端ﾌﾞﾛｯｸが空きﾌﾞﾛｯｸであった場合
			if ( seek( newpbn + num ) != -1 ){
				if ( file_write( newpbn, &num, sizeof(t_blknum) ) != -1 ){
					nblock += num - seqblknum;
					DD( DD_SETNBLOCK, nblock );
					goto normal;
				}
			}
		}
		// 失敗した場合は再登録
		emptylist->Register( newpbn, seqblknum );
	}
	if ( nblock >= MAX_PBN2 ){
		error = DICERR_INDEXFULL;	// インデックス数不足
		return BLK_ERROR;
	}
	databuf.resetWrite();
	databuf.invalidate();
	newpbn = nblock;
	if ( file_seek_alloc( newpbn + num ) == -1 ){
		error = DICERR_DATAFULL;
		return BLK_ERROR;
	}
	file_write( newpbn, &num, sizeof(t_blknum) );
	nblock += num;
	DD( DD_SETNBLOCK, nblock );

normal:

	return newpbn;
}
#endif

void HPdicData::ShrinkBlock( t_pbn2 pbn, t_blknum newnum )
{
	if ( !emptylist ){
		OpenEmptyList();
	}
	emptylist->Register( pbn + newnum, (t_blknum)(databuf.getBlockNum() - newnum) );
	databuf.shrink(newnum);
}

//fw.pbnのブロックに単語を追加
//必ず追加できる余裕があるものとする（無いとエラー）
// 分割が必要な場合、divinfo != NULLなら、そこに適当な情報を書き込む
// Field1専用関数 //
// wordlen:
//	the character length of the word.
int HPdicData::addWord( const _kchar *word, uint wordlen, const uint8_t *japa, uint japalen, FINDWORD &fw, DivInfo *divinfo )
{
	if (read(fw.pbn) == -1){
		return -1;
	}

	t_pbn2 newpbn = BLK_ERROR;
	// byte count //
	int d, ib;
	// charactor count //
	int c;
	int bb;

	int nbl;			// 次の単語のレコード長(next block len)

	FieldFormat *sp, *p;
	sp = p = databuf.GetDataPtr( fw.lp );
	tfield ic = p->fieldlen;
	
	if ( fw.lp > sizeof(t_blknum) ){
		c = get_complen( fw.word1, word );		/* 新しい単語の圧縮文字数 */
	} else {
		c = 0;
	}
	if (!wordlen)
		wordlen = _kcslen(word);
	ib = _KLENTOBYTE(wordlen - c + 1) + L_FieldHeader + japalen;		// 追加されるバイト数
#ifdef NEWDIC4
	wa_t bak_attr = 0;
#endif
    
	while ( sp->fieldlen ){
		sp = NextField( sp );
	}
	if ( ic ){
		bb = get_complen( fw.fword, word );
		d = _KLENTOBYTE(bb - p->complen);		// 次の単語の圧縮文字数増減分(byte count)
	} else {
		bb = 0;
		d = 0;
	}
	int postsz = FP_DIFF( sp, p );	// 既存のdataの挿入点以降の長さ
	int cursz = FP_DIFF( sp, getDataBuf() ) + sizeof(tfield);	// 現在の使用バイト数
	if (ib-d > (int)( blocksize * databuf.getBlockNum() - cursz ) ){	// 空容量のチェック
		if ( databuf.getBlockNum() > 1 ){
			int n = databuf.GetNumWord( );
			if ( n > GetNd( nword ) ){
		divblock:
				if ( divinfo ){
					divinfo->diff = ib;		// byte count
					divinfo->nextdiff = d;	// byte count
					divinfo->complen = c;	// charactor count
				}
				return -2;	// ブロック分割
			} else {
				if ( databuf.getBlockNum() * blocksize + ib > max((unsigned)n * AVE_LD, blocksize) ){
					goto divblock;
				}
			}
		}
		// ブロック拡張
		int newblknum = ( cursz + ib - d + blocksize - 1 ) / blocksize;
		newpbn = GrowBlock( fw.pbn, databuf.getBlockNum(), (t_blknum)newblknum, 1 );
		if ( newpbn == BLK_ERROR )
			return -1;
		if ( fw.pbn != newpbn ){
			fw.pbn = newpbn;
		} else {
			newpbn = BLK_ERROR;
		}
		read(fw.pbn);
		p = databuf.GetDataPtr( fw.lp );
	}
	if ( databuf.GetTopDataPtr()->fieldlen ){
		nbl = ic - d;		// 次の単語の新しいフィールド長
		word += c;
		memmove( (uint8_t*)p + ib, (uint8_t*)p+d, postsz + sizeof( tfield ) - d );
		// if ignore case Dic,. the number of variable "d" can be
		// minus. To correct this problem, need to add the following codes.
		if ( d < 0 ){
			memcpy( (uint8_t*)p + ib + L_FieldHeader, (const _kchar*)fw.word1 + bb, -d );
		}
		// 次の単語のフィールド長、圧縮長はあとで再セット
#ifdef NEWDIC4
		bak_attr = p->attr;
#endif
	}
	
	p->fieldlen = (tfield)(ib - L_FieldHeader);
#ifdef NEWDIC4
	p->attr = japa[japalen];
#endif
	p->complen = (tcomp)c;
	
	_kchar *q = p->word;
	while ( *word ){		// wordを変更しているので注！！
		*q++ = *word++;
	}
	*q++ = '\0';
	memcpy( q, japa, japalen );
	
	p = NextField( p );
	
	if ( !ic )		// 最後のフィールドである場合、またはこのブロックで最初のデータの場合
		p->fieldlen = 0;
	else {
		p->fieldlen = (tfield)nbl;
		p->complen  = (tcomp)bb;		/* 次の単語の圧縮情報 */
#ifdef NEWDIC4
		p->attr = bak_attr;
#endif
	}

	if (write(fw.pbn) == -1)
		return -1;
	nword++;
	return newpbn != BLK_ERROR ? -3 : 0;
}

#ifndef NOFIELD2
// これは名前と違ってaddではない
// 現在のdatabufにデータを書き込んでwriteするだけ(readはしない)
// wordlen : charactor count of word
int HPdicData::addWord2( const _kchar *word, uint wordlen, const uint8_t *japa, uint japalen, t_pbn2 pbn )
{
	FieldFormat2 *ff2 = (FieldFormat2*)databuf.GetTopDataPtr( );
#ifdef USE_TREE
	if ( dictype & DICID_TREE ){
		ff2->fieldlen = _KLENTOBYTE(1) + japalen;
#ifdef NEWDIC4
		ff2->attr = japa[japalen];
#endif
		ff2->complen = 0;
		ff2->word[0] = '\0';
		memcpy( ff2->word + 1, japa, japalen );
	} else
#endif
	{
		ff2->fieldlen = _KLENTOBYTE(wordlen + 1) + japalen;
#ifdef NEWDIC4
		ff2->attr = japa[japalen];
#endif
		ff2->complen = 0;
		memcpy( ff2->word, word, _KLENTOBYTE(wordlen+1) );
		memcpy( ff2->word + wordlen + 1, japa, japalen );
	}
	NextField2( ff2 )->fieldlen = 0;
	nword++;
	return write( pbn );
}
#endif	// !NOFIELD2
#if !NOUPDATEWORD
//fw.pbnのブロック
//NEWDIC2では追加領域が足りないと -2 を返す
// Field1専用関数 //
int HPdicData::updateWord( const uint8_t *japa, uint japalen, FINDWORD &fw, DivInfo *divinfo )
{
	if (read(fw.pbn) == -1){
		return -1;
	}

	t_pbn2 newpbn = BLK_ERROR;
	int nlen, oldjn, diff, cursz;
	FieldFormat *sp = databuf.GetDataPtr( fw.lp );
	nlen = sp->fieldlen;
	_kchar *p = sp->word;
	while ( *p++ );
	
	sp = NextField( sp );
	oldjn = FP_DIFF( sp, p );		// 古い日本語訳の長さ
	diff = japalen - oldjn;			// 新しい日本語訳との差
	
	while ( sp->fieldlen )
		sp = NextField( sp );			// 一番最後を探す

	cursz = FP_DIFF(sp, getDataBuf()) + sizeof(tfield);	// 現ブロックの使用中の長さを求める
	int newblknum = ( diff + cursz + blocksize - 1 ) / blocksize;
	if ( newblknum > databuf.getBlockNum() ){		// 差分が空容量を超えているかどうかのチェック
		if ( databuf.getBlockNum() > 1 ){
			int n = databuf.GetNumWord( );
			if ( n > GetNd( nword ) ){
		divblock:
				if ( divinfo ){
					divinfo->diff = diff;	// byte count
					divinfo->nextdiff = 0;	// byte count
					divinfo->complen = 0;	// charactor count
				}
				return -2;	// ブロック分割
			} else {
				// 更新では１つのデータでこの条件を満足することがあるため、n > 1が必要
				if ( n > 1 && databuf.getBlockNum() * blocksize > n * AVE_LD ){
					goto divblock;
				}
			}
		}
		// ブロック拡張
		int off_p = FP_DIFF(p, getDataBuf());	// 保存
		int off_sp = FP_DIFF(sp, getDataBuf());
		newpbn = GrowBlock( fw.pbn, databuf.getBlockNum(), (t_blknum)newblknum, 1 );
		if ( newpbn == BLK_ERROR )
			return -1;
		if ( fw.pbn != newpbn ){
			fw.pbn = newpbn;
		} else {
			newpbn = BLK_ERROR;
		}
		read(fw.pbn);
		p = (_kchar*)(getDataBuf() + off_p);
		sp = (FieldFormat*)(getDataBuf() + off_sp);
	}

	uint8_t *nwp = (uint8_t*)p + oldjn;
	if ( diff ){
		memmove( nwp+diff, nwp, FP_DIFF( sp, nwp ) + sizeof( tfield ) );
		if (diff < 0){
			memset( (uint8_t*)sp+diff, 0, (-diff + sizeof( tfield )) % blocksize );
				// Shrinkするため%blocksizeができる！！
			// ブロック縮小！！！
			if ( newblknum != databuf.getBlockNum() )
				ShrinkBlock( fw.pbn, (t_blknum)newblknum );
		}
	}
	memcpy( p, japa, japalen );
	databuf.GetDataPtr( fw.lp )->fieldlen = (tfield)(nlen + diff);
#ifdef NEWDIC4
	databuf.GetDataPtr( fw.lp )->attr = japa[japalen];
#endif

	if (write(fw.pbn) == -1){
		return -1;
	}
	return newpbn != BLK_ERROR ? -3 : 0;
}
#endif	// !SMALL

#if 0
// 一番最初のフィールドへコピーする
int HPdicData::CopyToTopField( FieldFormat *dp, FieldFormat *src, const _kchar *wbuf )
{
	dp->fieldlen = (tfield)(src->fieldlen + _KLENTOBYTE(src->complen));
	dp->complen = 0;
	memcpy( dp->word, wbuf, _KLENTOBYTE(src->complen) );	// 増加分
	memcpy( dp->word + src->complen, src->word, src->fieldlen );		// 全体
	return 0;
}

int HPdicData::CopyToTopField( FieldFormat2 *dp, FieldFormat2 *src, const _kchar *wbuf )
{
	dp->fieldlen = (tfield2)(src->fieldlen + _KLENTOBYTE(src->complen));
	dp->complen = 0;
	memcpy( dp->word, wbuf, _KLENTOBYTE(src->complen) );	// 増加分
	memcpy( dp->word + src->complen, src->word, src->fieldlen );		// 全体
	return 0;
}
#endif

// NEWDIC3の場合、NEWDIC2と違い、すでに大きいデータが登録されているところへ、大きいデータを登録することができるため、
// 簡単。
// なぜなら、NEWDIC3では大きいデータ(=FieldFormat2)を１つのブロックに収めるようにしているため、
// そのようなことが発生しないからである。
// NEWDIC2では、無理矢理登録しようとするため・・・それならばNEWDIC2もそうしようか？

struct DivList2 {
	int oldloc;		// 先頭からのオフセット
	int newloc;		// byte count
	int complen;	// charactor count
	int flag;		// 1:この位置は新規/更新
	DivList2( int _oldloc, int _newloc, int _complen, int _flag )
		{ oldloc = _oldloc; newloc = _newloc; complen = _complen; flag = _flag; }
};

// 分割だということで、分割後のブロックは小さいとは限らない！！(Growもありえる)
// 注意！！：呼び出し側でIndexには十分な領域があることを確認してから呼ぶこと！！
//           この関数ではIndexに関するエラーチェックを行わない

// 挿入点が無い場合(単なる分割の場合)は、divinfoをNULLにする
//   fw.lpが分割点

// でかいデータ用に最適化済み

#if NOUPDATEWORD
#define	IF_NEW()	// 必ず新規登録
#else
#define	IF_NEW()	if ( fNew )
#endif

int HPdicData::DivBlock( const _kchar *word, const uint8_t *japa, uint japalen, FINDWORD &fw, int fNew, DivInfo *divinfo, PdicIndex *index )
{
	if (!Flush()) return -1;

	FlexObjectArray<DivList2> dl;

	_kchar wbuf[ LWORD + 1 ];
	int insertflag;	// 挿入点フラグ(0=なし,1=前半,2=後半)

	int divoldloc;		// 分割点の位置(挿入前)
	int divnewloc;		// 分割点の位置(挿入後)
	int divcomplen;		// 分割点の単語の圧縮長

	int imaxsize;		// 挿入点がある場合のみ有効！

	int newoffs, oldoffs;
	int wordlen = 0;	// the char. length of the word.

	// recovery 情報
	int dellbn = -1;	// 削除すべきindex
	if (read(fw.pbn)==-1){
		return -1;
	}
	t_blknum oldblknum = databuf.getBlockNum();
	int addsize = 0;

	if ( divinfo ){
		// 最適な分割点を探す
		// ただし、挿入点が最後の場合は無条件に挿入点を分割点に(空きブロックが増えるのを防ぐため)
		const FieldFormat *fp = databuf.GetTopDataPtr();

		oldoffs = GetTopLoc();
		newoffs = GetTopLoc();
		int insertloc = (fw.lp ? fw.lp : GetTopLoc());
		int nextdiff = 0;	// 新規追加の場合、挿入点直後の単語の圧縮長が変わる
		int maxsize = 0;	// 格納率
		while ( fp->fieldlen || newoffs == oldoffs ){
			if ( (newoffs+sizeof(tfield)) % blocksize >= maxsize ){	// 後ろのほうを優先するため=がある
				maxsize = (newoffs+sizeof(tfield)) % blocksize;
				imaxsize = dl.get_num();
			}
			if ( newoffs == insertloc ){
				// 挿入/更新位置
				IF_NEW() {
					dl.add( new DivList2( oldoffs, newoffs, divinfo->complen, 1 ) );
					nextdiff = divinfo->nextdiff;
					newoffs += divinfo->diff;
				}
#if !NOUPDATEWORD
				else {
					dl.add( new DivList2( oldoffs, newoffs, fp->complen, 1 ) );
					oldoffs += fp->fieldlen + L_FieldHeader;
					newoffs += fp->fieldlen + L_FieldHeader + divinfo->diff;
					fp = NextField( fp );
				}
#endif
			} else {
				DivList2 *dv = new DivList2( oldoffs, newoffs, fp->complen, 0 );
//				dl.add( new DivList2( oldoffs, newoffs, fp->complen, 0 ) );
				dl.add( dv );
				oldoffs += fp->fieldlen + L_FieldHeader;
				newoffs += fp->fieldlen + L_FieldHeader - nextdiff;
				nextdiff = 0;
				fp = NextField( fp );
			}
		}

		// ここの時点で
		// oldoffs : 挿入前の終端位置
		// newoffs : 挿入後の終端位置

		// imaxsizeは必ず>=1
		if ( imaxsize == 0 ){
			// 不幸にも先頭の見出語が分割点になった場合
			imaxsize = 1;
		}

		if ( (insertloc > dl[imaxsize].oldloc) || ((insertloc == dl[imaxsize].oldloc) && (!fNew || dl[imaxsize].flag)) ){
			// 新規追加の場合、dl[imaxsize-1]が挿入データであるとinsertloc == oldlocとなってしまい
			// 後ろの挿入点になってしまう
			insertflag = 2;
		} else {
			insertflag = 1;
		}
		divnewloc = dl[imaxsize].newloc;
		divoldloc = dl[imaxsize].oldloc;
		divcomplen = dl[imaxsize].complen;

		// 分割点の直前が挿入点の場合（前半の一番最後に挿入される場合）
		// nextdiffの分だけ後半部分の使用量を増やさないいけない 2016.3.3
		if (imaxsize>0 && dl[imaxsize-1].flag){
			addsize = divinfo->nextdiff;
		}
	} else {
		// 分割点指定の場合
		insertflag = 0;
		divoldloc = divnewloc = fw.lp;
		FieldFormat *fp = databuf.GetDataPtr( fw.lp );
		divcomplen = fp->complen;
		// 終端を探す
		while ( fp->fieldlen ){
			fp = NextField( fp );
		}
		oldoffs = newoffs = FP_DIFF(fp, getDataBuf());
	}

	// imaxsizeの示すDivList2が、分割された後ろのブロックの先頭データ

	// 分割処理 //
	// まず、分割の前ブロックをShrinkし、新しいブロックを割り当て、そのブロックへ直接
	// 分割の後ろブロックを書き込む

	// 分割の前ブロックをshrink
	int presize = divnewloc + sizeof(tfield);
	t_blknum preblknum = (t_blknum)(( presize + blocksize - 1 ) / blocksize);

	// 必要なブロック数を求める
	// 1997/8/23 終端のsizeof(tfield)の扱いが悪かった
	// 終端のサイズをnewsizeに含めたとき、newsizeがblocksizeの整数倍になると、
	// newremainがblocksizeを超えてしまったためおかしくなる
	int newsize = sizeof(t_blknum) + _KLENTOBYTE(divcomplen) + (newoffs - divnewloc) + addsize;
	t_blknum newblknum = (t_blknum)(( (newsize+sizeof(tfield)) + blocksize - 1 ) / blocksize);
	int newremain = newblknum * blocksize - newsize;	// newremainは終端を含めた長さ

	t_pbn2 newprepbn = BLK_ERROR;	// 新規作成されるとBLK_ERROR以外になる

	if ( preblknum < databuf.getBlockNum() ){
		// Shrink
#if 0
		if ( !emptylist ){
			OpenEmptyList();
		}
		// Register the remaining blocks to empty list latter.
		emptylist->Register( fw.pbn + preblknum, (t_blknum)(oldblknum - preblknum) );
#endif
	} else
	if ( preblknum > databuf.getBlockNum() ){
		// Grow
		// 2006.5.25 dataset flagを0→1に変更
		// 今まで正常に動作しているほうがおかしい？？？？
		newprepbn = GrowBlock( fw.pbn, databuf.getBlockNum(), preblknum, 1 );
		if ( newprepbn == BLK_ERROR ){
			return -1;
		}
		read(newprepbn);
	}

	int l;
	FieldFormat *sp;
	FieldFormat ff;
	t_pbn2 newpostpbn = BLK_ERROR;

	// ここからrecovery処理が必要
	//* newprepbnがBLK_ERRORでない場合、newprepbnをemptylistにRegister

	// ブロック数書き換え
	if (file_write( newprepbn != BLK_ERROR ? newprepbn : fw.pbn, &preblknum, sizeof(preblknum) ) == -1 ){
		goto recover8;
	}

	//* newprepbn or fw.pbnのpreblknumをoldblknumに戻す

	// 新しいブロックを割り当てる
	if ( !emptylist ){
		OpenEmptyList();
	}
	newpostpbn = NewBlock( newblknum /*, 0*/ );
	if (newpostpbn==BLK_ERROR){
		goto recover8;
	}
	//* newpostpbnがBLK_ERRORで無い場合emptylistにRegister(newblknum)

	// databufの後半部分をdiskへ //
	if ( file_write( newpostpbn, &newblknum, sizeof(newblknum) ) == -1 ){
		goto recover8;
	}
	//TODO: databufからのアクセスを前提にしているため memory mapped ではold dataがoverwirteされてしまっている
	// emptylist->Register()を遅らせるか、databufを別途用意する、どちらかの方法が必要
	// →とりあえず遅らせる方法で実装してみた 2006.5.8
	sp = (FieldFormat*)(getDataBuf() + divoldloc);

	//* 以下のブロックに対して
	//* dellbnが-1で無い場合、delIndexする
	//* (書き込んだデータはemptylist->Registerであるためrecoveryの必要なし)
	if ( insertflag == 2 )
	{
		// 後半部分に挿入点が含まれている場合
		int fFirst = 1;
		int nextdiff = 0;
		for ( int i=imaxsize;i<dl.get_num();i++ ){
			if ( dl[i].flag ){
				IF_NEW() {
					// 新規
					if ( fFirst ){
						index->addIndex( fw.lbn + 1, newpostpbn, word );
						dellbn = fw.lbn+1;
						// 先頭
						ff.fieldlen = (tfield)(divinfo->diff - L_FieldHeader + _KLENTOBYTE(divinfo->complen));
						ff.complen = 0;
#ifdef NEWDIC4
						ff.attr = japa[japalen];
#endif
						if ( file_write( &ff, L_FieldHeader ) == -1 ){
							goto recover7;
						}
						if ( file_write( word, _kcsbyte1(word) ) == -1 ){
							goto recover7;
						}
					} else {
						// 先頭以外
						ff.fieldlen = (tfield)(divinfo->diff - L_FieldHeader);
						ff.complen = (tcomp)divinfo->complen;
#ifdef NEWDIC4
						ff.attr = japa[japalen];
#endif
						if ( file_write( &ff, L_FieldHeader ) == -1 ){
							goto recover7;
						}
						if ( file_write( word + divinfo->complen, _KLENTOBYTE(_kcslen(word)+1-divinfo->complen) ) == -1 ){
							goto recover7;
						}
					}
					if ( file_write( japa, japalen ) == -1 ){
						goto recover7;
					}
					nextdiff = divinfo->nextdiff;
				}
#if !NOUPDATEWORD
				else {
					// 更新
					if ( fFirst ){
						index->addIndex( fw.lbn + 1, newpostpbn, word );
						dellbn = fw.lbn+1;
						// 先頭
						// 1997.1.2 : fp->complenだった？？？
						ff.fieldlen = (tfield)(sp->fieldlen + divinfo->diff + _KLENTOBYTE(sp->complen));
						ff.complen = 0;
#ifdef NEWDIC4
						ff.attr = japa[japalen];
#endif
						if ( file_write( &ff, L_FieldHeader ) == -1 ){
							goto recover7;
						}
						if ( file_write( word, _kcsbyte1(word) ) == -1 ){
							goto recover7;
						}
					} else {
						// 先頭以外
						ff.fieldlen = (tfield)(sp->fieldlen + divinfo->diff);
						ff.complen = sp->complen;
#ifdef NEWDIC4
						ff.attr = japa[japalen];
#endif
						if ( file_write( &ff, L_FieldHeader ) == -1 ){
							goto recover7;
						}
						if ( file_write( word + sp->complen, _KLENTOBYTE(_kcslen(word)+1-sp->complen) ) == -1 ){
							goto recover7;
						}
					}
					if ( file_write( japa, japalen ) == -1 ){
						goto recover7;
					}
					sp = NextField(sp);
				}
#endif
			} else {
				if ( fFirst ){
					// ここまでdatabufは壊さないこと！！！！
					databuf.GetJustWord( wbuf, dl[i].oldloc );
					index->addIndex( fw.lbn + 1, newpostpbn, wbuf );
					dellbn = fw.lbn+1;
				}
				if ( fFirst && (sp->complen != 0) ){
					ff.fieldlen = (tfield)(sp->fieldlen + _KLENTOBYTE(sp->complen));
					ff.complen = 0;
#ifdef NEWDIC4
					ff.attr = sp->attr;
#endif
					if ( file_write( &ff, L_FieldHeader ) == -1 ){
						goto recover7;
					}
					if ( file_write( wbuf, _kcsbyte(wbuf) ) == -1 ){
						goto recover7;
					}
					int len = _kcslen(sp->word);
					if ( file_write( sp->word + len, sp->fieldlen - _KLENTOBYTE(len) ) == -1 ){
						goto recover7;
					}
				} else {
					if (nextdiff){
						// 直前に挿入/更新され、圧縮長が変わる
						ff.fieldlen = (tfield)(sp->fieldlen - nextdiff);
						ff.complen = sp->complen + _KBYTETOLEN(nextdiff);
#ifdef NEWDIC4
						ff.attr = sp->attr;
#endif
						if ( file_write( &ff, L_FieldHeader ) == -1 ){
							goto recover7;
						}
						const _kchar *word = sp->word + _KBYTETOLEN(nextdiff);
						if ( file_write( word, ff.fieldlen ) == -1 ){
							goto recover7;
						}
						nextdiff = 0;
					} else {
						if ( file_write( sp, sp->fieldlen + L_FieldHeader ) == -1 ){
							goto recover7;
						}
					}
				}
				sp = NextField(sp);
			}
			fFirst = 0;
		}
	} else {
		// 後半に挿入点が無い場合の後半部分の書き出し
		// 先頭の書き込み
		if ( sp->complen != 0 ){
			// ここまでdatabufは壊さないこと！！！！
			databuf.GetJustWord( wbuf, divoldloc );
			index->addIndex( fw.lbn + 1, newpostpbn, wbuf );
			dellbn = fw.lbn+1;
			ff.fieldlen = (tfield)(sp->fieldlen + _KLENTOBYTE(sp->complen));
			ff.complen = 0;
#ifdef NEWDIC4
			ff.attr = sp->attr;
#endif
			if ( file_write( &ff, L_FieldHeader ) == -1 ){
				goto recover7;
			}
			if ( file_write( wbuf, _kcsbyte(wbuf) ) == -1 ){
				goto recover7;
			}
			int len = _kcslen(sp->word);
			if ( file_write( sp->word + len, sp->fieldlen - _KLENTOBYTE(len) ) == -1 ){
				goto recover7;
			}
		} else {
			index->addIndex( fw.lbn + 1, newpostpbn, sp->word );
			dellbn = fw.lbn+1;
			if ( file_write( sp, sp->fieldlen + L_FieldHeader ) == -1 ){
				goto recover7;
			}
		}
		// 残り
		sp = NextField(sp);
		if ( sp->fieldlen ){
			if ( file_write( sp, oldoffs - FP_DIFF(sp,getDataBuf()) ) == -1 ){
				goto recover7;
			}
		}
	}
	// 後半部分・残り書き込み
	if (file_fill(newremain)==-1)
		goto recover7;

#if 1
	//TODO: このregister処理のrecoverが存在しない？？
	if (newprepbn==BLK_ERROR){
		// shrinkであった場合
		if ( !emptylist ){
			OpenEmptyList();
		}
		emptylist->Register(fw.pbn + preblknum, (t_blknum)(oldblknum - preblknum));
	}
#endif
	// 以上後半 //

	// 前半処理 //
	// 前半の残り部分をnullで埋める
	if (!databuf.invalidate()){
		goto recover7;
	}
	if ( newprepbn == BLK_ERROR ){
		if ( seek( fw.pbn, divoldloc ) == -1 ){
			goto recover7;
		}
	} else {
		if ( seek( newprepbn ) == -1 ){
			goto recover7;
		}
		*(t_blknum*)getDataBuf() = (t_blknum)preblknum;
		if ( file_write( getDataBuf(), divoldloc ) == -1 ){		//TODO: 正しいデータが書き込まれるのか？
			goto recover7;
		}
	}

	l = preblknum * blocksize - divoldloc;
	if (file_fill(l)==-1)
		goto recover6;

	//* NULLで埋めた前半の残りの部分を復活させる

	if ( insertflag == 1 ){
		// 前半に挿入点がある場合
		if ( newprepbn != BLK_ERROR ){
			fw.pbn = newprepbn;
		}
		IF_NEW() {
			if ( dl[imaxsize-1].flag ){
				// 一番最後に追加する場合
				fw.fword[0] = '\0';
			}
			if ( addWord( word, wordlen, japa, japalen, fw, NULL ) != 0 ){
				// ここにはこないはず
				goto recover6;
			}
			nword--;	// 後ろの++と帳尻あわせ
		}
#if !NOUPDATEWORD
		else {
			if ( updateWord( japa, japalen, fw, NULL ) != 0 ){
				// ここにはこないはず
				goto recover6;
			}
		}
#endif
		if (!Flush())
			goto recover6;
	}

	if ( newprepbn != BLK_ERROR ){
		index->setIndexBlock( fw.lbn, newprepbn );
	}

	if ( insertflag != 0 ){
		// エラーは起きないはずだから一番最後
		IF_NEW()
		{
			if ( fw.lp <= sizeof(t_blknum) ){
				index->renIndex( fw.lbn, word );
			}
		}

		IF_NEW() nword++;
	}

	return 0;

recover6:

	{
	int _error = error;
	file_write( fw.pbn, getDataBuf(), blocksize * databuf.getBlockNum() );
	error = _error;
	}

recover7:
	if ( dellbn != -1 ){
		index->delIndex( dellbn );
	}

recover8:
	//* newprepbn or fw.pbnのpreblknumをoldblknumに戻す
	file_write( newprepbn != BLK_ERROR ? newprepbn : fw.pbn, &oldblknum, sizeof(t_blknum) );

	if ( newpostpbn != BLK_ERROR ) emptylist->Register( newpostpbn, newblknum );

//recover9:
	if ( newprepbn != BLK_ERROR ) emptylist->Register( newprepbn, preblknum );

	return -1;
}

/*======================================================================*/
/*　後方検索用関数群													*/
/*======================================================================*/
// Field2の場合は、wbufはセットされない！
int TDataBuf::prevWord(uint &loc ,_kchar *wbuf)
{
	if ( loc <= sizeof(t_blknum) )
		return 0;

#ifndef NOFIELD2
	if ( isField2() ){
		FieldFormat2 *p = (FieldFormat2*)GetTopDataPtr();
		FieldFormat2 *tp = (FieldFormat2*)GetDataPtr( loc );
		FieldFormat2 *np;
		while ( 1 ){
//			strcpy( wbuf + p->complen, p->word );
			np = NextField2( p );
			if ( np == tp ){
				loc = FP_DIFF(p, getDataBuf());
				return 1;
			}
			p = np;
		}
	} else
#endif
	{
		FieldFormat *p = GetTopDataPtr();
		FieldFormat *tp = GetDataPtr( loc );
		FieldFormat *np;
		while ( 1 ){
			_kcscpy( wbuf + p->complen, p->word );
			np = NextField( p );
			if ( np == tp ){
				loc = FP_DIFF(p, getDataBuf());
				return 1;
			}
			p = np;
		}
	}
}

// Field1のみ！！
const uint8_t *TDataBuf::srchTail( _kchar *wbuf )
{
	__assert(!isField2());
#if 0
	if ( isField2() ){
		const FieldFormat2 *p = (FieldFormat2*)GetTopDataPtr();
		const FieldFormat2 *np;
		while ( 1 ){
			_kcscpy( wbuf + p->complen, p->word );
			np = NextField2( p );
			if ( !np->fieldlen )
				return (const uint8_t*)p;
			p = np;
		}
	} else
#endif
	{
		const FieldFormat *p = (FieldFormat*)GetTopDataPtr();
		const FieldFormat *np;
		while ( 1 ){
			_kcscpy( wbuf + p->complen, p->word );
			np = NextField( p );
			if ( !np->fieldlen )
				return (const uint8_t *)p;
			p = np;
		}
	}
}

int TDataBuf::isEmpty()
{
#ifndef NOFIELD2
	if ( isField2() ){
		return ((FieldFormat2*)GetTopDataPtr( ))->fieldlen == 0;
	} else
#endif
	{
		return GetTopDataPtr()->fieldlen == 0;
	}
}

// Field1専用
int TDataBuf::GetNumWord( )
{
	__assert(!isField2());
	FieldFormat *p = GetTopDataPtr();
	int n = 0;
	while ( p->fieldlen ){
		n++;
		p = NextField( p );
	}
	return n;
}

// loc位置の完全な単語を得る
// Field1専用関数 //
void TDataBuf::GetJustWord( _kchar *wbuf, int loc )
{
	__assert(!isField2());
	FieldFormat *ff = GetTopDataPtr();
	wbuf[0] = '\0';
	while ( ff->fieldlen && loc >= FP_DIFF(ff, getDataBuf()) ){
		_kcscpy( wbuf + ff->complen, ff->word );
		ff = NextField( ff );
	}
}

#ifndef NOFIELD2
// pbnのデータブロックがField2であるかどうかチェックするだけ
int TDataBuf::CheckField2( t_pbn2 pbn )
{
	t_blknum _blknum = readBlockNum(pbn);
	if (_blknum==(t_blknum)BLK_ERROR)
		return -1;
	return _blknum & FIELDTYPE ? 1 : 0;
}
#endif

