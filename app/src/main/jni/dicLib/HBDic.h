//---------------------------------------------------------------------------

#ifndef HBDicH
#define HBDicH
//---------------------------------------------------------------------------

#include "HBTree/HBTDefs.h"
#include "dicmix.h"

class THashBTree;
class TLangProc;

class THBDic {
public:
	THashBTree *Hbt;
	int LastError;
public:
	THBDic();
	virtual ~THBDic();
	bool Open(const tchar *filename, bool readonly);
	void Close();
	bool Record(const tchar *keyword, const tchar * word, int wordlen);
	bool Erase(const tchar *word, int wordlen);
};

#endif

