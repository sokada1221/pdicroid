//---------------------------------------------------------------------------

#include "tnlib.h"
#pragma hdrstop

#include "LangProcMan.h"
#include "LangProc.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

TLangProcMan LangProcMan;

////////////////////////////////////////////////////
// TLangProcMan calss
////////////////////////////////////////////////////
TLangProcMan::TLangProcMan()
{
}
TLangProcMan::~TLangProcMan()
{
	Close();
}
void TLangProcMan::Close()
{
	foreach(LangProcs, lp, vector<TLangProc*>){
		__assert(LangProcRef[*lp]==0);
		delete (*lp);
	}
	LangProcs.clear();
}
bool TLangProcMan::Register(const TLangProcInfo &info)
{
	int index = FindId(info.id);
	if (index!=-1){
		// same id already exists.
		return false;
	}
	LangProcInfo.push_back(info);
	LangProcs.push_back(NULL);
	return true;
}

// IdToIndex
int TLangProcMan::FindId(int id)
{
	for (int i=0;i<LangProcInfo.size();i++){
		if (LangProcInfo[i].id == id){
			return i;
		}
	}
	return -1;	
}

int TLangProcMan::FindNearestId(int id)
{
	int nearest_index = -1;
	for (int i=0;i<LangProcInfo.size();i++){
		if (LangProcInfo[i].id == id){
			return i;
		}
		if (((LangProcInfo[i].id ^ id) & LPID_LA_MASK)==0){
			nearest_index = i;
		}
	}
	return nearest_index;
}

// Is same group id?
bool TLangProcMan::CompareId(int id1, int id2)
{
	return ((id1 ^ id2) & LPID_LA_MASK) == 0;
}

int TLangProcMan::GetId(int index)
{
	if (index<0 || index>=LangProcInfo.size())
		return -1;
	return LangProcInfo[index].id;
}

tnstr TLangProcMan::GetName(int index)
{
	if (index<0 || index>=LangProcInfo.size())
		return _T("");
	return LangProcInfo[index].name;
}
int TLangProcMan::NameToIndex(const tchar *name)
{
	for (int i=0;i<LangProcInfo.size();i++){
		if (!_tcscmp(LangProcInfo[i].name, name)){
			return i;
		}
	}
	return -1;
}

// Do not delete the returned object. Free the object by calling Free()
TLangProc *TLangProcMan::GetLangProc(int index)
{
	if (index<0 || index>=LangProcs.size()){
		return NULL;
	}
	TLangProc *lp = LangProcs[index];
	if (!lp){
		lp = LangProcs[index] = Create(index);
	}
	IncRef(lp);
	return lp;
}
// Do not delete the returned object. Free the object by calling Free()
TLangProc *TLangProcMan::GetLangProc(const tchar *name)
{
	int index = NameToIndex(name);
	if (index<0)
		return NULL;	// not found the matched name.
	return GetLangProc(index);
}
int TLangProcMan::GetStdLangProcId() const
{
	if (LangProcInfo.size()==0)
		return -1;	// invalid
	return LangProcInfo[0].id;
}

void TLangProcMan::Free(TLangProc *lp)
{
	DecRef(lp);
}
bool TLangProcMan::IsUsing()
{
	foreach (LangProcRef, it, lpref_map_t){
		if (it->second>0)
			return true;
	}
	return false;
}
TLangProc *TLangProcMan::Create(int index)
{
	if (index<0 || index>=LangProcInfo.size())
		return NULL;
	TLangProc *lp = LangProcInfo[index].create_inst();
	lp->Open(/*FileNames[index]*/);
	return lp;
}
void TLangProcMan::IncRef(TLangProc *lp)
{
	LangProcRef[lp] = LangProcRef[lp] + 1;
}
int TLangProcMan::DecRef(TLangProc *lp)
{
	if (LangProcRef.count(lp)){
		if (LangProcRef[lp]>0){
			LangProcRef[lp] = LangProcRef[lp] - 1;
			return LangProcRef[lp];
		} else {
			__assert__;
			return 0;
		}
	} else {
		__assert(lp==NULL);
		return -1;
	}
}

