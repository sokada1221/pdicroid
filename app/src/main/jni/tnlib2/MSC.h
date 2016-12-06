#ifndef __MSC_H
#define	__MSC_H

// Compatible definitions for MS-C/C++

#define	CT_KJ1	_MBC_LEAD
#define	CT_KJ2	_MBC_TRAIL

#define	nthctype(x,y)	_mbsbtype((const unsigned char*)x, y)

#define	iskanji(x)	_ismbblead(x)

#endif	__MSC_H
