//---------------------------------------------------------------------------
#include <windows.h>
#pragma hdrstop

#include "mmf.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

void *TMmfMap::map(unsigned offset, unsigned size, bool readonly)
{
	hMem = CreateFileMapping(Handle, NULL, readonly?PAGE_READONLY:PAGE_READWRITE, 0, offset+size, NULL);
	if (!hMem){
		GetLastError();
		return NULL;
	}
	TopAddr = MapViewOfFile(hMem, readonly?FILE_MAP_READ:FILE_MAP_ALL_ACCESS, 0, 0, offset+size);
	if (!TopAddr){
		GetLastError();
		CloseHandle(hMem);
		hMem = NULL;
		return NULL;
	}
	Buffer = (char*)TopAddr + offset;
	Size = size;
	return Buffer;
}
void TMmfMap::unmap()
{
	if (!hMem)
		return;
	UnmapViewOfFile(Buffer);
	CloseHandle(hMem);
	Buffer = NULL;
	hMem = NULL;
	Size = 0;
}
void *TMmfMap::changeOffset(unsigned offset)
{
	if (!hMem)
		return NULL;
	int diff = offset - ((int)Buffer - (int)TopAddr);
	if (diff>Size)
		return NULL;
	Buffer = (char*)Buffer + diff;
	Size -= diff;
	return Buffer;
}
bool TMmfFile::create(const TCHAR *filename)
{
	Handle = CreateFile(filename, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (Handle == INVALID_HANDLE_VALUE)
		return false;
	return true;
}
void TMmfFile::close()
{
	unmap();
	if (Handle!=INVALID_HANDLE_VALUE){
		CloseHandle(Handle);
		Handle = INVALID_HANDLE_VALUE;
	}
}

