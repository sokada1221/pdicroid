#include "tnlib.h"
#pragma hdrstop
#include "zipDll.h"
#include "filestr.h"

//Comment:
// Use the error codes defined in LiteZip.h/LiteUnzip.h.
// And use the other error codes defined in zipDll.h.

#if 0
// pathからファイル名の先頭のアドレスを得る
tchar *GetFileName( const tchar *path )
{
	const tchar *p;
	const tchar *delim1 = NULL;
	const tchar *delim2 = NULL;

	for ( p=path; *p ; ){
		tchar c = *p;
		if ( c == '\\' ){
			delim1 = ++p;
		} else if ( c == ':' ){
			delim2 = ++p;
		} else {
			p = CharNext( p );
		}
	}

	return (tchar*)( delim1 ? delim1 : ( delim2 ? delim2 : path ) );
}
#endif


#if 0
//
// TZipLoader class
//
TZipLoader::TZipLoader()
	:super(new TLoadCounter)
{
}
bool TZipLoader::Load()
{
	return super::Load(_t("LiteZip.dll"));
}
bool TZipLoader::LoadProcs()
{
	ZipCreateFile = (FNZipCreateFileW)GetProc("ZipCreateFileW");
	ZipAddFile = (FNZipAddFileW)GetProc("ZipAddFileW");
	ZipClose = (FNZipClose)GetProc("ZipClose");
	ZipCreateBuffer = (FNZipCreateBuffer)GetProc("ZipCreateBuffer");
	ZipAddBufferRaw = (FNZipAddBufferRaw)GetProc("ZipAddBufferRaw");
	ZipGetMemory = (FNZipGetMemory)GetProc("ZipGetMemory");
	return ZipCreateFile && ZipAddFile && ZipClose;
}

//
// TUnzipLoader class
//
TUnzipLoader::TUnzipLoader()
	:super(new TLoadCounter)
{
}
bool TUnzipLoader::Load()
{
	return super::Load(_t("LiteUnzip.dll"));
}
bool TUnzipLoader::LoadProcs()
{
	UnzipOpenFile = (FNUnzipOpenFileW)GetProc("UnzipOpenFileW");
	UnzipGetItem = (FNUnzipGetItemW)GetProc("UnzipGetItemW");
	UnzipItemToFile = (FNUnzipItemToFileW)GetProc("UnzipItemToFileW");
	UnzipClose = (FNUnzipClose)GetProc("UnzipClose");
	UnzipSetBaseDir = (FNUnzipSetBaseDirW)GetProc("UnzipSetBaseDirW");
	UnzipOpenBufferRaw = (FNUnzipOpenBufferRaw)GetProc("UnzipOpenBufferRaw");
	UnzipItemToBuffer = (FNUnzipItemToBuffer)GetProc("UnzipItemToBuffer");
	UnzipRawOpen = (FNUnzipRawOpen)GetProc("UnzipRawOpen");
	UnzipRawClose = (FNUnzipRawClose)GetProc("UnzipRawClose");
	UnzipRawBuffer = (FNUnzipRawBuffer)GetProc("UnzipRawBuffer");
	return UnzipOpenFile && UnzipGetItem && UnzipItemToFile && UnzipClose && UnzipSetBaseDir;
}
#endif

#if 0
//
// TZip class
//

TZipLoader TZip::Dll;

TZip::TZip()
{
	Handle = NULL;
	WithDir = true;
}
TZip::~TZip()
{
	if (Handle){
		Close();
	}
}

bool TZip::Create(const tchar *filename, const char *passwd)
{
	if (Handle){
		// Already created.
		Result = ZR_PROGRAM;
		return false;
	}
	if (!Dll.Load()){
		// DLL load error.
		Result = ZR_DLLLOAD;
		return false;
	}
	if ((Result=Dll.ZipCreateFile(&Handle, filename, passwd))!=0){
		Dll.Unload();
		return false;
	}
	return true;
}

void TZip::Close()
{
	if (!Handle)
		return;
	Dll.ZipClose(Handle);
	Handle = NULL;
	Dll.Unload();
}

bool TZip::AddFile(const tchar *filename)
{
	return _AddFile(filename, WithDir ? filename : GetFileName(filename));
}
bool TZip::_AddFile(const tchar *filename, const tchar *internal_name)
{
	if (!Handle){
		Result = ZR_PROGRAM;
		return false;
	}
	if ((Result=Dll.ZipAddFile(Handle, internal_name, filename))!=0){
		return false;
	}
	return true;
}
#endif

//
// TUnzipLoader class
//
DWORD WINAPI TUnzipLoader::UnzipOpenFileW(HUNZIP *hzip, const WCHAR *filename, const char *passwd)
	{ return ::UnzipOpenFileW(hzip, filename, passwd); }
DWORD WINAPI TUnzipLoader::UnzipGetItemW(HUNZIP tunzip, ZIPENTRY *ze)
	{ return ::UnzipGetItemW(tunzip, ze); }
DWORD WINAPI TUnzipLoader::UnzipItemToFileW(HUNZIP tunzip, const WCHAR *fn, ZIPENTRY *ze)
	{ return ::UnzipItemToFileW(tunzip, fn, ze); }
DWORD WINAPI TUnzipLoader::UnzipClose(HUNZIP tunzip)
	{ return ::UnzipClose(tunzip); }
DWORD WINAPI TUnzipLoader::UnzipSetBaseDirW(HUNZIP tunzip, const WCHAR *dir)
	{ return ::UnzipSetBaseDirW(tunzip, dir); }

DWORD WINAPI TUnzipLoader::UnzipOpenBufferRaw(HUNZIP *tunzip, void *z, DWORD len, const char *password)
	{ return ::UnzipOpenBufferRaw(tunzip, z, len, password); }
DWORD WINAPI TUnzipLoader::UnzipItemToBuffer(HUNZIP tunzip, void *z, DWORD len, ZIPENTRY *ze)
	{ return ::UnzipItemToBuffer(tunzip, z, len, ze); }

DWORD WINAPI TUnzipLoader::UnzipRawOpen(HUNZIP *tunzip)
	{ return ::UnzipRawOpen(tunzip); }
DWORD WINAPI TUnzipLoader::UnzipRawClose(HUNZIP tunzip)
	{ return ::UnzipRawClose(tunzip); }
DWORD WINAPI TUnzipLoader::UnzipRawBuffer(HUNZIP tunzip, const void *in, unsigned compsize, void *out, unsigned decompsize)
	{ return ::UnzipRawBuffer(tunzip, in, compsize, out, decompsize); }

//
// TUnzip class
//
TUnzipLoader TUnzip::Dll;

TUnzip::TUnzip()
{
	Handle = NULL;
}

TUnzip::~TUnzip()
{
	if (Handle){
		Close();
	}
}

bool TUnzip::Open(const tchar *filename, const char *passwd)
{
	if (Handle){
		// Already opened.
		Result = ZR_PROGRAM;
		return false;
	}
	if ((Result=Dll.UnzipOpenFile(&Handle, filename, passwd))!=0){
		Dll.Unload();
		return false;
	}
	ze.Index = (DWORD)-1;
	if ((Result=Dll.UnzipGetItem(Handle, &ze))!=0){
		Close();
		return false;
	}
	NumItems = ze.Index;

	return true;
}

void TUnzip::Close()
{
	if (!Handle)
		return;
	Dll.UnzipClose(Handle);
	Handle = NULL;
	Dll.Unload();
}

bool TUnzip::SetBaseDir(const tchar *dir)
{
	if (!Handle){
		Result = ZR_PROGRAM;
		return false;	// not opened.
	}
	Result = Dll.UnzipSetBaseDir(Handle, dir);
	return Result==0;
}

bool TUnzip::GetEntry(ZIPENTRY &ze)
{
	if (!Handle){
		Result = ZR_PROGRAM;
		return 0;
	}

	Result = Dll.UnzipGetItem(Handle, &ze);
	return Result==0;
}

// if entry is -1, then unzip all files.
// overwrite = true;	// force to overwrite
bool TUnzip::UnzipToFile(int entry, const tchar *unzipdir, bool overwrite)
{
	if (!Handle){
		Result = ZR_PROGRAM;
		return 0;
	}

	if (unzipdir){
		Dll.UnzipSetBaseDir(Handle, unzipdir);
	}
	
	int num = 0;
	for (ze.Index=0;ze.Index<(DWORD)NumItems;ze.Index++){
		if (Dll.UnzipGetItem(Handle, &ze)!=0){
			break;
		}
		if (entry==-1 || (int)ze.Index==entry){
			if (overwrite){
				tnstr path;
				if (unzipdir){
					path = MakePath(unzipdir, ze.Name);
				} else {
					path = ze.Name;
				}
				if (fexist(path)){
					wopen(path, 0);
					_tchmod(path, S_IWRITE|S_IREAD);
					_tremove(path);
				}
			}
			if ((Result=Dll.UnzipItemToFile(Handle, ze.Name, &ze))!=0){
				return false;
			}
			return true;
		}
		num++;
	}
	Result = ZR_NOFILE;
	return false;	// not found
}

int TUnzip::FindIndex(const tchar *name)
{
	if (!Handle){
		Result = ZR_PROGRAM;
		return -1;
	}

	for (ze.Index=0;ze.Index<(DWORD)NumItems;ze.Index++){
		if (Dll.UnzipGetItem(Handle, &ze)!=0){
			break;
		}
		if (!_tcsicmp(GetFileName(ze.Name), name)){
			return ze.Index;
		}
	}
	Result = ZR_NOFILE;
	return -1;	// not found
}
