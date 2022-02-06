#include	"tnlib.h"
#pragma	hdrstop
#if !defined(WINCE) /*|| defined(PKTPC)*/
#include <winver.h>
#endif
#include	"pdver.h"
#include	"id.h"

#if defined(WINCE) /*&& !defined(PKTPC)*/

#if defined(DIC_UTF8) || defined(DIC_UTF16) || defined(DIC_BOCU)
#define	VERSION	"1.51"
#else
#define	VERSION	"1.26"
#endif

#define	STR_COPYRIGHT	_T("Copyright(c) 1990-2004 by TaN")

#pragma	message(">>>> Version --- Ver." VERSION)

ProjectRCVersion::ProjectRCVersion ( const tchar *appFName )
{
}


ProjectRCVersion::~ProjectRCVersion ()
{
}


BOOL ProjectRCVersion::GetProductName (LPTSTR &prodName)
{
	prodName =
#if MIXDIC
		_T("Personal Dictionary for WinCE");
#else
		_T("Personal Dictionary for CE/Unicode");
#endif
	return true;
}

bool ProjectRCVersion::GetProductVersion(tnstr &prodVersion)
{
	prodVersion = _t(VERSION);
	retunr true;
}


BOOL ProjectRCVersion::GetCopyright (LPTSTR &copyright)
{
	copyright = STR_COPYRIGHT;
	return true;
}


BOOL ProjectRCVersion::GetDebug (LPTSTR &debug)
{
	debug = _T("");
	return true;
}
#else	// !WINCE

ProjectRCVersion::ProjectRCVersion(const tchar *appFName)
{
	DWORD   fvHandle;

	FVData = NULL;

	tchar *appname = _tcsdup( appFName );
	DWORD dwSize = GetFileVersionInfoSize( appname, &fvHandle);
	if (dwSize) {
		FVData  = (void *)new tchar[dwSize];
		if (!GetFileVersionInfo( appname, fvHandle, dwSize, FVData)) {
			// error
			if (FVData){
				delete[] FVData;
				FVData = NULL;
			}
		}
	}
	free( appname );
}


ProjectRCVersion::~ProjectRCVersion()
{
	if (FVData)
		delete[] FVData;
}

bool ProjectRCVersion::GetProductVersion(tnstr &prodVersion)
{
	if (!FVData)
		return false;

	VS_FIXEDFILEINFO *pFixedFileInfo;
	UINT uLen;
	if(!VerQueryValue(FVData, _T("\\"), (LPVOID*)&pFixedFileInfo, &uLen)){
		return false;
	}
	prodVersion = tnsprintf(_t("%d.%d.%d"), 
		HIWORD(pFixedFileInfo->dwProductVersionMS),
		LOWORD(pFixedFileInfo->dwProductVersionMS),
		HIWORD(pFixedFileInfo->dwProductVersionLS)
		/*,LOWORD(pFixedFileInfo->dwProductVersionLS)*/);
	return true;
}

static const TCHAR *VERS_TRANSLATION = _T("\\varFileInfo\\translation");
static const TCHAR *VERS_STRINGFILEINFO = _T("\\StringFileInfo\\%08lx\\%s");
static const TCHAR *VERS_PRODUCTNAME = _T("ProductName");
//static const TCHAR *VERS_PRODUCTVERSION = _T("ProductVersion");
static const TCHAR *VERS_LEGALCOPYRIGHT = _T("LegalCopyright");
static const TCHAR *VERS_SPECIALBUILD = _T("SpecialBuild");

ProjectRCVersionString::ProjectRCVersionString(const tchar *appFName)
	:super(appFName)
{
	UINT vSize;

	if (FVData){
		// If not error in super class.
		tchar    subBlockName[255];
		_tcscpy(subBlockName, VERS_TRANSLATION);
		if (!VerQueryValue(FVData, subBlockName, (void **)&TransBlock, &vSize)) {
			delete[] FVData;
			FVData = 0;
		} else {
			// 上下ワードをスワップして，wsprintf が lang-charset を正しい書式で出力するようにする
			*(DWORD *)TransBlock = MAKELONG(HIWORD(*(DWORD *)TransBlock), LOWORD(*(DWORD *)TransBlock));
		}
	}
}

bool ProjectRCVersionString::GetProductName(tnstr &prodName)
{
	if (!FVData)
		return false;

	UINT    vSize;
	tchar    subBlockName[255];

	wsprintf(subBlockName, VERS_STRINGFILEINFO, *(DWORD *)TransBlock, VERS_PRODUCTNAME);
	LPTSTR pptr;
	if (!VerQueryValue(FVData, subBlockName, (void **)&pptr, &vSize)){
		return false;
	}
	prodName = pptr;
	return true;
}

#if 0
bool ProjectRCVersionString::GetProductVersion(tnstr &prodVersion)
{
	if (!FVData)
		return false;

	UINT    vSize;
	tchar    subBlockName[255];

	wsprintf(subBlockName, VERS_STRINGFILEINFO, *(DWORD *)TransBlock, VERS_PRODUCTVERSION);
	LPTSTR pptr;
	if (!VerQueryValue(FVData, subBlockName, (void **)&pptr, &vSize)){
		return false;
	}
	prodVersion = pptr;
	return true;
}
#endif

bool ProjectRCVersionString::GetCopyright(tnstr &copyright)
{
	if (!FVData)
		return false;

	UINT    vSize;
	tchar    subBlockName[255];

	wsprintf(subBlockName, VERS_STRINGFILEINFO, *(DWORD *)TransBlock, VERS_LEGALCOPYRIGHT);
	LPTSTR pptr;
	if (!VerQueryValue(FVData, subBlockName, (void FAR* FAR*)&pptr, &vSize)){
		return false;
	}
	copyright = pptr;
	return true;
}


bool ProjectRCVersionString::GetDebug(tnstr &debug)
{
	if (!FVData)
		return false;

	UINT    vSize;
	tchar    subBlockName[255];

	wsprintf(subBlockName, VERS_STRINGFILEINFO, *(DWORD *)TransBlock, VERS_SPECIALBUILD);
	LPTSTR pptr;
	if (!VerQueryValue(FVData, subBlockName, (void FAR* FAR*)&pptr, &vSize)){
		return false;
	}
	debug = pptr;
	return true;
}

#endif

// Version comparison for application.
int CompareVersion(const tchar *v1, const tchar *v2)
{
	const int NUM_DIGIT	= 3;
	int n1[NUM_DIGIT] = {0,0,0};
	int n2[NUM_DIGIT] = {0,0,0};

	if(EOF == _stscanf(v1, _T("%d.%d.%d"), &n1[0], &n1[1], &n1[2])){
		// format error. assume to be zero.
	}
	if(EOF == _stscanf(v2, _T("%d.%d.%d"), &n2[0], &n2[1], &n2[2])){
		// format error. assume to be zero. 
	}

	for(int i=0;i<NUM_DIGIT;i++){
		if(n1[i] < n2[i]){
			return -1;	// n1 is older than n2.
		} else if(n1[i] == n2[i]){
			continue;
		} else {
			return 1;	// n1 is newer than n2.
		}
	}
	return 0;	// Same version.
}

