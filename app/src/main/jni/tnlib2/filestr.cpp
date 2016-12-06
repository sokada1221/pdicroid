#include "stdafx.h"
#ifdef _Windows
#include <windows.h>
#endif
#pragma hdrstop
#include "tndefs.h"
#include "filestr.h"


// pathからファイル名の先頭のアドレスを得る
tchar *GetFileNamePtr( const tchar *path )
{
	if (!path)
		return NULL;

	const tchar *p;
	const tchar *delim1 = NULL;
	const tchar *delim2 = NULL;

	for ( p=path; *p ; ){
		tchar c = *p;
		if ( c=='\\' || c=='/'){
			delim1 = ++p;
		} else if ( c == ':' ){
			delim2 = ++p;
		} else {
			p = NEXT_CHAR(p);
		}
	}

	return (tchar*)( delim1 ? delim1 : ( delim2 ? delim2 : path ) );
}

const tchar *GetFileExtPtr(const tchar *filename)
{
	if (!filename)
		return NULL;

	const tchar *ext = NULL;
	while ( *filename ){
		if ( *filename == '.' ){
			ext = filename+1;
		} else if ( *filename == '\\' || *filename=='/'){
			ext = NULL;
		}
		NEXT_CHAR( filename );
	}
	return (tchar*)ext;
}
