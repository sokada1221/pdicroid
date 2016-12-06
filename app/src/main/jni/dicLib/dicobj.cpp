#include	"tnlib.h"
#pragma	hdrstop
#include "dic.h"
#if defined(DOS)
#include	"env.h"
#endif
#include	"faststr.h"
#ifdef NEWDIC2
#include "dicorder.h"
#endif

PdicObject::PdicObject( int &_error )
	:error( _error )
{
	error = 0;
}

#if defined(USE_DICORDER)
int cmpword( const char *str1, const char *str2, int order )
{
	switch ( order ){
		case SK_NORMAL:
			return strcmp( str1, str2 );
		case SK_IGNCASE:
			return stricmp( str1, str2 );
		case SK_DICORDER:
			return cmpnword( str1, str2, max( strlen(str1), strlen(str2) ), order );
		case SK_DESCENDING:
			{
				while ( *str1 == *str2 && *str1 != '\0' ){
					str1++;
					str2++;
				}
				if ( *str1 == '\0' ){
					if ( *str2 == '\0' )
						return 0;
					return -1;
				}
				if ( *str2 == '\0' ){
					return 1;			// でいいかな？（論理的に整然としているから多分大丈夫？）
				}
				return *str2 - *str1;
			}
//			return strcmp( str2, str1 );
	}
	return 0;
}

#ifdef DOS
int _fcmpword( const char far *str1, const char far *str2, int order )
{
	switch ( order ){
		case SK_NORMAL:
			return _fstrcmp( str1, str2 );
		case SK_IGNCASE:
			return _fstricmp( str1, str2 );
		case SK_DICORDER:;
	}
	return 0;
}
#endif

int cmpnword( const char *str1, const char *str2, int n, int order )
{
	switch ( order ){
		case SK_NORMAL:
			return strncmp( str1, str2, n );
		case SK_IGNCASE:
//			return strnicmp( str1, str2, n );
			return strnicmp( str1, str2, n );
		case SK_DICORDER:
			{
				// 辞書順規則
				// 無視するコード:記号、半角スペース
				while ( n ){
					uchar c1 = (uchar)*str1;
					if ( c1 ){
						c1 = DicOrderConv( c1 );
						if ( !c1 ){
							n--;
							str1++;
							continue;
						}
						str1++;
					}
					uchar c2;
					while ( (c2 = *str2) != 0 ){
						c2 = DicOrderConv( c2 );
						if ( !c2 ){
							str2++;
							continue;
						}
						str2++;
						break;
					}
					char r = (char)(c1 - c2);
					if ( r )
						return r < 0 ? -1 : 1;
					n--;
				}
				return 0;
			}
		case SK_DESCENDING:
			{
				while ( *str1 == *str2 && *str1 != '\0' && n ){
					str1++;
					str2++;
					n--;
				}
				if ( !n )
					return 0;
				if ( *str1 == '\0' ){
					if ( *str2 == '\0' )
						return 0;
					return -1;
				}
				if ( *str2 == '\0' ){
					return 1;
				}
				return *str2 - *str1;
			}
//			return strncmp( str2, str1, n );	// でいいかな？
	}
	return 0;
}
#endif


// コード順以外の処理について
// コード順以外は２種類の文字列比較が必要
// 辞書順を例に取る
// 検索時には  AA と A-A は同じ文字列とみなす必要がある。さらに同じ文字列は複数存在することを考える必要がある
// 登録時には  AA と A-A は異なる文字列である
// ※登録は検索を含む
//
// ・フロー
//
// 検索：辞書順で比較
// 登録：検索に一致した文字列に対し、辞書順の第２レベルで挿入点を探す
//       挿入点が同一単語であるかどうかチェックする
//
// ・テーブルによって実現する場合
// 第１レベルでの順番と第２レベルでの順番を用意する必要がある
// この場合、第２レベルはすべてのコードについて同じ順位のものは存在しない
//           第１レベルは第２レベルの順番に準拠し、同一順位のものが存在する
//
// 問題点：２バイト文字の対処方法
//         同様にテーブルを用意したいが、膨大であるためあまり適当でない。
//         ２バイト文字の場合はテーブル構造を変え、変換規則を指定し、それ以外はコード順に準拠するようにする
//         変換規則は第１レベル、第２レベルとも両方用意する必要がある。



