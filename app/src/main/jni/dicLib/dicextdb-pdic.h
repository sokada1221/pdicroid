#ifndef __dicextdb_pdic_h
#define	__dicextdb_pdic_h

#include "dicextdb.h"

#if 0
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
#endif

#define	TP_WORD		0x10
#define	TP_JAPA		0x01
#define	TP_EXP		0x02

///////////////////////////////////////////////////
///	TExtPdic class
///////////////////////////////////////////////////
class Pdic;
class TExtPdic : public TPdicExtDBBase {
typedef TPdicExtDBBase super;
	struct TStrLoc {
		const tchar *word;
		int length;
		TStrLoc(const tchar *_word, int _length)
			:word(_word), length(_length) {}
		void get(tnstr &s)
		{
			s.set(word, length);
		}
	};
	class fwords_t : public SortedFlexObjectArray<TStrLoc>
	{
	typedef SortedFlexObjectArray<TStrLoc> super;
	public:
		fwords_t()
			:super(fwords_comp)
		{
		}
	protected:
		static int fwords_comp( const void *, const void * );
	};
protected:
	PdicBase *dic;
	Pdic *db;
	Japa *FoundJapa;
	//fwords_t FoundWords;
	//int WordIndex;
	const tchar *WordPtr;
	int TakePart;	// TP_xxxx
public:
	TExtPdic(PdicBase *dic);
	virtual ~TExtPdic();

	__override bool Open( const tchar * fname, bool _readonly );
	__override bool Create(const tchar *fname);
	__override void Close( );
	__override bool Record(const tchar *word, const Japa &japa);
	__override bool Erase(const tchar *word);
	__override bool Update(const tchar *word, const Japa &japa);
	__override bool SetAllSearch( const tchar *word, SrchMode mode, GENERICREXP *jre, AllSearchParam *all );
	__override int NextAllSearch( tnstr &word, Japa *japa, AllSearchParam *all);
	__override int PrevAllSearch( tnstr &word, Japa *japa, AllSearchParam *all);
	__override bool IsMySearchMode(SrchMode mode);

	__override tnstr GetDBPath(const tchar *dicname=NULL);
	//TODO: –³‘Ê for _KChar
	__override int NextAllSearch( _KChar &word, Japa *japa, AllSearchParam *all);
	__override int PrevAllSearch( _KChar &word, Japa *japa, AllSearchParam *all);

protected:
	tnstr GetDBName(bool create_dir, const tchar *dicname=NULL);
	bool OpenDB(const tchar *fname, bool readonly);
	bool CreateDB(const tchar *fname);
	void CloseDB();
	bool RecordItem(const char *type, const tchar *key, const tchar *text);
	bool DeleteItem(const char *type, const tchar *key);
	void ResetResult();
	void SetupFoundWords();
	static void ParseFoundWords(fwords_t &words, const tchar *text);
	static const tchar *FindInsertPoint(const tchar *words, const tchar *text);
	int FoundProc(tnstr &word, Japa *japa, AllSearchParam &all);

	// Auxiary information
	bool WriteInfo(const char *key, const char *value);
	tnstrA ReadInfo(const char *key);
	bool EraseInfo(const char *key);
};


#endif	// __dicextdb_pdic_h

