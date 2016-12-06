//
// for WinCE
//
#include "tnlib.h"
#pragma hdrstop
#include "file.h"

int _tcreat( const tchar *filename, int mode )
{
	HANDLE h = CreateFile( filename, mode & O_WRONLY ? GENERIC_WRITE : GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( h == INVALID_HANDLE_VALUE ){
		return -1;
	}
	SetEndOfFile( h );
	return (int)h;
}
int _topen( const tchar *filename, int mode )
{
	if ( mode & O_TRUNC ){
		return _tcreat( filename, mode );
	}
	HANDLE h = CreateFile( filename, mode & O_WRONLY ? GENERIC_WRITE : GENERIC_READ,
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( h == INVALID_HANDLE_VALUE ){
		if ( mode & O_CREAT ){
			return _tcreat( filename, mode );
		}
		return -1;
	}
	if ( mode & O_WRONLY ){
		// write -> append mode
		SetFilePointer( h, 0, NULL, FILE_END );
	}
	return (int)h;
}
int _twrite( int handle, const void *buf, unsigned bytes )
{
	DWORD byteswritten = 0;
	if ( WriteFile( (HANDLE)handle, buf, bytes, &byteswritten, NULL ) ){
		return byteswritten;
	}
	return -1;
}
int _tread(int handle, void *buf, unsigned bytes )
{
	DWORD bytesread = 0;
	if ( ReadFile( (HANDLE)handle, buf, bytes, &bytesread, NULL ) ){
		return bytesread;
	}
	return -1;
}

