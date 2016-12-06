#include	"tnlib.h"
#pragma	hdrstop
#include "pdconfig.h"
#include "dic.h"
#include "multidef.h"
#include "multiwd.h"
#include "japa.h"
#include "jmerge.h"

MultiWord::MultiWord(int _arraynum)
	:warray( _arraynum )
	,jarray( _arraynum )
{
	for ( int i=0;i<_arraynum;i++ ){
		warray.add( new TCharSingle );
		jarray.add( (Japa *)new Japa );
	}
	samect = 0;
	dirflag = 0;
	Mode = MD_NOTMERGE;
}

MultiWord::~MultiWord()
{
}

void MultiWord::set( int i, const _mwkchar *word, Japa &japa )
{
	warray[i] = word;
	jarray[i] = japa;
}

#if 0
void MultiWord::reverse( void )
{
	dirflag ^= 1;
}
#endif

//返り値：Mode = MD_PARA : 辞書番号 Mode = MD_MERGE : 重複単語数
//
//	strがNULLでない場合は、strと同じかそれより前の単語の中で一番後ろの単語を返す
//	str以前の単語がなければ、最も若い単語を返す
int MultiWord::get( _mwkstr_t &word, Japa *japa, int *srchflag, const _mwkchar *str)
{
	__assert(word.empty());
	//word.clear();
	if (japa) japa->clear();
	if ( !(Mode&MD_PARA) || samect == 0 ){
		samect = get_sameword( srchflag, minstr, str );
		if ( samect == 0 ){
			return 0;
		}
	}
	if ( Mode & MD_PARA )
		return get_fromarray( word, japa, srchflag );
	word.set( warray[ minstr ].c_str() );

	//一致する単語を調べる
	srchflag[ minstr ] = 1;
	if (japa) *japa = jarray[ minstr ];
	for ( int i=minstr+1;i<jarray.get_num();i++ ){
		if ( srchflag[i] == 2 ){
			//日本語訳をマージする
			srchflag[i] = 1;
#ifdef SMALL
			if (Japa) japa->Cat( jarray[i], NULL, _T(","), TRS_ALL );	// 日本語訳マージしない
#else	// !SMALL
#ifdef _WINDOWS
			if ( Mode & MD_NOTMERGE ){
//				if (japa) japa->Cat( jarray[i], NULL, _T("\n"), TRS_ALL );	// 日本語訳マージしない
				if (japa) japa->Merge( jarray[i], _T("\n"), NULL, TRS_ALL );
			} else
				if (japa) japa->Merge( jarray[i], NULL, NULL, TRS_ALL );
#else	// !_Windows
			if (japa) japa->Merge( jarray[i], NULL, NULL, TRS_ALL );
#endif	// !_Windows
#endif	// !SMALL
		}
	}
	return samect;
}

//warray,jarrayからsrchflagで2になっている単語を順次取り出す
//Mode=MD_PARA専用
// dirflag = 1のときはwarrayのインデックスが大きい方から探す
int MultiWord::get_fromarray( _mwkstr_t &word, Japa *japa, int *srchflag )
{
	if ( samect == 0 ){
		return -1;
	}
	int i = 0;
	if ( dirflag ){
		i = warray.get_num() - 1;
	}
	while ( 1 ){
		if ( srchflag[i] == 2 ){
			srchflag[i] = 1;
			samect--;
			word.set( warray[i].c_str() );

			if (japa) *japa = jarray[ i ];
			return i;
		}
		if ( dirflag ){
			i--;
			if ( i >= 0 )
				continue;
		} else {
			i++;
			if ( i < warray.get_num() )
				continue;
		}
		break;
	}
	samect = 0;
	return -1;
}

//見つかった単語で最も若い見出語を探し、その同一の見出語の個数及びテーブルを返す
//minstrは最も若い見出語の中の１つのインデックス
// dirflag = 1のときは
// 見つかった単語で最も古い見出語を探し、その同一の見出語の個数及びテーブルを返す
// minstrは最も古い見出語の中の１つのインデックス
//
//	strがNULLでない場合は、strと同じかそれより前の単語の中で一番後ろの単語を返す
//	str以前の単語がなければ、最も若い単語を返す
int MultiWord::get_sameword( int *srchflag, int &minstr, const _mwkchar *str )
{
	//最も若い／古い見出語を探す
	minstr = -1;
	int maxstr = -1;
	int i;
	for ( i=0;i<warray.get_num();i++ ){
		if ( warray[i].exist() && srchflag[i] != -1 ){
			if (str && cmpword(warray[i],str)<=0){
				// str以前の単語の中で一番後ろのものを探す
				if (maxstr==-1){
					maxstr = i;
					continue;
				}
				int r = cmpword( warray[maxstr], warray[i] );
				if ( dirflag == 0 ){
					if ( r < 0 )
						maxstr = i;
				} else {
					__assert__;	// not supported yet.
					if ( r > 0 ){
						maxstr = i;
					}
				}
			} else {
				if ( minstr == -1 ){
					minstr = i;
					continue;
				}
				int r = cmpword( warray[minstr], warray[i] );
				if ( dirflag == 0 ){
					if ( r > 0 )
						minstr = i;
				} else {
					if ( r < 0 ){
						minstr = i;
					}
				}
			}
		}
	}
	if (maxstr!=-1){
		minstr = maxstr;	// maxstrがstrに最も近い
	} else
	if ( minstr == -1 )
		return 0;
	srchflag[minstr] = 2;
	int ct = 1;
	//一致する単語を調べる
	for ( i=0;i<warray.get_num();i++ ){
		if ( i == minstr )
			continue;
		if ( cmpword( warray[i], warray[minstr] ) == 0 ){
			srchflag[i] = 2;
			ct++;
		}
	}
	return ct;
}

void MultiWord::clear( int dicnum )
{
	if (warray.get_num()!=dicnum){
		warray.clear();
		jarray.clear();

		for ( int i=0;i<dicnum;i++ ){
			warray.add( new TCharSingle );
			jarray.add( (Japa *)new Japa );
		}
	} else {
		for ( int i=0;i<dicnum;i++ ){
			warray[i].clear();
			jarray[i].clear();
		}
	}

	samect = 0;
	dirflag = 0;	// 2008.5.31 added.
}

