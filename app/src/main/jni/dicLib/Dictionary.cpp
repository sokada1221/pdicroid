//---------------------------------------------------------------------------

#include "tnlib.h"
#pragma hdrstop

#include "Dictionary.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

TDllDictionaryBase::TDllDictionaryBase()
{
}
int TDllDictionaryBase::Open( const tchar *fname, int /* mode */ )
{
#if DICDBG1
	readcount = 0;
	maxcount = 0;
#endif
	app = mod = def = 0L;
	error = 0;
#if defined(PDICW) && !defined(DLL) && !defined(NOGUI)
	if ( IsWindow( (HWND)fname ) )
		name = _T("");
	else
#endif
	name = fname;
	return 0;
}
int TDllDictionaryBase::GetErrorCode( )
{
	return _getError( );
}

