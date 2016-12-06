//---------------------------------------------------------------------------

#ifndef LPMLoaderH
#define LPMLoaderH
//---------------------------------------------------------------------------

#include "DllLoader.h"

#define	LPMFUNCDEF( ret, name, args ) \
	ret WINAPI name args ; \
	typedef ret WINAPI (*FN##name) args

LPMFUNCDEF(int, QueryLangId, (int langid));

class TLPMLoaderMin : public TDllLoader {
typedef TDllLoader super;
protected:
	tnstr FileName;
public:
	FNQueryLangId QueryLangId;
public:
	TLPMLoaderMin(const tchar *filename)
		:super(new TLoadCounter)
		,FileName(filename)
	{
	}
	virtual bool Load()
	{
		return super::Load(FileName);
	}
protected:
	virtual bool LoadProcs();
};

class TLPMLoader : public TLPMLoaderMin {
typedef TLPMLoaderMin super;
public:
	TLPMLoader(const tchar *filename)
		:super(filename)
	{
	}
protected:
	virtual bool LoadProcs();
};

#endif

