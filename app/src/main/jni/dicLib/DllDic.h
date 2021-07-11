//---------------------------------------------------------------------------

#ifndef DllDicH
#define DllDicH
//---------------------------------------------------------------------------

#include "pdc600.h"	// It's a tricky way.
					// ansi/unicodeでchar/wchar_tの違いはあるが、
					// それ以外の扱いは差異がないためここでincludeしても問題はない
#include "Dictionary.h"

// DllDictionaryにLPMSTR系のデータを含んではいけない！！

class DllDictionary : public TDllDictionaryBase {
typedef TDllDictionaryBase inherited;
protected:
	// DLL access
	HINSTANCE hDll;
	HPDC hPdc;
	FNPDGetNextSearch fnPDGetNextSearch;
	FNPDQueryRecord fnPDQueryRecord;
	FNPDGetPercent fnPDGetPercent;
	FNPDGetLengthBinary fnPDGetLengthBinary;
	FNPDGetDataBinary fnPDGetDataBinary;
	FNPDGetExtraData fnPDGetExtraData;
	FNPDGetObjectType fnPDGetObjectType;
	FNPDGetObjectId fnPDGetObjectId;
	FNPDGetObjTitle fnPDGetObjTitle;
	FNPDGetFileLinkName fnPDGetFileLinkName;
	FNPDSetFileLinkName fnPDSetFileLinkName;
	FNPDGetOleObjParam fnPDGetOleObjParam;
	FNPDGetOleObjData fnPDGetOleObjData;
	FNPDSetOleObj fnPDSetOleObj;
	FNPDCreateObject fnPDCreateObject;
	FNPDGetEPWingData fnPDGetEPWingData;

	FNPDGetExtHeaderStr fnPDGetExtHeaderStr;
	FNPDSetExtHeaderStr fnPDSetExtHeaderStr;
	FNPDDeleteExtHeader fnPDDeleteExtHeader;

	FNPDCreateString fnPDCreateString;
	FNPDDeleteString fnPDDeleteString;
	FNPDSetString fnPDSetString;
	FNPDGetString fnPDGetString;
	FNPDGetStringLength fnPDGetStringLength;

	PDRECORDPARAM prp;
	bool UseObjectData;

	PDCDATA pdcdata;
public:
	DllDictionary();
	virtual ~DllDictionary();
	virtual bool Load( ) = 0;
	int Open(const tchar *fname, int mode);
	int CanOpen( class TWindow *, const tchar *fname, int mode );
	virtual void Close( );
	virtual void SetFastMode( );

	bool IsValid( )
		{ return Load( ); }

	long length( );
	int percent( );
	int _getError( );
	void setError( int _error );
//    int GetErrorCode()
//    	{ return error; }
	virtual const tchar *GetErrorMessage( int error );

	bool CheckDictionary( const tchar *filename, FNDicCheckCallBack CallBack, int UserData );
	bool OptimizeDictionary( const tchar *filename, FNDicCheckCallBack CallBack, int UserData );

	bool GetHeaderInfo( const tchar *filename, PDHEADERINFO *header );
	virtual int sizeOfChar() const = 0;
protected:
	bool Load( const tchar *dllname );
	virtual bool _Open( HPDC , const tchar *filename, bool readonly );
	virtual bool _Create( HPDC , const tchar *filename, PDCREATEINFO *info );
};

#endif

