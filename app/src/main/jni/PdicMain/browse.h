#ifndef	__BROWSEE_H
#define	__BROWSEE_H

#include "dictype.h"

// 以下は自動インストール用
#define	DT_NONE		40	// None
#define	DT_COPY		41	// Copy
#define DT_MOVE		42	// Move
#define	DT_CREATE	43
#define	DT_HELP		44

#define	DT_MASK		0x7f


#define	DT_NODICLIST	0x80	// DT_PDICのBrowseProc()で"辞書グループから"を使わない
#define	DT_NOEXISTCHECK	0x100	// ファイルの存在チェックをしない
//#define	DT_USEFILENAME	0x200	// デフォルトでファイル名を表示する

//bool BrowseProc( HWND hwnd, int type, tnstr &tfile, int &nFileOffset, int msgno, int help, bool bMulti, bool bSave=false );
// use BrowseProc(..., tnstr_vec &)

bool BrowseProc( HWND hwnd, int type, tnstr_vec &tfile, int msgno=0, int help=0 );
bool BrowseProc( HWND hwnd, int type, tnstr &tfile, int msgno=0, int help=0, bool save=false );
bool BrowseProc( HWND hwnd, int type, class TTntComboBox *tfile, int titleno=0, int help=0, bool bMulti=false);

bool BrowseFolderProc( HWND hwnd, tnstr &path, int msgno );

// BrowseProc()でMultiの場合のファイル名取得方法
//	tnstr_vec files;
//	GetCommFiles( tfile, srcfiles );	// COMMDLGS.H

#define	MAX_STORAGECARDS	2	// the number of supported storage cards
int GetStorages( tnstr_vec &stgs, bool createdir );	// for WINCE

extern const tchar *BrowseInitialDir;

#endif	// __BROWSEE_H

