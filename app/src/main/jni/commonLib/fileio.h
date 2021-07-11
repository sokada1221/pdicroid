#ifndef __fileio_h
#define	__fileio_h

typedef __int64 lsize_t;

lsize_t filesize(const tchar *filename);
bool setfilesize(const tchar *filename, lsize_t size);


#endif	// __fileio_h

