#include "tnlib.h"
#pragma	hdrstop
#include "pdfunc.h"

// strをcmdから探し、そのインデックスを返す
// strはNULLで終わっている必要はなく、英数字以外で終わっていれば良い
// cmdはスペースで区切られたコマンド文字列群
int GetStrIndex( const tchar *str, const tchar *cmd, int defval, int *subinx )
{
	const tchar *p = str;
	while ( *p ){
		if ( !isalpha( (tuchar)*p ) ){
			break;
		}
		p = CharNext( p );
	}
	int len = STR_DIFF( p, str );
	int i = 0;
	while ( *cmd ){
		if ( (tuchar)*cmd == (tuchar)*str && (tuchar)cmd[len] <= ' ' ){
			if ( !_tcsnicmp( cmd, str, len ) ){
				if ( subinx ){
					if ( str[ len ] >= '0' && str[ len ] <= '9' ){
						*subinx = str[ len ] - '0';
					} else {
                    	*subinx = 0;
					}
				}
				return i;
			}
		}
		// 次のコマンドへ
		i++;
		while ( *cmd ){
			if ( (tuchar)*cmd <= ' ' ){
				if ( *cmd )
					cmd++;
				break;
			}
			cmd++;
//			cmd = CharNext( cmd );
		}
	}
	return defval;
}


