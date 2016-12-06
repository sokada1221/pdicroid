#ifndef	__HELPS_H
#define	__HELPS_H

#include	"../../idh.h"

void InitHelp();
void UninitHelp();

void GetHelpFileName( tnstr &path );
void HelpContext( HWND hwnd, int id );
void HelpQuit( HWND hwnd );
void HelpIndex( HWND hwnd );
void HelpKey( HWND hwnd );
int HelpCommand(HWND hwnd, int cmd, int id);
void HelpWeb( const tchar *url );


typedef int (WINAPI *FNHelpXCallback)(HWND hwnd, int cmd, int id);
extern FNHelpXCallback cbHelp;

int WinHelpX(HWND hwnd, const tchar *path, int cmd, int id);
int WinHelpX(HWND hwnd, int cmd, int id);

// MessageBox with Help
int MessageBoxHelp(HWND hwnd, const tchar *text, const tchar *caption, int type, int helpId);

// for VCL bug
HWND FindHelpWindow();
void EnableHelpWindow();

// Helper
#define	CALLHELP(id)	Application->HelpContext(id)
#define	CALLHELPDLG()	Application->HelpContext(this->HelpContext)

extern const tchar *STR_HELPWEB_HOMEPAGE;
extern const tchar *STR_HELPWEB_QA;
extern const tchar *STR_HELPWEB_DOWNLOAD;

extern int LastHelpId;

#endif	__HELPS_H

