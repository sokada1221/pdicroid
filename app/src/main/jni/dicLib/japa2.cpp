#include "tnlib.h"
#pragma	hdrstop
#include "zen2han.h"
#include "japa.h"
#include "dictext.h"
#ifndef SMALL
#include "zen2han2.h"
#endif

#ifdef SMALL
// 区切り文字指定で付け加える
// zen2hanがtrueのとき、
// j.japaがある場合のみ、j.japaのみzen2hanを行う
// flags : CF_ZEN2HAN, TRS_ALL
// Japa::Merge( j, delim, dic, ... )と同じ動作をする
void Japa::Cat( Japa &j, Pdic *dic, const tchar *delim, int flags )
{
	if ( (flags & TRS_JAPA) && j.japa[0] ){
		if ( japa[0] ){
#ifndef SMALL
			if ( flags & CF_ZEN2HAN ){
				tchar *buf = new tchar[ _tcslen( j.japa ) + 1 ];
				Zen2Han( j.japa, buf );
				if ( japa[0] == '?' && japa[1] == '\0' ){
					japa.SetBuf( buf );
				} else {
					if ( delim[0] )
						japa.cat( delim, buf );
					else
						japa.cat( buf );
					delete buf;
				}
			} else
#endif
            {
				if ( japa[0] == '?' && japa[1] == '\0' ){
					japa = j.japa;
				} else {
					if ( delim[0] )
						japa.cat( delim, j.japa );
					else
						japa.cat( j.japa );
				}
			}
		} else
        	japa.set( j.japa );
    }
	if ( (flags & TRS_EXP) && j.exp[0] ){
		if ( exp[0] )
			exp.cat( StrExpSepa, j.exp );
		else
			exp.set( j.exp );
	}
    if ( (flags & TRS_PRON) && j.pron[0] ){
    	if ( pron[0] )
    		if ( delim[0] )
				pron.cat( delim, j.pron );
    		else
				pron.cat( j.pron );
        else
        	pron.set( j.pron );
    }

	wa_t a = j.attr;
	if ( flags & TRS_ATTR )
		attr |= a & (WA_MEMORY | WA_JEDIT);
	if ( flags & TRS_LEVEL ){
		// 単語レベルはマージ元のレベルを優先？
		if ( attr & WA_LEVELMASK ){
			attr |= (wa_t)( a & ~WA_LEVELMASK );
		} else {
			attr |= a;
		}
	}

#ifdef USE_REF
	// 参照のマージ
	if ( refnum < j.refnum ){
		refnum = j.refnum;
	}
	for ( int i=0;i<refnum;i++ ){
		if ( refdata[i] == -1 )
			refdata[i] = j.refdata[i];	// 未参照の場合のみ変更（で良いかな？）
	}
#endif

#ifdef USE_JLINK
	if ( flags & TRS_OBJECT ){
		// リンクデータのマージ
		// 単純に追加？
		{
		for ( int i=0;i<j.jlinks.get_num();i++ ){
			jlinks.add( j.jlinks[i].Clone( dic ) );
		}
		}
	}
#endif
}

#endif	// SMALL

