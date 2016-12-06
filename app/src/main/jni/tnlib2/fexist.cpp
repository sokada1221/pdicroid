#include "stdafx.h"
#ifdef	_WINDOWS
#include	<windows.h>
#endif
#include	"bc.h"
#pragma	hdrstop
#include "file.h"
#include	"fexist.h"
#include "uniansi.h"
#ifdef UNIX
#include <unistd.h>
#endif

#ifdef	_WINDOWS
bool fexist( const TCHAR *filename )
{
#if defined(WIN32) && !defined(UNIANSI)
#if 1
	return ::GetFileAttributes(filename) != 0xFFFFFFFF;
#else
	HANDLE h = CreateFile( filename, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( h != INVALID_HANDLE_VALUE ){
		CloseHandle( h );
		return true;
	}
	return false;
#endif
#else
	OFSTRUCT of;
	memset( &of, 0, sizeof( OFSTRUCT ) );
	of.cBytes = sizeof( OFSTRUCT );
	ConvertUniToAnsi( CP_ACP, filename, _filename, MAXPATH );
	return OpenFile( _filename, &of, OF_EXIST ) != HFILE_ERROR;
#endif
}
#else
bool fexist( const tchar *filename )
{
	int fd = _topen( filename, O_RDONLY );
	if ( fd == -1 )
		return false;
	close( fd );
	return true;
}
#endif

