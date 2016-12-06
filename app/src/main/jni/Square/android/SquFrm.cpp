#include "pdclass.h"
#pragma hdrstop
#include "SquFrm.h"
#include "SquView.h"
#include "SquInterface.h"
#include "WinSquUI.h"

TSquareFrame::TSquareFrame(TSquUIMain *main)
{
	UIMain = main;

	TSquareView *view = new TSquareView(NULL);	//TODO: owner‚Ínewtab‚Å‚Í‚È‚¢‚Ì‚©H
	//view->Name = TtoWString(name);
	//view->Parent = newtab;	// Parent‚ÌŽw’è‚ðŠÔˆá‚¦‚é(SquareTab)‚É‚·‚é‚Æ³í‚É“®ì‚µ‚È‚¢‚Ì‚Å’ˆÓ
	squi = new TSquareIF(this, view);
	view->SetInterface(squi);
}
TSquareFrame::~TSquareFrame()
{
	if (squi) delete squi;
}

MPdic *TSquareFrame::GetDic()
{
	return squi->GetDic();
}

void TSquareFrame::SetMessage(TSquare *sender, const tchar *msg, int pasttime, int count)
{

}
bool TSquareFrame::IsAutoSpecialSearchEnabled()
{
    return false;
}

void TSquareFrame::ListClear()
{
	UIMain->ListClear();
}
void TSquareFrame::ListAdd(tnstr *word, Japa *japa, int dicno, int level)
{
	UIMain->ListAdd(word, japa, dicno, level);
}
void TSquareFrame::ListInsert(int index, tnstr *word, Japa *japa, int dicno, int level)
{
	UIMain->ListInsert(index, word, japa, dicno, level);
}
void TSquareFrame::ListDel(int index)
{
	UIMain->ListDel(index);
}
void TSquareFrame::ListDel(int index1, int index2)
{
	UIMain->ListDel(index1, index2);
}
