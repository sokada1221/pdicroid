//---------------------------------------------------------------------------
#include "tnlib.h"
#pragma hdrstop

#include "nkfdll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

#include "DllLoader.h"
#include "nkf/nkfu.h"

class TNkfLoader : public TDllLoader {
typedef TDllLoader super;
protected:
	//static TLoadCounter counter;
	FNSetNkfOption fnSetNkfOption;
	FNNkfGetKanjiCode fnNkfGetKanjiCode;
	FNGetNkfVersion fnGetNkfVersion;
	FNNkfConvertFileW fnNkfConvertFile;
	FNNkfConvertString fnNkfConvertString;
	FNNkfConvertStream fnNkfConvertStream;
	FNGetNkfGuess fnGetNkfGuess;
	tnstr Path;
public:
	TNkfLoader(const tchar *path)
		:super(new TLoadCounter())	//TODO: ‚±‚ê‚Å‚¢‚¢H
		,Path(path)
	{
	}
	virtual bool Load()
	{
		return super::Load(Path);
	}
	const char *GetVersion()
	{
		return fnGetNkfVersion();
	}
	inline void SetOption(const char *option)
	{
		fnSetNkfOption(option);
	}
	void _Convert(const char *s, int len)
	{
		char buf[1024];	//TODO:
		DWORD ret;
		fnNkfConvertString(buf, sizeof(buf), &ret, s, len);
	}
	inline bool ConvertFile(const tchar *filename, const tchar *out)
	{
		return (bool)fnNkfConvertFile(filename, out);
	}
public:
	bool ConvertStream(FNNkfConvertStreamCallback in, FNNkfConvertStreamCallback out, int userdata)
	{
		return (bool)fnNkfConvertStream(in, out, userdata);
	}
	__tnstrA GetGuess()
	{
		const char *g = fnGetNkfGuess();
		return __tnstrA(g);
	}
protected:
	virtual bool LoadProcs()
	{
		fnSetNkfOption = (FNSetNkfOption)GetProc("SetNkfOption");
		fnNkfGetKanjiCode = (FNNkfGetKanjiCode)GetProc("NkfGetKanjiCode");
		fnGetNkfVersion = (FNGetNkfVersion)GetProc("GetNkfVersion");
		fnNkfConvertFile = (FNNkfConvertFileW)GetProc("NkfConvertFileW");
		fnNkfConvertString = (FNNkfConvertString)GetProc("NkfConvertString");
		fnNkfConvertStream = (FNNkfConvertStream)GetProc("NkfConvertStream");
		fnGetNkfGuess = (FNGetNkfGuess)GetProc("GetNkfGuess");
		return fnGetNkfVersion!=NULL;
	}
};

//TLoadCounter TNkfLoader::counter;

//
// TNkf class
//
TNkf::TNkf(const tchar *dllpath)
{
	Dll = new TNkfLoader(dllpath);
	Dll->Load();
#if 0	// for test
	if (IsValid()){
		Dll->GetVersion();
		Dll->_Convert((char*)L"hoge", 4);
		printf("%s\n", nkf.GetGuess().c_str());
		Dll->_Convert("hoge", 4);
		printf("%s\n", nkf.GetGuess().c_str());
	}
#endif
}

TNkf::~TNkf()
{
	if (Dll)
		delete Dll;
}

bool TNkf::IsValid() const
{
	return Dll && Dll->IsLoaded();
}

void TNkf::SetOption(const char *option)
{
	Dll->SetOption(option);
}

bool TNkf::ConvertFile(const tchar *filename, const tchar *out)
{
	return Dll->ConvertFile(filename, out);
}

bool TNkf::ConvertStream(FNConvertStreamCallback is, FNConvertStreamCallback os)
{
	InStream = is;
	OutStream = os;
	Dll->ConvertStream(cbInStream, cbOutStream, (int)this);
	return true;
}

__tnstrA TNkf::GetGuess()
{
	return Dll->GetGuess();
}

int CALLBACK TNkf::cbInStream(int ch, int userdata)
{
	TNkf *self = (TNkf*)userdata;
	return self->InStream(ch, self);
}

int CALLBACK TNkf::cbOutStream(int ch, int userdata)
{
	TNkf *self = (TNkf*)userdata;
	return self->OutStream(ch, self);
}

int TNkf::Guess(const tchar *str)
{
	if (!Dll->IsLoaded())
		return 0;
	return 0;
}

// file -> line callback
bool TNkf::ConvertFileStream(const tchar *filename, FNConvertLineCallback cb)
{
	fp = _wfopen(filename, L"rb");
	if (!fp)
		return false;
	Line.clear();
	cr = false;
	cbLine = cb;
	Dll->ConvertStream(cbInFileStream, cbStreamOutLine, (int)this);
	fclose(fp);
	fp = NULL;
	return true;
}

bool TNkf::ConvertDataStream(const char *data, int datalen, FNConvertLineCallback cb)
{
	InData = data;
	InDataEnd = InData + datalen;
	InDataPtr = InData;
	Line.clear();
	cr = false;
	cbLine = cb;
	Dll->ConvertStream(cbInDataStream, cbStreamOutLine, (int)this);
	return true;
}

int CALLBACK TNkf::cbInFileStream(int ch, int userdata)
{
	TNkf *self = (TNkf*)userdata;
	return fgetc(self->fp);
}

int CALLBACK TNkf::cbInDataStream(int userdata, int ch)
{
	TNkf *self = (TNkf*)userdata;
	if (self->InDataPtr>=self->InDataEnd)
		return -1;	// eof
	return *(unsigned char*)self->InDataPtr++;
}

int CALLBACK TNkf::cbStreamOutLine(int ch, int userdata)
{
	TNkf *self = (TNkf*)userdata;
	return self->StreamOutLine(ch);
}

int TNkf::StreamOutLine(int ch)
{
	tchar buf[2] = {ch,0};
	if (ch=='\r'){
		cr = true;
		return 0;
	}
	if (ch=='\n'){
		int r = cbLine(Line, Line.length(), this);
		Line.clear();
		return r;
	}
	if (cr){
		int r = cbLine(Line, Line.length(), this);
		Line.clear();
		if (r!=0)
			return r;
	}
	Line += buf;
	return 0;
}

