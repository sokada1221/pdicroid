//---------------------------------------------------------------------------

#ifndef nkfdllH
#define nkfdllH
//---------------------------------------------------------------------------

typedef int (*FNConvertStreamCallback)(int ch, class TNkf *obj);
typedef int (*FNConvertLineCallback)(const tchar *line, int len, class TNkf *obj);

class TNkf {
protected:
	class TNkfLoader *Dll;
public:
	TNkf(const tchar *dllpath);
	virtual ~TNkf();
	bool IsValid() const;
	void SetOption(const char *option);
	bool ConvertFile(const tchar *filename, const tchar *out);
	bool ConvertStream(FNConvertStreamCallback is, FNConvertStreamCallback os);
	__tnstrA GetGuess();
protected:
	FNConvertStreamCallback InStream;
	FNConvertStreamCallback OutStream;
	static int CALLBACK cbInStream(int ch, int userdata);
	static int CALLBACK cbOutStream(int userdata, int ch);
public:
	int Guess(const tchar *str);

	// Additional methods.
protected:
	// stream out
	FNConvertLineCallback cbLine;
	// in parameters.
	FILE *fp;
	const char *InData;
	const char *InDataEnd;
	const char *InDataPtr;
	// out work buffers.
	tnstrbuf Line;
	bool cr;
public:
	int UserData;	// user data
	int Result;		// user data
public:
	bool ConvertFileStream(const tchar *filename, FNConvertLineCallback cb);
	bool ConvertDataStream(const char *data, int datalen, FNConvertLineCallback cb);
protected:
	// in stream
	static int CALLBACK cbInFileStream(int userdata, int ch);
	static int CALLBACK cbInDataStream(int userdata, int ch);
	// out stream
	static int CALLBACK cbStreamOutLine(int ch, int userdata);
	int StreamOutLine(int ch);
};

#endif

