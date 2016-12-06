#ifndef __ASSEXEC_H
#define	__ASSEXEC_H

typedef bool (*FNTextLinkProc)(const tchar *filename, int line, int column);

void SetTextLinkProc(FNTextLinkProc TextLinkProc);
void GetTextLinkParam( const tchar *param, tnstr &filename, int &line, int &col, class Pdic *dic );
void GetTextLinkParam( const tchar *param, tnstr &filename, int &line, int &col, class MPdic *dic );
bool AssociateExecute( HWND, const tchar *filename, int type=0, bool activate=true );
bool AssociateExecuteEx(HWND hwnd, const tchar *filename, int type, bool activate, Pdic *dic);
bool FindFileLinkPath( tnstr &fullpath, const tchar *filename, const tchar *dicpath, const tchar *dicflink, const tchar *commonpath, bool extra);
bool GetRelFileLinkPath( tnstr &relpath, const tchar *filename, const tchar *dicpath, const tchar *dicflink, const tchar *commonpath, bool inc_drive=false);

#endif	// __ASSEXEC_H

