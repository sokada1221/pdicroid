#include	<windows.h>
#pragma	hdrstop
#include	<memory.h>
#include	"tnwinx.h"
#include	"jreusr.h"
#include	"jrectrl.h"


//	JRE.DLL コントロール・インターフェース部

HINSTANCE JREObject::hinstJRE = NULL;	// JREインスタンスハンドル

LPJRE2OPEN JREObject::lpfnJre2Open = NULL;
LPJRE2CLOSE JREObject::lpfnJre2Close = NULL;
LPJRE2GETMATCHINFO JREObject::lpfnJre2GetMatchInfo = NULL;
LPJRE2COMPILE JREObject::lpfnJre2Compile = NULL;

LPFUZZY_OPTIONDIALOG JREObject::lpfnFuzzy_OptionDialog;
LPFUZZY_GETFUZZYDATAINJRE JREObject::lpfnFuzzy_GetFuzzyDataInJre;

int JREObject::nInstance = 0;
BOOL JREObject::bOpened = FALSE;

const tchar *JREObject::DllName = _T("HMJRE.DLL");

//	返り値
//		TRUE		: 正常
//		FALSE		: 異常
JREObject::JREObject( )
{
}

JREObject::~JREObject()
{
	Close( );
}


BOOL JREObject::Open( )
{
	// 構造体初期化
	memset( &jre2, 0, sizeof( JRE2 ) );
	jre2.dwSize = sizeof( JRE2 );

	if ( !hinstJRE ){
#ifndef WINCE
		int oldErr = SetErrorMode( SEM_NOOPENFILEERRORBOX );
#endif
		hinstJRE = LoadLibrary( DllName );
#ifndef WINCE
		SetErrorMode( oldErr );
#endif
		if ( !IsValidInstance(hinstJRE) ){
			hinstJRE = NULL;
			return FALSE;
		}
	}

	if ( nInstance++ > 0 )
		return TRUE;

	// JRE Open,CloseのGetProcAddress
	(FARPROC)lpfnJre2Open = GetProcAddress( hinstJRE, "Jre2Open" );
	(FARPROC)lpfnJre2Close = GetProcAddress( hinstJRE, "Jre2Close" );
	(FARPROC)lpfnJre2GetMatchInfo = GetProcAddress( hinstJRE, "Jre2GetMatchInfo" );
	(FARPROC)lpfnJre2Compile = GetProcAddress( hinstJRE, "Jre2Compile" );
	(FARPROC)lpfnFuzzy_OptionDialog = GetProcAddress(hinstJRE, "Fuzzy_OptionDialog");
	(FARPROC)lpfnFuzzy_GetFuzzyDataInJre = GetProcAddress(hinstJRE, "Fuzzy_GetFuzzyDataInJre");
	if ( !lpfnJre2Open
		|| !lpfnJre2Close
		|| !lpfnJre2GetMatchInfo
		|| !lpfnJre2Compile
		|| !lpfnFuzzy_OptionDialog
		|| !lpfnFuzzy_GetFuzzyDataInJre
		){
		Close();
		return FALSE;
	}

	return TRUE;
}

BOOL JREObject::SubOpen( )
{
	// JREオープン
	if ( !lpfnJre2Open( &jre2 ) ){
		return FALSE;
	}
	bOpened = TRUE;
	return TRUE;
}

void JREObject::SubClose( )
{
	if ( !bOpened )
		return;

	if ( lpfnJre2Close ){
		lpfnJre2Close( &jre2 );
	}
	bOpened = FALSE;
}

void JREObject::Close( )
{
	if ( !hinstJRE )
		return;

	SubClose( );

	if ( --nInstance > 0 )
		return;

	FreeLibrary( hinstJRE );
	hinstJRE = NULL;
}


WORD JREObject::GetVersion( )
{
	LPJREGETVERSION lpfnJreGetVersion = (LPJREGETVERSION)GetProcAddress( hinstJRE, "JreGetVersion" );
	if ( lpfnJreGetVersion ){
		return lpfnJreGetVersion();
	} else {
		return 0;
	}
}


BOOL JREObject::CompileImpl( const tchar *patern )
{
	if ( !SubOpen( ) )
		return FALSE;
	jre2.wTranslate = (WORD)( (srchflag & GRXP_FUZZY) == GRXP_FUZZY );
	SetupIgnoreCase();
	return lpfnJre2Compile( &jre2, (LPSTR)patern );
}
void JREObject::SetupIgnoreCase()
{
	// 大文字・小文字同一視処理
	if (!jre2.wTranslate && !(srchflag & GRXP_IGNORECASE))
		return;

	// あいまい検索または、同一視のどちらからがある場合は
	// 必ず以下の処理が必要

	lpfnJre2Compile( &jre2, "" );	// dummy
									// Jre2Compile内で、JREFUZZYDATAを生成させるため
	JREFUZZYDATA *fd = lpfnFuzzy_GetFuzzyDataInJre(&jre2);
	if (srchflag & GRXP_IGNORECASE)
		fd->flags |= FUZZYOPTION_NOCASE;
	else
		fd->flags &= ~FUZZYOPTION_NOCASE;
}

BOOL JREObject::CompareImpl( const tchar *str )
{
	return lpfnJre2GetMatchInfo( &jre2, (LPSTR)str );
}

BOOL JREObject::Compare( const tchar *str, int startpos )
{
	jre2.nStart = startpos;
	return lpfnJre2GetMatchInfo( &jre2, (LPSTR)str );
}

BOOL JREObject::GetMatchImpl( int &len, int &loc )
{
	len = jre2.nLength;
	loc = jre2.nPosition;
	return TRUE;
}

int JREObject::GetErrorCode( )
{
	if ( !hinstJRE )
		return GRE_NOTREADY;

	switch ( jre2.nError ){
		case CantAllocate:
		case MemoryTooFew1:
		case MemoryTooFew2:
			return GRE_MEMORY;
		case ReTooLong:
			return GRE_TOOLONG;
		case TooComplex:
			return GRE_COMPLEX;
		case MismatchBracket:
			return GRE_BRACKET;
		case InvalidChClass:
			return GRE_CHARCLASS;
		case EscErr:
			return GRE_ESCAPE;
		case Unknown:
			return GRE_UNEXPECTED;
		case NoReString:
			return GRE_NOSTRINGS;
		case IncorrectUsing:
		case ReNotExist:
			return GRE_APPBUG;
		case InternalErr:
			return GRE_DLLBUG;
		case UsrAbort:
			return GRE_NORMAL;	// 起こり得ない？
		case OldVersion:
			return GRE_VERSION;
		default:
			return GRE_UNEXPECTED;
	}
}

void JREObject::OptionDialog(HWND parent)
{
	if ( !hinstJRE || !lpfnFuzzy_OptionDialog)
		return;
	lpfnFuzzy_OptionDialog(parent, 0);
}
