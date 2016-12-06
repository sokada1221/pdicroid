#ifndef __ktable_h
#define	__ktable_h

unsigned long encodeKT(unsigned long c);
unsigned long decodeKT(unsigned long c);
void encodeKT(const tchar *s, tchar *d);
void decodeKT(const tchar *s, tchar *d);

#endif	/* __ktable_h */
