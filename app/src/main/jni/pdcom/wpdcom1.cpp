//---------------------------------------------------------------------------
// applicationに依存しないwpdcom部分

#include "tnlib.h"
#pragma hdrstop

#include "wpdcom.h"
#include "pdstr.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

///////////////////////////////////////////////////////////////
// ファイル名関連
///////////////////////////////////////////////////////////////
//TODO: to filestr.cpp
#ifndef WINCE
// ext : .を含まない拡張子
bool IsExecuteExtension( const tchar *ext )
{
	return !_tcsicmp( ext, StrEXE ) || !_tcsicmp( ext, StrCOM ) || !_tcsicmp( ext, StrBAT );
}

bool IsBitmapExtension( const tchar *ext )
{
	return !_tcsicmp( ext, StrBMP ) || !_tcsicmp( ext, StrDIB );
}
#endif	// !WINCE

///////////////////////////////////////////////////////////////
// クリップボード
///////////////////////////////////////////////////////////////
// lenは終端のNULLを含まない
bool CopyClipboard( HWND hwnd, const tchar *txt, uint len )
{
	if ( !OpenClipboard( hwnd ) ){
		// クリップボードがオープンできない
		return false;
	}
	if (!EmptyClipboard( )){
		CloseClipboard();
		return false;
	}

	bool ret = CopyClipboard( txt, len );

	CloseClipboard( );
	return ret;
}
#define	USELOCAL	1
bool CopyClipboard( const tchar *txt, uint len )
{
	if (len==(UINT)-1){
		len = _tcslen(txt);
	}

#ifdef WINCE
#if USELOCAL	// どっちがいいのでしょう？？？？？？
	HLOCAL hmem = LocalAlloc( LMEM_MOVEABLE, LENTOBYTE(len+1) );
	if ( !hmem ) return false;
	LPTSTR lpmem = (LPTSTR)LocalLock( hmem );
#else
	HANDLE hmem = HeapCreate( HEAP_NO_SERIALIZE, LENTOBYTE(len+1), LENTOBYTE(len+1) );
	if ( !hmem ) return false;
	LPTSTR lpmem = (LPTSTR)HeapAlloc( hmem, HEAP_NO_SERIALIZE, LENTOBYTE(len+1) );
#endif
#else
	HGLOBAL hmem = GlobalAlloc( GMEM_MOVEABLE|GMEM_DDESHARE, LENTOBYTE(len+1) );
	if ( !hmem ) return false;
	LPTSTR lpmem = (LPTSTR)GlobalLock( hmem );
#endif
	if ( !lpmem ){
#ifdef WINCE
#if USELOCAL
		LocalFree( hmem );
#else
		HeapDestroy( hmem );
#endif
#else
		GlobalFree( hmem );
#endif
		return false;
	}
	memcpy( lpmem, txt, (UINT)LENTOBYTE(len) );
	lpmem[ len ] = '\0';
#ifdef WINCE
#if USELOCAL
	LocalUnlock( hmem );
#else
#endif
#else
	GlobalUnlock( hmem );
#endif
	return SetClipboardData( CF_TTEXT, hmem )!=NULL;
}

///////////////////////////////////////////////////////////////
// ウィンドウ操作
///////////////////////////////////////////////////////////////
static HWND hwndMain = NULL;
static WORD LangId = 0;

void SetPdcomMainWindow(HWND hwnd)
{
	hwndMain = hwnd;
}

// window(hwnd)をhwndParentのcenterへ移動
// screenの外へ出る場合は調整する
void MoveCenter( HWND hwnd, HWND hwndParent )
{
	RECT rc;
	RECT rd;
	if (!hwndParent){
		hwndParent = hwndMain;	// parentがなければmain window
	}
	if (hwndParent){
#ifndef WINCE
		if ( IsIconic(hwndParent) ){
			GetScreenSize(hwndParent, &rc);
		} else
#endif
		{
			::GetWindowRect( hwndParent, &rc );
		}
	} else {
		GetScreenSize(hwnd, &rc);
	}
	RECT rcScr;
	GetScreenSize(hwnd, &rcScr);
	::GetWindowRect( hwnd, &rd );
	int width = rd.right - rd.left;
	int height = rd.bottom - rd.top;
	int left = rc.left + ( ( rc.right - rc.left ) - width )/2;
	int top = rc.top + ( ( rc.bottom - rc.top ) - height )/2;
	if ( left < rcScr.left ){
		left = rcScr.left;
	}
	if ( top < rcScr.top ){
		top = rcScr.top;
	}
	if ( left + width > rcScr.right ){
		left = rcScr.right - (rd.right - rd.left);
	}
	if ( top + height > rcScr.bottom ){
		top = rcScr.bottom - (rd.bottom - rd.top);
	}
	::SetWindowPos( hwnd, (HWND)NULL, left & ~7, top, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
}


void GetScreenSize(HWND hwndBase, RECT *rcWork, RECT *rcScreen)
{
#if 0
	if (!PdicMain){
		__assert(false);
		MLFXPC_CMN_ASSERT(false);
		if (rcScreen){
			SetRect(rcScreen, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		}
		if (rcWork){
			SystemParametersInfo(SPI_GETWORKAREA, 0, rcWork, 0);
		}
		return;
	}
#endif
	//__assert(hwndMain!=NULL);
	HMONITOR hMonitor = MonitorFromWindow(hwndBase ? hwndBase : (hwndMain ? hwndMain : GetActiveWindow()), MONITOR_DEFAULTTONEAREST);
	__assert(hMonitor!=NULL);
	MONITORINFO mi;
	memset(&mi, 0, sizeof(mi));
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);
	if (rcScreen){
		*rcScreen = mi.rcMonitor;
	}
	if (rcWork){
		*rcWork = mi.rcWork;
	}
}

bool GetScreenSize(HMONITOR hMonitor, RECT &scr)
{
	if (hMonitor){
		MONITORINFO mi;
		memset(&mi, 0, sizeof(mi));
		mi.cbSize = sizeof(mi);
		if (GetMonitorInfo(hMonitor, &mi)){
			scr = mi.rcWork;
			return true;
		}
	}
	// Uses old fashion API
	SystemParametersInfo( SPI_GETWORKAREA, 0, (PVOID)&scr, 0 );
	return false;	// single monitor
}
// ref : [I] reference point in the current monitor
// scr : [O] Rectangle of the current monitor
bool GetScreenSize(POINT &ref, RECT &scr)
{
	HMONITOR hMonitor = MonitorFromPoint(ref, MONITOR_DEFAULTTONEAREST);
	return GetScreenSize(hMonitor, scr);
}
// ref : [I] reference rectangle in the current monitor
// scr : [O] Rectangle of the current monitor
bool GetScreenSize(RECT &ref, RECT &scr)
{
	HMONITOR hMonitor = MonitorFromRect(&ref, MONITOR_DEFAULTTONEAREST);
	return GetScreenSize(hMonitor, scr);
}

// 現在最前面の親ウインドウを取得する
HWND GetForeWindow()
{
	HWND hwnd = GetForegroundWindow( );
	while ( 1 ){
		HWND _hwnd = ::GetParent( hwnd );
		if ( !_hwnd )
			break;
		hwnd = _hwnd;
	}
	return hwnd;
}

// hwndの一番上のparent windowを返す
HWND GetTopParent(HWND hwnd)
{
	while (1){
		LONG style = GetWindowLong(hwnd, GWL_STYLE);
		if (!(style&WS_CHILDWINDOW))
			return hwnd;
		HWND _hwnd = ::GetParent( hwnd );
		if ( !_hwnd )
			break;
		hwnd = _hwnd;
	}
	return hwnd;
}

///////////////////////////////////////////////////////////////
// Language
///////////////////////////////////////////////////////////////
static struct TLangInfo {
	const char *name;
	WORD langid;
	WORD sublangid;
} LangInfo[] = {
	{ "Japanese",	LANG_JAPANESE },
	{ "English",	LANG_ENGLISH },
	{ NULL }	// terminator
};
void SetupLanguage(const char *langid)
{
	LangId = 0;	// reset
	for (int i=0;;i++){
		TLangInfo &li = LangInfo[i];
		if (!li.name)
			break;
		if (!stricmp(li.name, langid)){
			// found a language
			WORD langid;
			if (li.sublangid==0){
				langid = MAKELANGID(li.langid, SUBLANG_DEFAULT);
			} else {
				langid = MAKELANGID(li.langid, li.sublangid);
			}
			if (::IsValidLocale(MAKELCID(langid, SORT_DEFAULT), LCID_INSTALLED)){	// インストールされている言語のみサポート
				// installed language
				LangId = langid;
			} else {
				// not installed
				//TODO: 警告を出した方が良い？
			}
			break;
		}
	}
}

WORD GetLangId()
{
	return LangId;
}

///////////////////////////////////////////////////////////////
// MessageBox
///////////////////////////////////////////////////////////////
static void SetMsgBox(HWND hwnd);
// 任意のwindowを親ウインドウとして表示
// NULLの場合はmain window
// -1の場合はdesktop
int MsgBox(HWND hwnd, LPCTSTR text, LPCTSTR caption, UINT type)
{
	if (!hwnd){
		hwnd = hwndMain;
		if (!hwnd){
			hwnd = GetActiveWindow();
		}
	}
	if (hwnd){
		if (hwnd==(HWND)-1){
			hwnd = NULL;	// desktop window
		} else {
			SetMsgBox(hwnd);
		}
	}
	return ::MessageBoxEx(hwnd, text, caption, type, LangId);
}
// 現在activeなwindowを親ウインドウとして表示
int MsgBox(LPCTSTR text, LPCTSTR caption, UINT type)
{
	HWND hwnd = GetActiveWindow();
	return ::MsgBox(hwnd, text, caption, type);
}

#if 1	// 方法その１
// MessageBox をhwndParentの中心に表示するための処理
static HHOOK hHook;
static HWND hwndParent;
static LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HCBT_ACTIVATE){
		HWND hwndMsgBox = (HWND)wParam;
		RECT rcMsgBox;
		RECT rcParent;
		GetWindowRect(hwndMsgBox, &rcMsgBox);
		GetWindowRect(hwndParent, &rcParent);
		int x = rcParent.left + (rcParent.right-rcParent.left)/2 - (rcMsgBox.right-rcMsgBox.left)/2;
		int y = rcParent.top + (rcParent.bottom-rcParent.top)/2 - (rcMsgBox.bottom-rcMsgBox.top)/2;
		SetWindowPos(hwndMsgBox, 0, x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
		UnhookWindowsHookEx(hHook);
	}
	return 0;
}
static void SetMsgBox(HWND hwnd)
{
	hwndParent = hwnd;
	hHook = SetWindowsHookEx(WH_CBT, CBTProc, hTNInstance, GetCurrentThreadId());
}
#endif

#if 0	// 方法その２
static void MoveMsgbx( HWND hwnd )
{
	// OKボタンの中央に合わせる
	POINT mouse;
	RECT ok, mbox;
	GetCursorPos( &mouse );
	::GetWindowRect( ::GetDlgItem( hwnd, 2 ), &ok );// OKボタンのIDが何故か2
	::GetWindowRect( hwnd, &mbox );
	int x = mbox.left + mouse.x - ( ok.right + ok.left ) / 2;
	int y = mbox.top + mouse.y - ( ok.bottom + ok.top ) / 2;
	::SetWindowPos( hwnd, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE );
}

// サブクラス化
static WNDPROC g_wndproc;
static LONG CALLBACK WindowProc( HWND hwnd, UINT uMsg, UINT wp, LONG lp );
static void Subclass( HWND hwnd )
{
	g_wndproc = ( WNDPROC )::GetWindowLong( hwnd, GWL_WNDPROC );
	::SetWindowLong( hwnd, GWL_WNDPROC, LONG( WindowProc ) );
}
static void Unsubclass( HWND hwnd )
{
	::SetWindowLong( hwnd, GWL_WNDPROC, LONG( g_wndproc ) );
	g_wndproc = 0;
}
static LONG CALLBACK WindowProc( HWND hwnd, UINT uMsg, UINT wp, LONG lp )
{
	LONG ret = ::CallWindowProc( g_wndproc, hwnd, uMsg, wp, lp );
	if ( WM_INITDIALOG == uMsg ){
		MoveMsgbx( hwnd );
		Unsubclass( hwnd );
	}
	return ret;
}

// フック
static HHOOK g_hhook;
static LONG CALLBACK CallWndRetProc( int code, UINT wp, LONG lp );
static void Hook( HINSTANCE hi, DWORD id )
{
	g_hhook = ::SetWindowsHookEx( WH_CALLWNDPROCRET, CallWndRetProc, hi, id );
}
static void Unhook()
{
	::UnhookWindowsHookEx( g_hhook );
	g_hhook = 0;
}
static LONG CALLBACK CallWndRetProc( int code, UINT wp, LONG lp )
{
	const CWPRETSTRUCT* p = ( const CWPRETSTRUCT* )lp;
	if ( WM_NCCREATE == p->message )
	{// 最初に捕まるWM_NCCREATEがメッセージボックスのものと決め付け（怪しいかも
		Unhook();
		Subclass( p->hwnd );
	}
	return ::CallNextHookEx( g_hhook, code, wp, lp );
}
static void SetMsgBox(HWND hwnd)
{
	Hook(hTNInstance, GetCurrentThreadId());
}
#endif

#include <wininet.h>

tnstr FormatMessage(int errcode)
{
	LPTSTR lpBuffer = NULL;
	if (errcode>=INTERNET_ERROR_BASE){
		HMODULE hWininet = LoadLibrary(_t("WININET.DLL"));
		FormatMessage(
			FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			hWininet,
			errcode,
			0,
			(LPTSTR)&lpBuffer,
			LangId,	//TODO: 英語になってしまう。。
			NULL );
		GetLastError();
		FreeLibrary(hWininet);
	} else {
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			errcode,
			LangId,
			(LPTSTR)&lpBuffer,
			0,
			NULL );
	}
	GetLastError();
	tnstr ret(lpBuffer);
	LocalFree(lpBuffer);
	return ret;
}

#include "filestr.h"

bool IsAudioFile(const tchar *filename, const tchar *ext)
{
	if ( !ext ){
		if ( !filename )
			return false;
		ext = GetFileExtension( filename );
		if (!ext)
			return false;
	}
	if (!_tcsicmp(ext, _t("mp3"))
		|| !_tcsicmp(ext, _t("wav"))
		|| !_tcsicmp(ext, _t("wma"))
		|| !_tcsicmp(ext, _t("ogg"))
		|| !_tcsicmp(ext, _t("mid"))
		){
		return true;
	}
	return false;
}

#include <DbgHelp.h>
// C++Builder5のDbgHelp.hに無いもの //
typedef struct _SYMBOL_INFO {
  ULONG   SizeOfStruct;
  ULONG   TypeIndex;
  ULONG64 Reserved[2];
  ULONG   Index;
  ULONG   Size;
  ULONG64 ModBase;
  ULONG   Flags;
  ULONG64 Value;
  ULONG64 Address;
  ULONG   Register;
  ULONG   Scope;
  ULONG   Tag;
  ULONG   NameLen;
  ULONG   MaxNameLen;
  TCHAR   Name[1];
} SYMBOL_INFO, *PSYMBOL_INFO;
#define	_In_
#define	_Out_
#define	_Out_opt_
#define	_Inout_
typedef USHORT WINAPI (*FNCaptureStackBackTrace)(
  _In_       ULONG FramesToSkip,
  _In_       ULONG FramesToCapture,
  _Out_      PVOID *BackTrace,
  _Out_opt_  PULONG BackTraceHash
);
typedef BOOL WINAPI (*FNSymFromAddr)(
  _In_       HANDLE hProcess,
  _In_       DWORD64 Address,
  _Out_opt_  PDWORD64 Displacement,
  _Inout_    PSYMBOL_INFO Symbol
);
// End of //
LONG CALLBACK SWFilter(EXCEPTION_POINTERS *ExInfo);

void InitExceptionInfo()
{
	SetUnhandledExceptionFilter(SWFilter);
}
LONG CALLBACK SWFilter(EXCEPTION_POINTERS *ExInfo)
{
	STACKFRAME sf;
	BOOL bResult;
	PIMAGEHLP_SYMBOL pSym;
	DWORD Disp;

	DBW("--------- Unhandled Exception");

	/* シンボル情報格納用バッファの初期化 */
	pSym = (PIMAGEHLP_SYMBOL)GlobalAlloc(GMEM_FIXED, 10000);
	pSym->SizeOfStruct = 10000;
	pSym->MaxNameLength = 10000 - sizeof(IMAGEHLP_SYMBOL);

	/* スタックフレームの初期化 */
	ZeroMemory(&sf, sizeof(sf));
	sf.AddrPC.Offset = ExInfo->ContextRecord->Eip;
	sf.AddrStack.Offset = ExInfo->ContextRecord->Esp;
	sf.AddrFrame.Offset = ExInfo->ContextRecord->Ebp;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrStack.Mode = AddrModeFlat;
	sf.AddrFrame.Mode = AddrModeFlat;

#if 0
	/* シンボルハンドラの初期化 */
	SymInitialize(GetCurrentProcess(), NULL, TRUE);

	/* スタックフレームを順に表示していく */
	for(;;) {
		/* 次のスタックフレームの取得 */
		bResult = StackWalk(
			IMAGE_FILE_MACHINE_I386,
			GetCurrentProcess(),
			GetCurrentThread(),
			&sf,
			NULL, 
			NULL,
			SymFunctionTableAccess,
			SymGetModuleBase,
			NULL);

		/* 失敗ならば、ループを抜ける */
		if(!bResult || sf.AddrFrame.Offset == 0) break;

		/* プログラムカウンタから関数名を取得 */
		bResult = SymGetSymFromAddr(GetCurrentProcess(), sf.AddrPC.Offset, &Disp, pSym);
		
		/* 取得結果を表示 */
		if (bResult) DBW("0x%08x %s() + 0x%x\n", sf.AddrPC.Offset, pSym->Name, Disp);
		else DBW("%08x, ---", sf.AddrPC.Offset);
	}

	/* 後処理 */
	SymCleanup(GetCurrentProcess());
#endif
	GlobalFree(pSym);

	return(EXCEPTION_EXECUTE_HANDLER);
}
tnstr GetExceptionInfo()
{
	 unsigned int   i;
	 void         * stack[ 100 ];
	 unsigned int frames;
	 SYMBOL_INFO  * symbol;
	 HANDLE         process;

	 process = GetCurrentProcess();

	 HINSTANCE hDll = LoadLibrary(_t("DbgHelp.dll"));
	 if (!hDll)
		return NULL;
	FNCaptureStackBackTrace CaptureStackBackTrace = (FNCaptureStackBackTrace)GetProcAddress(hDll, "CaptureStackBackTrace");
	FNSymFromAddr SymFromAddr = (FNSymFromAddr)GetProcAddress(hDll, "SymFromAddr");
	if (!CaptureStackBackTrace || !SymFromAddr){
		FreeLibrary(hDll);
		return NULL;
	}
	 
	 SymInitialize( process, NULL, TRUE );
	 
	 frames               = CaptureStackBackTrace( 0, 100, stack, NULL );
	 symbol               = ( SYMBOL_INFO * )calloc( sizeof( SYMBOL_INFO ) + 256 * sizeof( char ), 1 );
	 symbol->MaxNameLen   = 255;
	 symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

	 tnstrbuf ret;
	 for( i = 0; i < frames; i++ )
	 {
		 SymFromAddr( process, ( DWORD64 )( stack[ i ] ), 0, symbol );

		 ret += tnsprintf( _t("%i: %s - 0x%0X\n"), frames - i - 1, symbol->Name, symbol->Address );
	 }

	 free( symbol );

	 FreeLibrary(hDll);
	 
	 return tnstr(ret);
}

