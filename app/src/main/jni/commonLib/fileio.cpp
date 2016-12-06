#include "tnlib.h"
#pragma hdrstop
#include "fileio.h"

lsize_t filesize(const tchar *filename)
{
	HANDLE h = CreateFile( filename, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );
	if (h==INVALID_HANDLE_VALUE)
		return -1;
	LARGE_INTEGER size;
	BOOL ret = GetFileSizeEx(h, &size);
	CloseHandle(h);
	return ret ? size.QuadPart : 0;
}

