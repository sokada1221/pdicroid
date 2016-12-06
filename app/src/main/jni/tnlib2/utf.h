#ifndef __UTF_H
#define	__UTF_H

#include "tndefs.h"

int UTF16toUTF8( const utf16_t *src, int srclen, char *dst, int dstlen=-1 );
int UTF8toUTF16( const char *src, int srclen, utf16_t *dst, int dstlen=-1 );
unsigned long UTF8toUTF16( const char *src, const char **next );

int UTF32toUTF8( const utf32_t *src, int srclen, char *dst, int dstlen=-1 );
int UTF8toUTF32( const char *src, int srclen, utf32_t *dst, int dstlen=-1 );
utf32_t UTF8toUTF32( const char *src, const char **next );

const char *utf8CharNext(const char *s);

int UTF32toUTF16( const utf32_t *src, int srclen, utf16_t *dst, int dstlen );
int UTF16toUTF32( const utf16_t *src, int srclen, utf32_t *dst, int dstlen );

#endif	/* __UTF_H */

