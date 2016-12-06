#include "tnlib.h"
#pragma	hdrstop
#include "jtype.h"

// strからpatternで始まる単語を削除(for FENG)
// ２つのパターンまで同時対応
void RemoveStr( tchar *str, const tchar *pattern1, const tchar *pattern2, BOOL fKanaEnd )
{
	tchar *sp = str;
	ushort c;
	while ( 1 ){	// 複数対応
		tchar *lp2;
		tchar *lp = _tcsstr( sp, pattern1 );
		if ( !lp ){
			if ( pattern2 ){
				lp = _tcsstr( sp, pattern2 );
			}
			if ( !lp )
				break;
			lp2 = lp + _tcslen(pattern2);
		} else {
			lp2 = lp + _tcslen(pattern1);
		}

		while ( 1 ){
			tchar *_lp2 = lp2;
			LD_CHAR( c, lp2 );
			if ( IS_ENDCHAR(c) ){
				lp2 = _lp2;
				break;
			}
			if ( isdelim( c ) ){
				if ( fKanaEnd ){
					// 次がカタカナでなければ終了
					_lp2 = lp2;
					LD_CHAR( c, _lp2 );
					if ( !iskatakana(c) ){
						break;
					}
				} else {
					break;
				}
			}
			if ( islbr2( c ) && c != CODE_LBR6 ){
				// 左括弧は強制終了
				lp2 = _lp2;
				break;
			}
		}
		memmove( lp, lp2, LENTOBYTE(_tcslen( lp2 )+1) );
		sp = lp + STR_DIFF(lp2,lp);
	}
}
