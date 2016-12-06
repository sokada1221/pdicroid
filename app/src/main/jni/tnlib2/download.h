#ifndef __download_h
#define	__download_h

bool download_app(const tchar *appname);
HANDLE WinExecEx( const tchar *cmd, int show, const tchar *dir=NULL, const tchar *title=NULL );

#endif
