#include	<windows.h>
#pragma	hdrstop
#include	"tnwinlib.h"

HINSTANCE hTNInstance = NULL;

#if defined(WINCE) && defined(SUPPORT_LANG)
FNWideCharToMultiByte _WideCharToMultiByte;
FNMultiByteToWideChar _MultiByteToWideChar;
static HINSTANCE hInstUniTbl = NULL;

typedef int (*FNWideCharToMultiByteDll)( UINT, DWORD, LPCWSTR, int, LPSTR, int, LPCSTR, LPBOOL );
typedef int (*FNMultiByteToWideCharDll)( UINT, DWORD, LPCSTR, int, LPWSTR, int );

static FNWideCharToMultiByteDll WideCharToMultiByteDll;
static FNMultiByteToWideCharDll MultiByteToWideCharDll;

int WINAPI WideCharToMultiByteStub( UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cchMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar )
{
	// UNITBL32.DLLÇÃbugëŒçÙ
	if ( cchWideChar == 0 ){
		lpMultiByteStr[0] = '\0';
		return 0;
	}
	return WideCharToMultiByteDll( CodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cchMultiByte, lpDefaultChar, lpUsedDefaultChar );
}
int WINAPI MultiByteToWideCharStub( UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, int cchMultiByte, LPWSTR lpWideCharStr, int cchWideChar )
{
	return MultiByteToWideCharDll( CodePage, dwFlags, lpMultiByteStr, cchMultiByte, lpWideCharStr, cchWideChar );
}
#endif

#if 1
#define	DBU(x)
#define	_DBU(x)
#else
#define	DBU(x)	_DBU(_T(x))
#define	_DBU(x)	MessageBox( NULL, x, _T(""), MB_OK )
#endif

void InitTNWinLib( HINSTANCE hInstance )
{
	hTNInstance = hInstance;
#if defined(WINCE) && defined(SUPPORT_LANG)
	if ( GetSystemDefaultLangID() != 0x0411 )
	{
		// Not Japanese
		hInstUniTbl = LoadLibrary( _T("UNITBL32.DLL") );
		if ( hInstUniTbl ){
			WideCharToMultiByteDll = (FNWideCharToMultiByteDll)GetProcAddress( hInstUniTbl, _T("WideCharToMultiByte") );
			MultiByteToWideCharDll = (FNMultiByteToWideCharDll)GetProcAddress( hInstUniTbl, _T("MultiByteToWideChar") );
			if ( WideCharToMultiByteDll && MultiByteToWideCharDll ){
				_WideCharToMultiByte = WideCharToMultiByteStub;
				_MultiByteToWideChar = MultiByteToWideCharStub;
#if 0
				{
					wchar_t b[50];
					wchar_t c[20];
					unsigned char d[10];
					_WideCharToMultiByte( CP_ACP, 0, _T("ABÇ†Ç¢"), 4, (char*)d, sizeof(d), NULL, NULL );
					wsprintf( b, _T("%02X %02X %02X %02X"), d[0], d[1], d[2], d[3] );
					_DBU(b);
					_MultiByteToWideChar( CP_ACP, 0, "ABÇ†Ç¢", -1, c, sizeof(c) );
					wsprintf( b, _T("%04X %04X %04X %04X"), c[0], c[1], c[2], c[3] );
					_DBU(b);
				}
#endif
				return;
			}
		}
	}
	_WideCharToMultiByte = WideCharToMultiByte;
	_MultiByteToWideChar = MultiByteToWideChar;
#endif
}

void FinishTNWinLib()
{
#if defined(WINCE) && defined(SUPPORT_LANG)
	if ( hInstUniTbl ){
		FreeLibrary( hInstUniTbl );
	}
#endif
}
