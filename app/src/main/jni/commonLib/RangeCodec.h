#ifndef __RANGE_CODEC_H
#define	__RANGE_CODEC_H

//int rcCompress( const unsigned char *sp, int sp_len, char *dp );
//int rcDecode( const char *sp, char **sp_next, char *dp );

struct RC_HEADER {
	unsigned char size;
	unsigned char reserved[3];
	char method[4];	// 'rc1'
	unsigned long compsize;
	unsigned long orgsize;
	unsigned long blocksize;	// 0のときは、blocksize=orgsize
};

int RCDecode( const byte *src, long srclen, byte *dest, long &destlen );
int RCEncode( const byte *src, long srclen, byte *dest, long &destlen );
unsigned long RCGetOrgSize( const byte *header );
int RCGetErrorCode( );
#ifdef _WINDOWS
void RCSetWindow( HWND hwnd );
#endif

// オリジナルサイズの取得（拡張ヘッダーサイズは含まない）
inline unsigned long RCGetOrgSize( const byte *header )
{
	return ((const RC_HEADER *)header)->orgsize;
}

#endif	/* __RANGE_CODEC_H */
