//---------------------------------------------------------------------------

#ifndef DicDecH
#define DicDecH
//---------------------------------------------------------------------------

class TPwdPool {
protected:
	vector<string> pwds;
	tnstr filename;
public:
	TPwdPool(const tchar *filename);
	const vector<string> &get();
	bool add(const char *pwd, int length);
	static void makehash(const char *data, int length, char hash[16]);
protected:
	bool readfile();
	std::string getkey();
	bool getmacaddr(char *addr);
	bool getusername(tnstr &name);
};

class TDicDec {
protected:
	class TUnzip *unzip;
public:
	TDicDec();
	~TDicDec();
	static bool IsDicDec(const struct HEADER *header);
	unsigned GetDecodeSize(const char *data);
	void Decode(const char *data, byte *out, unsigned size);
};

#endif

