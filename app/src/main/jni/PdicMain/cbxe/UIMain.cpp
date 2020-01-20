#include <fmx.h>
#include <System.UITypes.hpp>
#pragma hdrstop
#include "TabbedTemplate.h"
#include "tnlib.h"
#include "UIMain.h"
#include "japa.h"
#include "LangProc.h"s5

TUIMain *TUIMain::Instance = NULL;

TUIMain::TUIMain(TPdicMain *main, IWordEdit *edit, TListBox *wordList)
{
	Main = main;
	Edit = edit;
	WordList = wordList;
}
TUIMain::~TUIMain()
{
	Instance = NULL;
}
TUIMain *TUIMain::CreateInstance(TPdicMain *main, IWordEdit *edit, TListBox *wordList)
{
	if (Instance)
		return Instance;
	return Instance = new TUIMain(main, edit, wordList);
}

tnstr TUIMain::GetWordText()
{
	return TabbedForm->edWord->Text;
}
tnstr TUIMain::GetCompleteWordText()
{
	return TabbedForm->edWord->Text;	//TODO: not complete word
}

void TUIMain::ListClear()
{
	WordList->Items->Clear();
}
void TUIMain::ListAdd(tnstr *word, Japa *japa, int dicno, int level)
{
	tnstr s = find_cword_pos(word->c_str());
	s += _t("=");
	s += japa->japa;
	WordList->Items->Add(s.c_str());
}
void TUIMain::ListInsert(int index, tnstr *word, Japa *japa, int dicno, int level)
{
	tnstr s = find_cword_pos(word->c_str());
	s += _t("=");
	s += japa->japa;
	WordList->Items->Insert(index, s.c_str());
}
void TUIMain::ListDel(int index)
{
	WordList->Items->Delete(index);
}
void TUIMain::ListDel(int index1, int index2)
{
	for (int i=index1;i<=index2;i++){
		WordList->Items->Delete(i);
	}
}
