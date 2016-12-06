//---------------------------------------------------------------------------
// THashBTree‚ğPdic—pclass‚É‚·‚é‚½‚ß‚Ìclass
//
#include "tnlib.h"
#pragma hdrstop

#include "HBDic.h"
#include "HBTree/HBTree.h"
#include "dicerr.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

THBDic::THBDic()
{
	Hbt = NULL;
	LastError = 0;
}
THBDic::~THBDic()
{
	Close();
}

bool THBDic::Open(const tchar *filename, bool readonly)
{
	if (Hbt){
		LastError = HBTE_OPENED;
		return false;	// Already opened.
	}
	Hbt = new THashBTree();
	if (!Hbt){
		LastError = HBTE_MEMORY;
		return false;	// memory error
	}
	if (!Hbt->Open(filename, readonly)){
		if (Hbt->GetLastError()==DICERR_OPEN_CREATE){
			if (!Hbt->Create(filename)){
				LastError = Hbt->GetLastError();
				delete Hbt;
				Hbt = NULL;
				return false;
			}
		} else {
			LastError = Hbt->GetLastError();
			delete Hbt;
			Hbt = NULL;
			return false;
		}
	}
	return true;
	
}
void THBDic::Close()
{
	if (!Hbt)
		return;
	delete Hbt;
	Hbt = NULL;
}

bool THBDic::Record(const tchar *keyword, const tchar *word, int wordlen)
{
	if (!Hbt){
		LastError = HBTE_NOT_OPENED;
		return false;
	}
	if (!Hbt->AddData(keyword, word, wordlen)){
		LastError = Hbt->GetLastError();
		return false;
	}
	return true;
}
bool THBDic::Erase(const tchar *keyword, int wordlen)
{
	if (!Hbt){
		LastError = HBTE_NOT_OPENED;
		return false;
	}
#if 0
	if (!Hbt->DeleteData(keyword, wordlen)){
		LastError = HBTE_Hbt->GetLastError();
		return false;
	}
#endif
	return true;
}


