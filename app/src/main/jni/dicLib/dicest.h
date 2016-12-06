//---------------------------------------------------------------------------

#ifndef dicestH
#define dicestH
//---------------------------------------------------------------------------

#include "dicextdb.h"

struct ESTDB;
struct ESTDOC;
struct ESTCOND;
class PdicBase;

// Estraier DB error codes.
//TODO: EstraierÇÃerror codeÇ∆Ç‘Ç¬Ç©ÇÁÇ»Ç¢ÇÊÇ§Ç…í≤êÆÇ∑ÇÈÇ±Ç∆
enum EESTDB {
	EESTDB_NONE = 0,
	EESTDB_DLL,
	EESTDB_NOT_OPENED,
	EESTDB_ALREADY_OPENED,
	EESTDB_NO_URI,
//	EESTDB_PARAMETER,
	EESTDB_MEMORY,
};

class TEstraierDB : public TPdicExtDBBase {
typedef TPdicExtDBBase super;
protected:
	ESTDB *db;
	int ecode;
	ESTCOND *cond;
	int resnum;
	int *result;
	int result_index;
	PdicBase *dic;
public:
	TEstraierDB(PdicBase *dic);
	~TEstraierDB();
	
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


	// Specific methods //
	int GetGeneration();
	
// Internal methods //
protected:
	bool LoadDll();
	void UnloadDll();
	tnstr GetDBDir(bool create_dir, const tchar *dicname=NULL);
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

#endif

