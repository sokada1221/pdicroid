//---------------------------------------------------------------------------
#ifndef pdicuniH
#define pdicuniH
//---------------------------------------------------------------------------

//#include "pdicdefs.h"

//#include "utydefs.h"
//#include "japa.h"
// TEST—pDictionary class
//#include "utydic.h"

#ifndef LPMSTR
#define	LPMSTR	wchar_t *
#endif
#ifndef MCHAR
#define	MCHAR	wchar_t
#endif
#ifndef LPCMSTR
#define	LPCMSTR	const wchar_t *
#endif

#define	DEFINED_UNICODE_DLLDIC

#include "DllDicT.h"
#include "CharT.h"

class TUniObjData : public TObjDataT<wchar_t> {
protected:
	PDCOLEDATAPARAMW oleparam;	// OLEŽž‚Ì‚Ý—LŒø
	DWORD aspect;				// OLEŽž‚Ì‚Ý—LŒø
public:
	virtual void *GetOleParamImpl()
		{ return &oleparam; }
	virtual PDCOLEDATAPARAMW *GetOleParamW()
		{ return &oleparam; }
	virtual DWORD *GetAspectPtr()
		{ return &aspect; }
};

#define	PDCUNIOLEDATAPARAM	PDCOLEDATAPARAMW

#if sizeof(PDCUNIOLEDATAPARAM)!=64
#error
#endif

class WJapa : public JapaT<wchar_t> {
typedef JapaT<wchar_t> super;
public:
	WChar japa;
	WChar pron;
	WChar exp;
};

class PdicUni : public DllDictionaryT<wchar_t> {
typedef DllDictionaryT<wchar_t> inherited;
typedef DllDictionaryT<wchar_t> super;
protected:
	int EndianMode;	// 0:Little Endian 1:Big Endian
	int UTFMode;	// 0:UTF16 1:UTF8
	tnstr ErrorMessage;
protected:
	virtual int getWord( WChar &word){return -1;}
	virtual int getJapa( WJapa &japa ){return -1;}
public:
	PdicUni( int EndianMode, int utfmode);
	~PdicUni();
	virtual bool Load( );
	void InitReadPare( MergeMode mergemode, int TransFlags );
	int readPare( WChar &word, WJapa &japa );
	int readPare(tnstr &word, class Japa &japa);
	int record(const wchar_t *word, WJapa &japa );
	bool IsBigEndian()
		{ return EndianMode == 1; }
	virtual const tchar *GetErrorMessage( int error );

	void SetAllSearch( const wchar_t *word, SrchMode mode );
	void SetDivRatio( int ratio );
	int NextAllSearch( WChar &word, WJapa *japa);
	int ResetAttr( const wchar_t *word, uchar bit );
	int EraseWordAS( );
	int bsearch( const wchar_t *word );
	void getfjapa( WJapa &j );
	const wchar_t *getfword( );

	bool ReadExtHeader(const char *key, tnstr &str);
	bool WriteExtHeader(const char *key, const tchar *str);
	bool DeleteExtHeader(const char *key);
protected:
	virtual bool _Open( HPDC , const tchar *filename, bool readonly );
	virtual bool _Create( HPDC , const tchar *filename, PDCREATEINFO *info );
	int CreateObjectNumber();
	bool SetObject(Japa &Japa, const WJapa &j);
	class JLOle *GetObjectOle(TUniObjData &obj, int id);
public:
	virtual TUniObjData *CreateObjData()
		{ return new TUniObjData(); }
};

bool IsPdicUni( const tchar *filename, int *EndianMode, int *UTFMode );

#endif

