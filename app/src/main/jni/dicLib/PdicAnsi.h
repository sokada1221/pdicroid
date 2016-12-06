//---------------------------------------------------------------------------

#ifndef pdicansiH
#define pdicansiH
//---------------------------------------------------------------------------
//#include "pdcdefs.h"

#undef LPMSTR
#undef MCHAR
#undef LPCMSTR

#define	LPMSTR	char *
#define	MCHAR	char
#define	LPCMSTR	const char *

#define	DEFINED_ANSI_DLLDIC

#include "DllDicT.h"
#include "CharT.h"

class TAnsiObjData : public TObjDataT<char> {
protected:
	PDCOLEDATAPARAMA oleparam;	// OLEéûÇÃÇ›óLå¯
	DWORD aspect;				// OLEéûÇÃÇ›óLå¯
public:
	virtual void *GetOleParamImpl()
		{ return &oleparam; }
	virtual PDCOLEDATAPARAMA *GetOleParamA()
		{ return &oleparam; }
	virtual DWORD *GetAspectPtr()
		{ return &aspect; }
};

#define	PDCANSIOLEDATAPARAM	PDCOLEDATAPARAMA

#if sizeof(PDCANSIOLEDATAPARAM)!=48
#error
#endif

class CJapa : public JapaT<char> {
public:
	CChar japa;
	CChar pron;
	CChar exp;
};

struct PDRECORDPARAM;
struct PDCDATA;

class PdicAnsi : public DllDictionaryT<char> { //pdutyópÇÃPDIC3é´èë
typedef DllDictionaryT<char> inherited;
typedef DllDictionaryT<char> super;
protected:
	bool OleLoaded;
	virtual int getWord( CChar &word){return -1;}
	virtual int getJapa( CJapa &japa ){return -1;}
public:
	PdicAnsi( );
	~PdicAnsi();
	void InitReadPare( MergeMode mergemode, int TransFlags );
	int readPare( CChar &word, CJapa &japa );
	int record(const char *word, CJapa &japa );
public:
	void SetAllSearch( const char *word, SrchMode mode );
	void SetDivRatio( int ratio );
//	int NextAllSearch( TNChar &word, CJapa *japa);
	int ResetAttr( const char *word, uchar bit );
	int EraseWordAS( );
	int bsearch( const char *word );
	void getfjapa( CJapa &j );
	const char *getfword( );
protected:
	virtual bool Load( );
//	TObjDataT<T> *_CreateObjData() = 0;
	virtual TObjDataT<char> *_CreateObjData()
		{ return new TAnsiObjData(); }
	TAnsiObjData *CreateObjData()
		{ return (TAnsiObjData*)_CreateObjData(); }
protected:
	virtual bool _Open( HPDC , const tchar *filename, bool readonly );
	virtual bool _Create( HPDC , const tchar *filename, PDCREATEINFO *info );
	bool ConvertObject(CJapa &j);

	// Helpers //
public:
	bool ConvertObject(const CJapa &j, class Japa &japa);
	class JLOle *GetObjectOle(TAnsiObjData &obj, int id);
};

bool ConvertOleDataParam(const PDCOLEDATAPARAMA &paramA, PDCOLEDATAPARAMW &paramW);
bool IsPdicAnsi( const tchar *filename, int *EndianMode );

extern const tchar *DllNamePdicAnsi;

#endif

