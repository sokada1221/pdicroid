#include	<windows.h>
#pragma	hdrstop
#include	"tnwinx.h"
#include	"rexpctrl.h"

HINSTANCE RexpObject::hinstRexp = NULL;
LPRECOMP RexpObject::lpfnReComp = NULL;
LPREEXEC RexpObject::lpfnReExec = NULL;
LPREIGNORECASE RexpObject::lpfnReIgnoreCase = NULL;
LPREERRNO RexpObject::lpfnReErrno = NULL;

int RexpObject::nInstance = 0;

RexpObject::RexpObject( )
{
	srchflag = GRXP_IGNORECASE;
	error = 0;
}


RexpObject::~RexpObject( )
{
	Close( );
}

BOOL RexpObject::Open( )
{
	if ( !hinstRexp ){
#ifndef WINCE
		int olderr = SetErrorMode( SEM_NOOPENFILEERRORBOX );
#endif
#ifdef WIN32
		hinstRexp = LoadLibrary( _T("REXP32.DLL") );
#else
		hinstRexp = LoadLibrary( _T("REXPDLL.DLL") );
#endif
#ifndef WINCE
		SetErrorMode( olderr );
#endif
		if ( !IsValidInstance(hinstRexp) ){
			hinstRexp = NULL;
			return FALSE;
		}
	}

	if ( nInstance++ > 0 )
		return TRUE;

	lpfnReComp = (LPRECOMP)GetProcAddress( hinstRexp, "ReComp" );
	lpfnReExec = (LPREEXEC)GetProcAddress( hinstRexp, "ReExec" );
	lpfnReIgnoreCase = (LPREIGNORECASE)GetProcAddress( hinstRexp, "ReIgnoreCase" );
	lpfnReErrno = (LPREERRNO)GetProcAddress( hinstRexp, "ReErrno" );
	if ( !lpfnReComp || !lpfnReExec || !lpfnReIgnoreCase || !lpfnReErrno ){
		FreeLibrary( hinstRexp );
		hinstRexp = NULL;
		return FALSE;
	}
	return TRUE;
}

void RexpObject::Close( )
{
	if ( !hinstRexp )
		return;

	if ( --nInstance > 0 )
		return;

	FreeLibrary( hinstRexp );
	hinstRexp = NULL;
}

BOOL RexpObject::CompileImpl( const tchar *patern )
{
	lpfnReIgnoreCase( srchflag & GRXP_IGNORECASE ? TRUE : FALSE );
	int r = lpfnReComp( patern );
	error = lpfnReErrno( );
	return r == NULL;
}

BOOL RexpObject::CompareImpl( const tchar *str )
{
	BOOL f = lpfnReExec( str ) != 0;
	error = lpfnReErrno( );
	return f;
}

int RexpObject::GetErrorCode( )
{
	if ( !hinstRexp )
		return GRE_NOTREADY;

	switch ( error ){

		case RE_ERR_NONE:
			return GRE_NORMAL;

		case RE_ERR_CCL_NUM:
			return GRE_TOOLONG;

		case RE_ERR_CCL_TERM:
			return GRE_CHARCLASS;

		case RE_ERR_PAR:
			return GRE_BRACKET;

		case RE_ERR_SYNTAX:
			return GRE_SYNTAX;

		case RE_ERR_MEMORY:
			return GRE_MEMORY;

		case RE_ERR_TABLE:
		case RE_ERR_DFA_STAT:
			return GRE_COMPLEX;

		case RE_ERR_UNKNOWN:
		default:
			return GRE_UNEXPECTED;
	}
}

#if defined(CMD32)
RexpObject::RexpObject( )
{
	srchflag = GRXP_IGNORECASE;
	error = 0;
}


RexpObject::~RexpObject( )
{
	Close( );
}
BOOL RexpObject::Open( )
{
	return FALSE;
}

void RexpObject::Close( )
{
}

BOOL RexpObject::Compile( const tchar *patern )
{
	return FALSE;
}

BOOL RexpObject::Compare( const tchar *str )
{
	return FALSE;
}

int RexpObject::GetErrorCode( )
{
	return GRE_NOTREADY;
}
#endif

