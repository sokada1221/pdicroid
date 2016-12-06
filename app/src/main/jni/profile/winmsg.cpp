#include	"tnlib.h"
#pragma	hdrstop
#include "pdconfig.h"
#include "id.h"
#include	"winmsg.h"
#include	"pdcom.h"
#include "wpdcom.h"

#ifdef GUI
static const tchar *szMessageText;

const int StrColorNames[] = {
	TID_ColorNameSystem, TID_ColorNameBlack, TID_ColorNameMaroon, TID_ColorNameGreen, TID_ColorNameOlive,
	TID_ColorNameNavy, TID_ColorNamePurple, TID_ColorNameTeal, TID_ColorNameGray,
	TID_ColorNameSilver, TID_ColorNameRed, TID_ColorNameLime, TID_ColorNameYellow,
	TID_ColorNameBlue, TID_ColorNameFuchsia, TID_ColorNameAqua, TID_ColorNameWhite, TID_ColorNameAny
};

#ifdef WIN32
#define	__EXPORT
#else
#define	__EXPORT	_export
#endif

BOOL CALLBACK __EXPORT MessageDialogProc( HWND hDlg, UINT msg, WPARAM wparam, LPARAM )
{
	switch ( msg ){
		case WM_INITDIALOG:
			SetWindowText( GetDlgItem( hDlg, ID_TEXT ), szMessageText );
			return TRUE;
		case WM_COMMAND:
			EndDialog( hDlg, LOWORD(wparam) );
			return TRUE;
	}
	return FALSE;
}

int MessageDialog( HWND hwnd, LPCTSTR dlgname, const tchar *text )
{
	szMessageText = text;
	__assert(FindResource(hTNInstance, dlgname, RT_DIALOG));
	return DialogBox( hTNInstance, dlgname, hwnd, (DLGPROC)MessageDialogProc );
}

const tchar *GetStateMessage( int msgno )
{
	const tchar *p = _LT(msgno);
	__assert(!msgno||p);
	return p;
}

// Error Message with msgno.
// Simple error message if the str argument is NULL.
void ErrorMessage( HWND hwnd, int msgno, const tchar *str, const tchar *title )
{
	if ( !title ){
		title = _LT(TID_ERROR);
	}
	tchar *buf = NULL;
	int msgid;
	if (msgno>0&&msgno<512){
		msgid = msgno+MSG_DICERROR_FIRST;	// It may be a dictionary error message.
		if (msgid>=MSG_DicLast){
			// undefined dictionary error message.
			msgid = MSG_DicParameter;	// parameter error (Programming Error)
		}
	} else {
		msgid = msgno;
	}
	const tchar *p = _LT(msgid);
	if ( !p ){
		p = str;
		if (!p && msgno>=0){
			// msgno or text resource error
			p = _T("Undefined error message or load error");
			str = _T("");
		}
	}
	if (str){
		buf = new tchar[ _tcslen( p ) + _tcslen( str ) + 16 + 1 ];
		if ( !buf ){
			MessageBox(hwnd, _T("Out of memory"), NULL, MB_OK|MB_ICONSTOP);
			return;
		}
		wsprintf( buf, p, str );
		if (msgno<TID_ORIGIN){
			// 桁数が少ない場合は10進数で表示
			wsprintf( buf + _tcslen(buf), _T("[%d]"), msgno );
		} else {
			// 桁数が多い場合は16進数で表示
			wsprintf( buf + _tcslen(buf), _T("[%04X]"), msgno-TID_ORIGIN );
		}
		p = buf;
	}
	MsgBox( hwnd, p, title, MB_OK|MB_ICONWARNING);
	if (buf)
		delete[] buf;
}

tnstr GetErrorMessage( int msgno, const tchar *str )
{
	tchar *buf = NULL;
	int msgid = (msgno>0&&msgno<512) ?
		msgno+MSG_DICERROR_FIRST : // It may be a dictionary error message.
		msgno;
	const tchar *p = _LT(msgid);
	if ( !p ){
		p = str;
		if (!p && msgno>=0){
			// msgno or text resource error
			p = _T("Undefined error message or load error");
			str = _T("");
		}
	}
	if (str){
		buf = new tchar[ _tcslen( p ) + _tcslen( str ) + 16 + 1 ];
		if ( !buf ){
			return _t("Out of memory");
		}
		wsprintf( buf, p, str );
		if (msgno<TID_ORIGIN){
			// 桁数が少ない場合は10進数で表示
			wsprintf( buf + _tcslen(buf), _T("[%d]"), msgno );
		} else {
			// 桁数が多い場合は16進数で表示
			wsprintf( buf + _tcslen(buf), _T("[%04X]"), msgno-TID_ORIGIN );
		}
		p = buf;
	}
	tnstr ret(p);
	if (buf)
		delete[] buf;
	return ret;
}

// Simple error message.
void ErrorMessage(HWND hwnd, int msgno)
{
	tchar buf[40];
	const tchar *p = _LT(msgno);
	if (!p){
		wsprintf(buf, _T("Undefined error message msgno=%d"), msgno);
		p = buf;
	}
	MsgBox(hwnd, buf, NULL, MB_OK|MB_ICONWARNING);
}

const tchar *GetWinErrorMessage( int error )
{
	switch ( error ){
		case ERROR_DIRECTORY:
			return _LT(MSG_WRONGDIRTOEXECUTE);
	}
	return NULL;
}

#endif

// Note:
// The max length of the message is limited by 1024 bytes.
tnstr GetStringParam(int id, ...)
{
	va_list ap;
	va_start(ap, id);
	const tchar *fmt = _LT(id);
	__assert(fmt);
	tnstr ret = _tnsprintf(fmt, ap);
	va_end(ap);
	return ret;
}

///////////////////////////////////////////////////////////////
// MessageBox
///////////////////////////////////////////////////////////////
int MsgBox(HWND hwnd, int text, int caption, UINT type)
{
	return MsgBox(hwnd, _LT(text), caption?_LT(caption):NULL, type);
}


