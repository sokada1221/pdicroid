//---------------------------------------------------------------------------

#ifndef dicextdbH
#define dicextdbH
//---------------------------------------------------------------------------

#define	DICEXT_FASTDB			0x0001	// 高速全文検索は有効
#define	DICEXT_FASTDB_CREATE	0x0002	// 高速全文検索辞書がなければ作成する(ただしreadonly時は作成せず）

class TLangProc;
class Japa;
class TPdicExtDBBase {
protected:
	//bool CanCreate;
	TLangProc *LangProc;
public:
	TPdicExtDBBase();
	virtual ~TPdicExtDBBase();
#if 0
	void SetMode(bool can_create)
	{
		CanCreate = can_create;
	}
#endif
	virtual void SetLangProc(TLangProc *proc);
	virtual bool Open( const tchar * fname, bool _readonly ) { return true; }
	virtual bool Create(const tchar *fname) = 0;
	virtual void Close( ){}
	virtual bool Record(const tchar *word, const Japa &japa) { return true; }
	virtual bool Erase(const tchar *word) { return true; }
	virtual bool Update(const tchar *word, const Japa &japa) { return true; }
	virtual bool SetAllSearch( const tchar *word, SrchMode mode, GENERICREXP *jre, AllSearchParam *all) { return true; }
	virtual int NextAllSearch( tnstr &word, Japa *japa, AllSearchParam *all) { return AS_END; }
	virtual int PrevAllSearch( tnstr &word, Japa *japa, AllSearchParam *all) { return AS_END; }
	virtual bool IsMySearchMode(SrchMode mode) { return false; }
	virtual tnstr GetDBPath(const tchar *dicname=NULL) { return _t(""); }

	//TODO: 無駄 for _KChar
	virtual int NextAllSearch( _KChar &word, Japa *japa, AllSearchParam *all);
	virtual int PrevAllSearch( _KChar &word, Japa *japa, AllSearchParam *all);
	const TKCodeTranslateSet *GetKCodeTrans() const;
};

class PdicExtDBLayer : public PdicBase {
typedef PdicBase super;
protected:
	int ExtDBMode;
	TPdicExtDBBase *ExtDB;
public:
	PdicExtDBLayer();
	virtual ~PdicExtDBLayer();

	void SetExtDBMode(int mode, bool f);
	tnstr GetExtDBPath(const tchar *dicname=NULL);

	// Overrides
	virtual int Open( const tchar * fname, bool _readonly );
	virtual int Create( const tchar *fname, const tchar *dicname, int size, int dictype, int lang_proc_id=-1 );
	virtual void Close( );
	virtual int record( const tchar * word, const Japa &japa, int srchf=1 );
	virtual int update( const tchar *word, const Japa &japa );
	virtual int erase( const tchar *word );
	virtual void SetAllSearch( const tchar * word, SrchMode mode, GENERICREXP *jre, AllSearchParam *all=NULL );
	virtual int NextAllSearch( tnstr &word, Japa *japa, AllSearchParam *all=NULL);
	virtual int PrevAllSearch( tnstr &word, Japa *japa, AllSearchParam *all=NULL);

	// to avoid error due to override
	virtual int NextAllSearch( _KChar &word, Japa *japa, AllSearchParam *all=NULL);
	virtual int PrevAllSearch( _KChar &word, Japa *japa, AllSearchParam *all=NULL);

protected:
	int OpenExtDB(const tchar *fname, int _readonly, bool create_force=false);
	int CreateExtDB(const tchar *fname);
	void CloseExtDB();
};

#endif

