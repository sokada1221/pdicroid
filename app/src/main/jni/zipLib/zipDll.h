#ifndef __zipDll_h
#define	__zipDll_h

#define	__AVOID_CONF
//#include "LiteZip.h"
#include "LiteUnzip.h"

class TZipCommon {
protected:
	int Result;		// Last result code. (ZR_xxx defined in LiteUnzip.h)
public:
	TZipCommon()
	{
		Result = 0;
	}
	virtual ~TZipCommon(){}
	int GetResult() const
		{ return Result; }
};

#if 0
class TZip : public TZipCommon {
typedef TZipCommon super;
protected:
	HZIP Handle;
	bool WithDir;
public:
	TZip();
	virtual ~TZip();
	bool Create(const tchar *filename, const char *passwd=NULL);
	void Close();
	void SetWithDir(bool yes)
		{ WithDir = yes; }
	bool AddFile(const tchar *filename);
	bool _AddFile(const tchar *filename, const tchar *internal_name);

	static TZipLoader &dll()
		{ return Dll; }
};
#endif

class TUnzipLoader {
public:
	// unzip APIs //
	static DWORD WINAPI UnzipOpenFileW(HUNZIP *hzip, const WCHAR *filename, const char *passwd);
	static DWORD WINAPI UnzipGetItemW(HUNZIP, ZIPENTRY *);
	static DWORD WINAPI UnzipItemToFileW(HUNZIP, const WCHAR *, ZIPENTRY *);
	static DWORD WINAPI UnzipClose(HUNZIP);
	static DWORD WINAPI UnzipSetBaseDirW(HUNZIP, const WCHAR *);

	static DWORD WINAPI UnzipOpenBufferRaw(HUNZIP *, void *, DWORD, const char *);
	static DWORD WINAPI UnzipItemToBuffer(HUNZIP, void *, DWORD, ZIPENTRY *);

	static DWORD WINAPI UnzipRawOpen(HUNZIP *);
	static DWORD WINAPI UnzipRawClose(HUNZIP);
	static DWORD WINAPI UnzipRawBuffer(HUNZIP, const void *, unsigned, void *, unsigned);
public:
	bool Load() { return true; }
	void Unload(){}	// dummy
};

class TUnzip : public TZipCommon {
typedef TZipCommon super;
protected:
	static TUnzipLoader Dll;
	HUNZIP Handle;
	ZIPENTRY ze;
	int NumItems;	// Enabled after opened.

public:
	TUnzip();
	virtual ~TUnzip();
	bool Open(const tchar *filename, const char *passwd=NULL);
	void Close();
	int GetNumItems() const
		{ return NumItems; }
	bool SetBaseDir(const tchar *dir);
	bool GetEntry(ZIPENTRY &ze);
	bool UnzipToFile(int entry=-1, const tchar *unzipdir=NULL, bool overwrite=true);
	int FindIndex(const tchar *name);

	static TUnzipLoader &dll()
		{ return Dll; }
};

enum {
	ZR_PROGRAM = 0x100,	// already handle opened/handle is not opened.
	ZR_DLLLOAD,			// zip dll load error
};

#endif	/* __zipDll_h */

