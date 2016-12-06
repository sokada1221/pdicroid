#include "tnlib.h"
#pragma hdrstop
#include "MarkDic.h"
#include "dic.h"

TMarkDic::TMarkDic(Pdic *mainDic)
	:MainDic(mainDic)
{
	Dic = NULL;
}

TMarkDic::~TMarkDic()
{
	Close();
}

bool TMarkDic::Open(const tchar *filename, bool readonly)
{
	if (Dic)
		return false;	// already opened.
	Dic = Pdic::CreateInstance(filename);
	if (!Dic)
		return false;
	if (Dic->Open(filename, readonly)!=0){
		Dic->Release();
		Dic = NULL;
		return false;
	}
	return true;
}
bool TMarkDic::Create(const tchar *filename)
{
	if (Dic)
		return false;	// already opened.
	Dic = Pdic::CreateInstance(filename);
	if (!Dic)
		return false;
	if (Dic->CreateHyper(filename)!=0){
		Dic->Release();
		Dic = NULL;
		return false;
	}
	return true;
}
void TMarkDic::Close()
{
	if (!Dic)
		return;
	Dic->Close();
	Dic->Release();
	Dic = NULL;
}
bool TMarkDic::AddMark(const tchar *word, int sn, int pos, int length)
{
	// not implemented.
	return false;
}
bool TMarkDic::RemoveMark(const tchar *word, int sn, int pos)
{
	// not implemented.
	return false;
}
bool TMarkDic::GetMark(const tchar *word, int sn, int pos, TMarkInfo &minfo)
{
	// not implemented.
	return false;
}
bool TMarkDic::ClearMark(const tchar *word)
{
	// not implemented.
	return false;
}
bool TMarkDic::IsMarked(const tchar *word)
{
	// not implemented.
	return false;
}
bool TMarkDic::FindMark(int type)
{
	// not implemented.
	return false;
}
bool TMarkDic::InduceMark(const tchar *word, Japa &japa)
{
	// not implemented.
	return false;
}
bool TMarkDic::Read(const tchar *word, tnstr &s)
{
	return false;
}
bool TMarkDic::Write(const tchar *word, const tchar *s)
{
	return false;
}


