#ifndef __unix_h
#define	__unix_h

unsigned GetTickCount();
void Sleep(int msec);

int strnicmp( const char *s1, const char *s2, int len );

void DBW( const char *format, ... );
#define	dbw	DBW

#endif
