#include	<windows.h>
#pragma	hdrstop
#include "tndefs.h"
#include "tnstr.h"

#ifdef WIN32

bool GetTemporaryFileName( const tchar *prefix, tnstr &filepath )
{
	tchar tempname[ MAX_PATH ];
	tchar pathbuf[ MAX_PATH ];
	if ( !GetTempPath( MAX_PATH, pathbuf ) ){
		// エラー
		return false;
	}
	if ( !GetTempFileName( pathbuf, prefix, 0, tempname ) ){
		// エラー
		return false;
	}
    filepath.set( tempname );
	return true;
}

#else
#endif


//TODO: obsolete
void ShowItemWindows( HWND hwnd, BOOL f, int *ids )
{
	for ( ;ids[0];ids++ ){
		ShowWindow( GetDlgItem( hwnd, ids[0] ), f ? SW_SHOW : SW_HIDE );
	}
}
