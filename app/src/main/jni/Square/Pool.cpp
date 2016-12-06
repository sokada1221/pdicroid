#include "tnlib.h"
#pragma	hdrstop
#include "dic.h"
#ifdef EPWING
#include "pdepwing.h"
#endif
#include "pool.h"

//#ifdef GUI
TPoolItem::TPoolItem(Japa *_japa, int _fdn, int _level)
	:japa(_japa)
	,fdn(_fdn)
#if  USE_DISPLEVEL
	,level(_level)
#endif
{
	__assert(japa);
#if USE_DT2
	ExtractStaticWords( &hl, japa );
#endif
}

TPoolItem::~TPoolItem()
{
	if (japa)
		delete japa;
}


#if USE_DT2
int TPoolItem::ExtractStaticWords( THyperLinks *hls, const Japa *japa )
{
	int n = hls->ExtractStaticWords( HLI_PRON, japa->pron );
	n += hls->ExtractStaticWords( HLI_JAPA, japa->japa );
	n += hls->ExtractStaticWords( HLI_EXP, japa->exp );
#ifdef EPWING
	if ( depwif._IsValid() ){
//		depwif.fAutoLinkSearch = AutoLinkConfig.fEnable;
		n += japa->jlinks.SearchHyperLink( *hls );
//		depwif.fAutoLinkSearch = false;
	}
#endif
	return n;
}
#endif	// USE_DT2

void TPoolItem::UpdateHyperLink( )
{
#if USE_DT2
	hl.clear();
	ExtractStaticWords(&hl, japa);
#endif
}

Pool::Pool( )
	:PrimaryPool( MAX_NDISP )
	,Items(MAX_NDISP)
{
}

Pool::~Pool()
{
}

// keywordのみで比較する
// 厳密な位置にはならないので注意
// strict version -> BSearchExact
int Pool::BSearch( const tchar *str )
{
	int left = 0;
	int right = get_num();
	if ( right == 0 ) return 0;
#if MIXDIC || defined(KMIXDIC)
	_kwstrdef( _str, str );
#else
	#define	_str	str
#endif
	do {
		int mid = ( left + right ) /2;
		int k = comp_word( _str, (const _kchar*)_kwstr(fw[mid], GetKCodeTrans()) );
		if (k == 0){
			return mid;
		}
		if (k < 0){
			right = mid;
		} else {
			left = mid + 1;
		}
	} while (left < right);
	return left;
}

void Pool::Clear( )
{
	fw.clear();
	Items.clear();
#if 0
	fj.clear();
#if USE_DT2
	hl.clear();
#endif
	fdn.clear();
	levels.clear();
#endif
}
void Pool::Add( const tchar *word, const Japa *japa, int dicno, int level )
{
	AddDirect( new tnstr(word), new Japa(*(Japa*)japa), dicno, level);
}

// word, japaの所有権が移るので注意
void Pool::AddDirect( tnstr *word, Japa *japa, int dicno, int level )
{
	fw.add( word );
#if 1
	Items.add(new TPoolItem(japa, dicno, level));
#else
	fj.add( japa );
	fdn.add( dicno );
	levels.add(level);
#if USE_DT2
	THyperLinks *_hl = new THyperLinks;
	ExtractStaticWords( _hl, japa );
	hl.add( _hl );
#endif
#endif
}

void Pool::Insert( int i, const tchar *word, const Japa *japa, int dicno, int level )
{
	InsertDirect(i, new tnstr(word), new Japa(*(Japa*)japa), dicno, level);
}

// The owner of the word and the japaが移るので注意
void Pool::InsertDirect( int i, tnstr *word, Japa *japa, int dicno, int level )
{
	fw.insert( i, word );		// オフセットは要るかなぁ・・・
#if 1
	Items.insert(i, new TPoolItem(japa, dicno, level));
#else
	fj.insert( i, japa );
	fdn.insert( i, dicno );
	levels.insert(i, level);
#if USE_DT2
	THyperLinks *_hl = new THyperLinks;
	ExtractStaticWords( _hl, japa );
	hl.insert( i, _hl );
#endif
#endif
}

#if USE_DT2
void Pool::UpdateHyperLink( int i )
{
#if 1
	Items[i].UpdateHyperLink();
#else
	hl[i].clear();
	ExtractStaticWords( &hl[i], &fj[i] );
#endif
}
#endif
void Pool::Del( int i )
{
	fw.del( i );
#if 1
	Items.del(i);
#else
	fj.del( i );
	fdn.del( i );
	levels.del(i);
#if USE_DT2
	hl.del( i );
#endif
#endif
}
void Pool::Del(int si, int ei)
{
	fw.del( si, ei );
#if 1
	Items.del(si, ei);
#else
	fj.del( si, ei );
	fdn.del( si, ei );
	levels.del(si, ei);
#if USE_DT2
	hl.del( si, ei );
#endif
#endif
}
//#if !NEWINCSRCH
// bi から ei-1 を残して後はカット！
void Pool::Keep( int bi, int ei )
{
	if ( bi >= get_num() )
		return;
	if ( bi > ei ){
		Clear();
		return;
	}
	if ( ei < get_num() ){
		// 最後に残りがあれば削除
		Del(ei, get_num()-1);
	}
	if ( bi ){
		// 最初の部分を削除
		Del(0, bi-1);
	}
}
//#endif

#if 0
// strではじまる文字列をキープ
bool Pool::Keep( const tchar *str, int len, int &_i1 )
{
	tchar *p = new tchar[ len + 1 ];
	memcpy( p, str, len + 1);
	p[ len - 1 ]++;
	p[ len ] = '\0';
	int i1 = BSearch( str );
	int i2 = BSearch( p );
	delete p;
	// 一気に全部消えてしまう場合は、何もしない
	if ( i1 == i2 )
		return false;
	Keep( i1, i2 );
	_i1 = i1;
	return true;
}
#endif

//Note:
// strはkeyword部分のみを渡す
void Pool::GetKeepLocK( const tchar *str, int len, int &i1, int &i2 )
{
	tchar *p = new tchar[ len + 2 ];
	memcpy( p, str, (uint)LENTOBYTE(len + 1) );
	p[ len ] = 0xff;
	p[ len+1 ] = '\0';
	i2 = BSearch( p );
	p[ len-1 ]--;
	p[ len ] = 0xff;
	//p[ len+1 ] = '\0';
	i1 = BSearch( p );
	delete[] p;
}

#if 0	// 使用しなくなったのでcomment out
//Note:
// strはcomposit word
void Pool::GetKeepLocC( const tchar *str, int len, int &i1, int &i2 )
{
	tchar *p = new tchar[ len + 2 ];
	memcpy( p, str, (uint)LENTOBYTE(len + 1) );
	i1 = BSearchExact( p );
	p[ len ] = 0xff;
	p[ len+1 ] = '\0';
	i2 = BSearchExact( p );
	delete[] p;
}
#endif

#if 0
// strがfw内に含まれるか？
bool Pool::Included( const tchar *str, int len, bool ignorecase )
{
	if ( ignorecase ){
		return jstrnicmp( str, fw[0], len ) >= 0 && jstrnicmp( str, fw[ get_num() - 1 ], len ) <= 0;
	} else {
		return strncmp( str, fw[0], len ) >= 0 && strncmp( str, fw[ get_num() - 1 ], len ) <= 0;
	}
}
#endif

#if USE_DT2
int Pool::ExtractStaticWords( THyperLinks *hls, const Japa *japa )
{
	int n = hls->ExtractStaticWords( HLI_PRON, japa->pron );
	n += hls->ExtractStaticWords( HLI_JAPA, japa->japa );
	n += hls->ExtractStaticWords( HLI_EXP, japa->exp );
#ifdef EPWING
	if ( depwif._IsValid() ){
//		depwif.fAutoLinkSearch = AutoLinkConfig.fEnable;
		n += japa->jlinks.SearchHyperLink( *hls );
//		depwif.fAutoLinkSearch = false;
	}
#endif
	return n;
}
#endif	// USE_DT2

const tchar *Pool::GetText( int no, int sqmno )
{
	switch ( sqmno ){
		case SQM_WORD:
			return fw[no];
#if 1
		case SQM_PRON:
			return Items[no].japa->pron;
		case SQM_JAPA:
			return Items[no].japa->japa;
		case SQM_EXP:
			return Items[no].japa->exp;
#else
		case SQM_PRON:
			return fj[no].pron;
		case SQM_JAPA:
			return fj[no].japa;
		case SQM_EXP:
			return fj[no].exp;
#endif
	}
	return NULL;
}
//#endif	// GUI

PoolView::PoolView(IPoolViewer *viewer)
	:Viewer(viewer)
{
}
PoolView::~PoolView()
{
	if (Viewer) delete Viewer;
}
void PoolView::Clear()
{
	super::Clear();
	Viewer->Clear();
}
void PoolView::AddDirect( tnstr *word, Japa *japa, int dicno, int level )
{
	super::AddDirect(word, japa, dicno, level);
	Viewer->Add(word, japa, dicno, level);
}
void PoolView::InsertDirect( int i, tnstr *word, Japa *japa, int dicno, int level )
{
	super::InsertDirect(i, word, japa, dicno, level);
	Viewer->Insert(i, word, japa, dicno, level);
}
void PoolView::Del( int i )
{
	super::Del(i);
	Viewer->Del(i);
}
void PoolView::Del(int si, int ei)
{
	super::Del(si, ei);
	Viewer->Del(si, ei);
}
