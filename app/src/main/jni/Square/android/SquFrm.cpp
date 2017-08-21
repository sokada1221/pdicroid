#include "pdclass.h"
#pragma hdrstop
#include "SquFrm.h"
#include "SquView.h"
#include "SquInterface.h"
#include "WinSquUI.h"

TSquareFrame::TSquareFrame(TSquUIMain *main)
{
	UIWord = NULL;
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

void TSquareFrame::SetUIMain(TSquUIMain *uimain)
{
	UIMain = uimain;
	squi->SetUIMain( uimain );
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
	if (UIMain) UIMain->ListClear();
}
void TSquareFrame::ListAdd(tnstr *word, Japa *japa, int dicno, int level)
{
	if (UIMain) UIMain->ListAdd(word, japa, dicno, level);
}
void TSquareFrame::ListInsert(int index, tnstr *word, Japa *japa, int dicno, int level)
{
	if (UIMain) UIMain->ListInsert(index, word, japa, dicno, level);
}
void TSquareFrame::ListDel(int index)
{
	if (UIMain) UIMain->ListDel(index);
}
void TSquareFrame::ListDel(int index1, int index2)
{
	if (UIMain) UIMain->ListDel(index1, index2);
}
