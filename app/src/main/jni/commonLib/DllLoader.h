//---------------------------------------------------------------------------

#ifndef DllLoaderH
#define DllLoaderH
//---------------------------------------------------------------------------

class TLoadCounter {
protected:
	int Counter;
public:
	TLoadCounter()
	{
		Counter = 0;
	}
	bool Up()
	{
		if (Counter>0){
			Counter++;
			return false;
		}
		Counter++;
		return true;
	}
	bool Down()
	{
		if (Counter==0)
			return false;
		Counter--;
		return Counter==0;
	}
	int GetCounter() const
		{ return Counter; }
};

// Counter‚É‚æ‚éDLL‚ÌÄƒ[ƒh‚ð–hŽ~‚µ‚½‚¢ê‡‚ÍA
// static‚ÈTLoadCounter object‚ðconstructor‚É“n‚·

class TDllLoader {
protected:
	HINSTANCE hDll;
	TLoadCounter *Counter;
public:
	TDllLoader(TLoadCounter *counter=0)
		:Counter(counter)
	{
		hDll = NULL;
	}
	virtual ~TDllLoader()
	{
		if (hDll){
			FreeLibrary(hDll);
		}
		if (Counter)
			delete Counter;
	}
	bool IsLoaded() const
	{
		return hDll ? true : false;
	}
	virtual void Unload();
protected:
	FARPROC GetProc(LPCSTR name)
	{
		return ::GetProcAddress(hDll, name);
	}
	FARPROC GetProcAddress(LPCSTR name)
	{
		return ::GetProcAddress(hDll, name);
	}
	bool Load(const tchar *dllname)
	{
		if (Counter && !Counter->Up())
			return true;
		if (hDll)
			return true;
		hDll = LoadLibrary(dllname);
		if (!hDll){
			if (Counter)
				Counter->Down();
			return false;
		}
		if (!LoadProcs()){
			FreeLibrary(hDll);
			if (Counter)
				Counter->Down();
			return false;
		}
		return true;
	}
	virtual bool LoadProcs() { return true; }
};

#endif

