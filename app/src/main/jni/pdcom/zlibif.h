#ifndef __zlibif_h
#define	__zlibif_h

struct ZLIB_HEADER {
	unsigned char size;
	unsigned char reserved[3];
	char method[4];	// 'zl1'
	unsigned long compsize;
	unsigned long orgsize;
	unsigned long blocksize;	// 0のときは、blocksize=orgsize
};

int zlibEncode( const byte *src, long srclen, byte *dest, long &destlen );
int zlibDecode( const byte *src, long srclen, byte *dest, long &destlen );
unsigned long zlibGetOrgSize( const byte *header );
int zlibGetErrorCode( );
#ifdef _WINDOWS
void zlibSetWindow( HWND hwnd );
#endif

// オリジナルサイズの取得（拡張ヘッダーサイズは含まない）
inline unsigned long zlibGetOrgSize( const byte *header )
{
	return ((const ZLIB_HEADER *)header)->orgsize;
}


#endif	/* __zlibif_h */
