//---------------------------------------------------------------------------

#ifndef LangProcManH
#define LangProcManH
//---------------------------------------------------------------------------

class TLangProc;

typedef class TLangProc *(*FNCreateLangProcInstance)();

struct TLangProcInfo {
	int id;	// 32bit language processor ID
	tnstr name;
	FNCreateLangProcInstance create_inst;

	TLangProcInfo()
	{
		id = 0;
		create_inst = NULL;
	}
	TLangProcInfo(const TLangProcInfo &o)
		:id(o.id)
		,name(o.name)
		,create_inst(o.create_inst)
	{
	}
	TLangProcInfo(int _id, const tchar *_name, FNCreateLangProcInstance _create_inst)
		:id(_id)
		,name(_name)
		,create_inst(_create_inst)
	{
	}
};

// The manager of Language Processor
class TLangProcMan {
protected:
	typedef map<TLangProc*,int> lpref_map_t;
	vector<TLangProcInfo> LangProcInfo;
	vector<TLangProc*> LangProcs;
	lpref_map_t LangProcRef;
public:
	TLangProcMan();
	virtual ~TLangProcMan();
	void Close();
	bool Register(const TLangProcInfo &info);
	int FindId(int id);	// IdToIndex
	int FindNearestId(int id);	// IdToIndex
	static bool CompareId(int id1, int id2);	// is same group id?
	int GetId(int index);
	tnstr GetName(int index);
	int NameToIndex(const tchar *name);
	TLangProc *GetLangProc(int index);	// Do not delete the returned object. Free the object by calling Free()
	TLangProc *GetLangProc(const tchar *name);	// Do not delete the returned object. Free the object by calling Free()
	TLangProc *GetStdLangProc()
		{ return GetLangProc(0); }
	int GetStdLangProcId() const;
	void Release(TLangProc *proc)
		{ Free(proc); }
	void Free(TLangProc *);
	bool IsUsing();
protected:	
	TLangProc *Create(int index);
public:
	void IncRef(TLangProc *lp);
	int DecRef(TLangProc *lp);

#if 0
	// for debug
	void SetFileName(int index, const tchar *filename)
		{ FileNames[index] = filename; }
#endif
};

extern TLangProcMan LangProcMan;

#endif
