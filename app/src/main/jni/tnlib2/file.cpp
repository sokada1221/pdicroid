#include "stdafx.h"
#include	"tnlib.h"
#pragma	hdrstop
#include "file.h"
#include <fcntl.h>
#ifdef UNIX
#include <unistd.h>
#endif

File::File()
{
	fd = -1;
	text = 1;
	filename = 0;
	err = 0;
	cc = NULL;
}

File::~File()
{
	close();
}


void File::close( void )
{
	if ( fd != -1 ){
		_tclose( fd );
		fd = -1;
	}
}

#ifdef _Windows
TCharCodeStreamW32::TCharCodeStreamW32(HANDLE _handle)
	:Handle(_handle)
{}
int TCharCodeStreamW32::read(void *Buffer, int Count)
{
	DWORD nbytes;
	if (ReadFile(Handle, Buffer, Count, &nbytes, NULL)){
		return nbytes;
	}
	return -1;
}
int TCharCodeStreamW32::write(const void *Buffer, int Count)
{
	DWORD nbytes;
	if (WriteFile(Handle, Buffer, Count, &nbytes, NULL)){
		return nbytes;
	}
	return -1;
}
__off_t TCharCodeStreamW32::seek(__off_t Offset, int Origin)
{
	LONG high = (LONG)(Offset>>32);
	DWORD ret = SetFilePointer(Handle, (LONG)Offset, &high, Origin);
	if (ret==0xFFFFFFFFu){
		//err = GetLastError();
		return false;
	}
	return ((__off_t)high<<32) | ret;
}
__off_t TCharCodeStreamW32::tell()
{
	LONG high = 0;
	DWORD low = SetFilePointer(Handle, 0, &high, FILE_CURRENT);
	if (low==0xFFFFFFFFu){
		int err = GetLastError();
		if (err!=NO_ERROR){
			return -1;
		}
	}
	return ((__int64)high<<32) | low;
}
#endif

__int64 FileTime(const tchar *filename)
{
#ifdef _Windows
	FILETIME ft;
	memset(&ft, 0, sizeof(ft));
	HANDLE h = CreateFile( filename, GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
	if (h!=INVALID_HANDLE_VALUE){
		GetFileTime(h, NULL, NULL, &ft);
		CloseHandle(h);
		return ((__int64)ft.dwHighDateTime<<32) | ft.dwLowDateTime;
	}
	return 0;
#else
	struct stat st;
	__cstr sfilename(filename);
	if (stat(sfilename, &st)!=0) return 0;
	return st.st_mtime;
#endif
}

bool is_dir(const tchar *path)
{
#if _Windows
	DWORD dwAttr = ::GetFileAttributes(path);
	if (dwAttr == 0xFFFFFFFFu)
		return false;	// error
	return (dwAttr & FILE_ATTRIBUTE_DIRECTORY) ? true : false;
#else
	struct stat st;
	__cstr spath(path);
	if (stat(spath, &st)!=0) return false;	// error
	return (st.st_mode & S_IFDIR) == S_IFDIR;
#endif
}
