#include "tnlib.h"
#pragma	hdrstop
#define	ShlObjHPP	// to avoid compile error with shlobj.h
#ifdef USE_VCL
#include <vcl.h>
#endif
#include <shlobj.h>

#ifdef USE_VCL
#include "TntStdCtrls.hpp"
#endif
#ifndef WINCE
#include	<cderr.h>
#endif
#include "pdconfig.h"
#include	"browse.h"
#include	"dicdlg.h"
#include "pdstr.h"
#include "tid.h"
#include "winmsg.h"
#include	"helps.h"
#include "wpdcom.h"

const tchar *BrowseInitialDir = NULL;

bool BrowseProc( HWND hwnd, int type, tnstr &tfile, int &nFileOffset, int msgno, int help, bool bMulti, bool bSave=false );

bool BrowseProc( HWND hwnd, int type, tnstr &tfile, int &nFileOffset, int msgno, int help, bool bMulti, bool bSave )
{
	tnstr path;
	int fi = 4;
	switch ( type & DT_MASK ){
		case DT_PDIC:
			path.set( StrDic );
			fi = 0;
			break;
		case DT_PDICTEXT:
		case DT_WX2TEXT:
		case DT_LEVEL:
		case DT_EXTPDICTEXT:
		case DT_FENG5:
			path.set( StrTxt );
			fi = 1;
			break;
		case DT_TEXT:
			path.set( StrTxt );
			fi = 14;
			break;
		case DT_EXE:
			path.set( StrExe );
			fi = 5;
			break;
#ifndef WINCE
		case DT_USER:
			path.set( StrAll );
			fi = 3;			// は大丈夫？？
			break;
		case DT_DLL:
			path.set( tfile );	// ちょっと違うので注意
			fi = 6;
			break;
		case DT_CSV:
			path.set( StrCSV );
			fi = 2;
			break;
		case DT_INF:
			path.set( StrInf );
			fi = 10;
			break;
		case DT_WAV:
			path.set( StrWav );
			fi = 11;
			break;
		case DT_EPWING:
			path.set( StrEPWing );
			fi = 12;
			break;
		case DT_INI:
			path.set( StrIni );
			fi = 13;
			break;
#endif
	}
	// find extension
	tnstr DefExt;
	{
		const tchar *p = _tcschr(path, '.');
		if (p){
			// Found the extension.
			p++;
			if (!_tcschr(p, '*')){
				// if not wild card.
				DefExt = p;
			}
		}
	}
	if ( !tfile.empty() ) path.set( tfile );	// ファイル名がすでに入っている場合
	CommFileDialog *dlg;
#if defined(PDICW) && !defined(SML)
	if ( ( type & DT_MASK ) == DT_PDIC && !(type & DT_NODICLIST) ){
		dlg = new DicFileOpenDialog( hwnd, path, msgno ? _LT(msgno) : NULL, bSave );
	} else
#endif
	if (bSave){
		dlg = new CommFileSaveDialog( hwnd, path, msgno ? _LT(msgno) : NULL );
	} else {
		dlg = new CommFileOpenDialog( hwnd, path, msgno ? _LT(msgno) : NULL );
		// ファイル名順に表示されるのに、getしたときはランダム順(?)になってしまうため止めた！！
	}
	dlg->ofn.Flags |= OFN_EXPLORER|OFN_ENABLESIZING;
#ifndef WINCE
	dlg->EnableDragDrop( );
#endif
#if defined(PDICW) && !defined(WINCE)
	if ( help ){
		tnstr hpath;
		GetHelpFileName( hpath );
		dlg->UseHelp( hpath, HELP_CONTEXT, help );
	}
#endif
#ifndef WINCE
	if ( bMulti ){
#ifdef WIN32
		dlg->AllowMultiSelect( 8192 );	// GetOpenFileName関数の制限？
#else
		dlg->AllowMultiSelect( 2048 );	// GetOpenFileName関数の制限？
#endif
	}
#endif
	if ( fi == 5 ){
		dlg->SetFilter( _LT( IDS_FILTER2 ), '|' );
		dlg->SetFilterIndex( 0 );
#ifndef WINCE
	} else if ( fi == 6 ){
		dlg->SetFilter( _LT( IDS_FILTER3 ), '|' );
		dlg->SetFilterIndex( 0 );
	} else if ( fi == 10 ){
		dlg->SetFilter( _LT( IDS_FILTER4 ), '|' );
		dlg->SetFilterIndex( 0 );
	} else if ( fi ==11 ){
		dlg->SetFilter( _LT( IDS_FILTER5 ), '|' );
		dlg->SetFilterIndex( 0 );
	} else if ( fi == 12 ){
		dlg->SetFilter( _LT( IDS_FILTER6 ), '|' );
		dlg->SetFilterIndex( 0 );
	} else if ( fi == 13 ){
		dlg->SetFilter( _LT( IDS_FILTER7 ), '|' );
		dlg->SetFilterIndex( 0 );
#endif
	} else if ( fi == 14 ){
		dlg->SetFilter( _LT( IDS_FILTER9 ), '|' );
		dlg->SetFilterIndex( 0 );
	} else {
		dlg->SetFilter( _LT( IDS_FILTER1 ), '|' );
		dlg->SetFilterIndex( fi );
	}
	if ( type & DT_NOEXISTCHECK ){
#ifdef WINCE
		dlg->ofn.Flags &= ~(OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST);	// これでいいのかわからない？
#else
//		dlg->ofn.Flags &= ~(OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST);
		dlg->ofn.Flags |= OFN_READONLY|OFN_NOVALIDATE;
#endif
	}
#ifndef SMALL
	if ( BrowseInitialDir )
		dlg->SetInitialDir( BrowseInitialDir );
#endif
	if (!DefExt.empty()){
		dlg->ofn.lpstrDefExt = DefExt;
	}
	int r = dlg->Execute();
	nFileOffset = dlg->ofn.nFileOffset;
	delete dlg;
	if ( r != TRUE ){
#ifndef WINCE
		DWORD err = CommDlgExtendedError( );
		if ( err == FNERR_BUFFERTOOSMALL ){
			// 不正なドライブを選択して、cancelをするとここにくる！！(Windowsのバグ！！)
			MsgBox( hwnd, _LT(MSG_BUFFERTOOSMALL), MsgError, MB_OK | MB_ICONSTOP );
		} else
		if (err){
			tnstr msg = tnsprintf(_t("Browse Dialog Box Error : %d\n"), err);
			msg += FormatMessage(err);
			MsgBox( hwnd, msg, MsgError, MB_OK );
		}
#endif
		return false;
	}
	tfile.set( path );
	return true;
}

// 複数選択用
bool BrowseProc( HWND hwnd, int type, tnstr_vec &tfile, int msgno, int help )
{
	tnstr path;
	int nFileOffset;
	if ( !BrowseProc( hwnd, type, path, nFileOffset, msgno, help, true ) )
		return false;
	GetCommFiles( path, tfile, '|', nFileOffset );
	return true;
}

// 単一選択用
bool BrowseProc( HWND hwnd, int type, tnstr &tfile, int msgno, int help, bool save )
{
	int nFileOffset;
	return BrowseProc( hwnd, type, tfile, nFileOffset, msgno, help, false, save);
}

#ifdef USE_VCL
bool BrowseProc( HWND hwnd, int type, TTntComboBox *tfile, int msgno, int help, bool bMulti)
{
	tnstr path;
	int nFileOffset;
	if ( BrowseProc( hwnd, type, path, nFileOffset, msgno, help, bMulti ) ){
		tfile->Text = toString(path);
		return true;
	}
	return false;
}
#endif

#ifndef WINCE
// フォルダー選択
bool BrowseFolderProc( HWND hwnd, tnstr &path, int msgno )
{
	InitShell();

	BROWSEINFO bi;
	memset(&bi,0,sizeof(bi));
	bi.hwndOwner = hwnd;
#if 0	// mendo kuse-
	char *idl_buf = new char[sizeof(ITEMIDLIST)+_tcslen(path)+1];
	ITEMIDLIST &idl = *(ITEMIDLIST*)idl_buf;
	idl.mkid.cb = sizeof(idl.mkid.cb)+_tcsbyte1(path);
	_tcscpy(idl.mkid.abID, path);
	bi.pidlRoot = &idl;
#endif
	tchar *dispname = new tchar[MAXPATH+1];
	bi.pszDisplayName = dispname;
	if ( msgno )
		bi.lpszTitle = _LT(msgno);
//	bi.ulFlags = BIF_EDITBOX;
	LPITEMIDLIST item = SHBrowseForFolder(&bi);
	delete[] dispname;
	if (!item)
		return false;
	tchar *buf = new tchar[MAXPATH+1];
	bool r = SHGetPathFromIDList( item, buf );
	CoTaskMemFree(item);
	if (r){
		path = buf;
	}
	delete[] buf;
	return r;
}
#endif

#ifdef WINPPC
#include "projects.h"
// createdir : \My Documents が存在しない場合作成する
int GetStorages( tnstr_vec &stgs, bool createdir )
{
	int iNumOfFlashCard = 0;            // tTotal number of storage                                           // cards.
	bool bContinue = TRUE;              // If TRUE, continue searching.
										// If FALSE, stop searching.
	TCHAR szRootDirPath[MAX_PATH];      // Root directory path of 
										// storage cards.
	HANDLE hFlashCard;               // Search handle for storage cards.
	WIN32_FIND_DATA wfd; // Structure for storing storage
										// card information temporarily.
	hFlashCard = FindFirstFlashCard (&wfd);
	if (hFlashCard == INVALID_HANDLE_VALUE)
	{
	   return 0;	// no storages
	}
	while (bContinue) 
	{
		iNumOfFlashCard++;
	   // Create a "My Documents" directory on the card, if it does not 
	   // exist.
		_tcscpy(szRootDirPath, _T("\\"));
		_tcscat(szRootDirPath, wfd.cFileName );
		stgs.add( wfd.cFileName );
		if (createdir){
			_tcscat (szRootDirPath, _T(PATH_MYDOCUMENTS) );
			CreateDirectory (szRootDirPath, NULL);
		}
		// Search for the next storage card.
		bContinue = FindNextFlashCard(hFlashCard, &wfd);
	}
	FindClose (hFlashCard);                    // Close the search handle.
	return iNumOfFlashCard;
}
#if 0
HANDLE FindFirstFlashCard( LPWIN32_FIND_DATA )
{
	return NULL;
}
bool FindNextFlashCard( HANDLE h, LPWIN32_FIND_DATA )
{
	return FALSE;
}
#endif
#endif	// WINCE

