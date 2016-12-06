#ifndef __dichbt_h
#define	__dichbt_h

#include "dicextdb.h"

// THBDic DB error codes.
enum EHBTDB {
	EHBTDB_NONE = 0,
	EHBTDB_DLL,
	EHBTDB_NOT_OPENED,
	EHBTDB_ALREADY_OPENED,
	EHBTDB_NO_URI,
//	EHBTDB_PARAMETER,
	EHBTDB_MEMORY,
};

#define	TP_WORD		0x10
#define	TP_JAPA		0x01
#define	TP_EXP		0x02

///////////////////////////////////////////////////
///	PdicHbt class
///////////////////////////////////////////////////
class THBDic;
class TPdicHbt : public TPdicExtDBBase {
typedef TPdicExtDBBase super;
protected:
	PdicBase *dic;
	THBDic *db;
	int ecode;
	struct HBTDataElem **DataElems;
	int MaxDataElems;
	int DataElemCount;
	int DataElemIndex;
	int TakePart;	// TP_xxxx
public:
	TPdicHbt(PdicBase *dic);
	virtual ~TPdicHbt();

	__override bool Open( const tchar * fname, bool _readonly );
	__override void Close( );
	__override bool Record(const tchar *word, const Japa &japa);
	__override bool Erase(const tchar *word);
	__override bool Update(const tchar *word, const Japa &japa);
	__override bool SetAllSearch( const tchar *word, SrchMode mode, GENERICREXP *jre, AllSearchParam *all );
	__override int NextAllSearch( tnstr &word, Japa *japa, AllSearchParam *all);
	__override int PrevAllSearch( tnstr &word, Japa *japa, AllSearchParam *all);
	__override bool IsMySearchMode(SrchMode mode);

	__override tnstr GetDBPath(const tchar *dicname=NULL);

protected:
	tnstr GetDBName(bool create_dir, const tchar *dicname=NULL);
	bool OpenDB(const tchar *fname, bool readonly);
	void CloseDB();
	bool RecordItem(const char *type, const tchar *key, const tchar *text);
	bool DeleteItem(const char *type, const tchar *key);
	void ResetResult();
	int FoundProc(tnstr &word, Japa *japa, AllSearchParam &all);

	// Auxiary information
	bool WriteInfo(const char *key, const char *value);
	tnstrA ReadInfo(const char *key);
	bool EraseInfo(const char *key);
};

#endif	/* __dichbt_h */

