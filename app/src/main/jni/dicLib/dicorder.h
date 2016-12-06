#ifndef __DICORDER_H
#define	__DICORDER_H

// 無視するコードの場合は 0 を返す
// それ以外はコードをそのまま返す（ただし、英字小文字は大文字に変換）
inline uchar DicOrderConv( uchar c )
{
	if ( c >= 'a' && c <= 'z' )
		return (uchar)(c & 0xdf);
	if ( ( c >= 'A' && c <= 'Z' ) || ( c >= 0x80 ) || ( c >= '0' && c <= '9' ) )
		return c;
	return 0;
}

#endif	// __DICORDER_H
