//
//		ÇfÇcÇhä«óù
//

#include	<windows.h>
#pragma	hdrstop
#include	<memory.h>
#include	<string.h>
#include <tchar.h>
#include "tndefs.h"
#include	"gdi.h"

LogFont::LogFont( )
{
	clear();
}

void LogFont::clear( void )
{
	memset( &lf, 0, sizeof( LOGFONT ) );
}

