#ifndef __ALTMBSTR_H
#define	__ALTMBSTR_H

inline bool _ismbblead( unsigned char c )
{
	return (c>=0x80&&c<=0x9F) || (c>=0xE0 /*&&c<=0xFF*/);
}


#endif
