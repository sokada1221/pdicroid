//---------------------------------------------------------------------------

#include "tnlib.h"
#pragma hdrstop

#include "DllDic.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

DllDictionary::DllDictionary()
{
	hDll = NULL;
	hPdc = NULL;
	UseObjectData = true;	// object data‚Ì“Ç‚ÝŽæ‚è‚ðs‚¤
}
DllDictionary::~DllDictionary()
{
	if ( hDll ){
		Close();
		FreeLibrary( hDll );
	}
}
bool DllDictionary::Load( const tchar *dllname )
{
	if ( !hDll ){
		hDll = LoadLibrary( dllname );
		if ( !hDll ){
			error = 16;
			return false;
		}
		fnPDGetNextSearch =
			(FNPDGetNextSearch)GetProcAddress( hDll, "PDGetNextSearch" );
		GETPROC( PDGetPercent, "PDGetPercent" );
		GETPROC( PDGetDataBinary, "PDGetDataBinary" );
		GETPROC( PDGetLengthBinary, "PDGetLengthBinary");
		GETPROC( PDGetExtraData, "PDGetExtraData");
		GETPROC( PDGetObjectType, "PDGetObjectType");
		GETPROC( PDGetObjectId, "PDGetObjectId");
		GETPROC( PDGetObjTitle, "PDGetObjTitle");
		GETPROC( PDGetFileLinkName, "PDGetFileLinkName");
		GETPROC( PDSetFileLinkName, "PDSetFileLinkName");
		GETPROC( PDGetOleObjParam, "PDGetOleObjParam");
		GETPROC( PDGetOleObjData, "PDGetOleObjData");
		GETPROC( PDSetOleObj, "PDSetOleObj");
		GETPROC( PDCreateObject, "PDCreateObject");
		GETPROC( PDGetEPWingData, "PDGetEPWingData");
	}
	return true;
}
void DllDictionary::Close( )
{
	if ( hPdc ){
		FNEXEC1( PDClose, "PDClose", hPdc );
		FNEXEC1( PDDeleteHandle, "PDDeleteHandle", hPdc );
		hPdc = NULL;
	}
}
int DllDictionary::Open(const tchar *fname, int mode)
{
	Close();
	if ( !Load() ) return -1;

	if ( !hPdc ){
		FNEXEC0R( hPdc, PDCreateHandle, "PDCreateHandle" );
	}

	inherited::Open( fname, mode );

	PDCREATEINFO pdinfo;

	switch ( mode & FOM_OPENMODEMASK ){
		case FOM_READONLY:
			if ( !_Open( hPdc, fname, true ) ){
				return -1;
			}
			break;
		case FOM_CREATE:
			memset( &pdinfo, 0, sizeof(pdinfo) );
			if ( !_Create( hPdc, fname, &pdinfo ) ){
				return -1;
			}
			break;
		case FOM_WRITE:
			if ( !_Open( hPdc, fname, false ) ){
				return -1;
			}
			break;
	}
	InitReadPare( MRG_APPCR, TRS_ALL );
	GETPROC( PDQueryRecord, "PDQueryRecord" );
	return 0;
}
bool DllDictionary::_Open( HPDC hpdc, const tchar *filename, bool readonly )
{
	FNPDOpen fnPDOpen;
	GETPROC( PDOpen, "PDOpen" );
	return fnPDOpen( hpdc, filename, readonly );
}
bool DllDictionary::_Create( HPDC hpdc, const tchar *filename, PDCREATEINFO *info )
{
	FNPDCreate fnPDCreate;
	GETPROC( PDCreate, "PDCreate" );
	return fnPDCreate( hpdc, filename, info );
}
#ifdef GUI
int DllDictionary::CanOpen( class TWindow *, const tchar *fname, int mode )
{
	if ( ( mode & FOM_OPENMODEMASK ) == FOM_READONLY ){
		if ( fexist( fname ) ){
			return 0;
		} else {
			setError( 1 );
			return -1;
		}
	} else {
#if 0
		if ( fexist( fname ) ){
			int c;
			switch (c){
				case ID_MERGE:
					if ( Open(fname, FOM_WRITE) == -1){
						if ( parent )
							ErrorMessage( parent->HWindow, GetErrorCode(), fname );
						return -1;
					}
					break;
				case ID_NEW:
					if ( parent ){
						if ( CreateNewDictionary( parent, fname ) == -1)
							return -1;
					} else return -1;
					break;
				default:
					return -2;
			}
		} else {
			if ( parent ){
				if ( CreateNewDictionary( parent, fname ) == -1)
					return -1;
			} else return -1;
		}
#endif
		Close( );
	}
	return 0;
}
#endif
void DllDictionary::SetFastMode( )
{
	FNPDSetFastMode fnPDSetFastMode;
	GETPROC( PDSetFastMode, "PDSetFastMode" );
	fnPDSetFastMode( hPdc, true );
}
long DllDictionary::length( )
{
	return 0;
}
int DllDictionary::percent( )
{
	return fnPDGetPercent( hPdc );
}
int DllDictionary::_getError( )
{
	if ( hPdc ){
		FNPDGetErrorCode fnPDGetErrorCode;
		GETPROC( PDGetErrorCode, "PDGetErrorCode" );
		return fnPDGetErrorCode( hPdc );
	}
	return error;
}
bool DllDictionary::CheckDictionary( const tchar *filename, FNDicCheckCallBack CallBack, int UserData )
{
	if ( !Load() )
		return false;
	HPDC hPdc;
	FNEXEC0R( hPdc, PDCreateHandle, "PDCreateHandle" );

	PDDICCHECKPARAM dcp;
	FNPDDicCheck fnPDDicCheck;
	memset( &dcp, 0, sizeof(dcp) );
	dcp.CallBack = CallBack;
	dcp.UserData = UserData;
	GETPROC( PDDicCheck, "PDDicCheck" );
	fnPDDicCheck( hPdc, filename, &dcp );

	FNEXEC1( PDDeleteHandle, "PDDeleteHandle", hPdc );

	return true;
}
bool DllDictionary::OptimizeDictionary( const tchar *filename, FNDicCheckCallBack CallBack, int UserData )
{
	if ( !Load() )
		return false;
	HPDC hPdc;
	FNEXEC0R( hPdc, PDCreateHandle, "PDCreateHandle" );

	PDDICCHECKPARAM dcp;
	FNPDOptimize fnPDOptimize;
	memset( &dcp, 0, sizeof(dcp) );
	dcp.CallBack = CallBack;
	dcp.UserData = UserData;
	GETPROC( PDOptimize, "PDOptimize" );
	fnPDOptimize( hPdc, filename, &dcp );

	FNEXEC1( PDDeleteHandle, "PDDeleteHandle", hPdc );

	return true;
}
bool DllDictionary::GetHeaderInfo( const tchar *filename, PDHEADERINFO *header )
{
	if ( !Load() ) return false;
	FNPDGetHeaderInfo fnPDGetHeaderInfo;
	GETPROC( PDGetHeaderInfo, "PDGetHeaderInfo" );
	return fnPDGetHeaderInfo( filename, header );
}
const tchar *DllDictionary::GetErrorMessage( int error )
{
	if ( !Load() ) return NULL;
	FNPDGetErrorMessage fnPDGetErrorMessage;
	GETPROC( PDGetErrorMessage, "PDGetErrorMessage" );
	return fnPDGetErrorMessage( error );
}

