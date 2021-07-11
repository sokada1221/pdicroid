#include "tnlib.h"
#pragma hdrstop
#include "fileio.h"

lsize_t filesize(const tchar *filename)
{
#ifdef WIN32
	HANDLE h = CreateFile( filename, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );
	if (h==INVALID_HANDLE_VALUE)
		return -1;
	LARGE_INTEGER size;
	BOOL ret = GetFileSizeEx(h, &size);
	CloseHandle(h);
	return ret ? size.QuadPart : 0;
#else
	__cstr sfilename(filename);
	struct stat st;
	if (stat(sfilename, &st) < 0){
		return -1;
	}
	return st.st_size;
#endif
}

bool setfilesize(const tchar *filename, lsize_t size)
{
#ifdef WIN32
	HANDLE h = CreateFile( filename, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );
	if (h==INVALID_HANDLE_VALUE)
		return false;

	bool ret = false;
	LONG hi = size>>32;
	if (SetFilePointer((HANDLE)h, (LONG)size, &hi, FILE_BEGIN) == (DWORD)-1){
		if (SetEndOfFile(h)){
			ret = true;
		}
	}

	CloseHandle(h);

	return ret;
#else
	__cstr sfilename(filename);
	return truncate(sfilename, size) == 0;
#endif
}
