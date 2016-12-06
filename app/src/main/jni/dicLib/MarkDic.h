#ifndef __MarkDic_h
#define	__MarkDic_h

struct TMarkInfo {
	int sn;
	int pos;
	int length;
};

class Pdic;
class Japa;

class TMarkDic {
protected:
	Pdic *Dic;
	Pdic *MainDic;	// reference pointer
public:
	TMarkDic(Pdic *mainDic);
	virtual ~TMarkDic();
	bool Open(const tchar *filename, bool readonly);
	bool Create(const tchar *filename);
	void Close();
	bool AddMark(const tchar *word, int sn, int pos, int length);
	bool RemoveMark(const tchar *word, int sn, int pos);
	bool GetMark(const tchar *word, int sn, int pos, TMarkInfo &minfo);
	bool ClearMark(const tchar *word);
	bool IsMarked(const tchar *word);
	bool FindMark(int type);
	bool InduceMark(const tchar *word, Japa &japa);
protected:
	bool Read(const tchar *word, tnstr &s);
	bool Write(const tchar *word, const tchar *s);
};


#endif	// __MarkDic_h

