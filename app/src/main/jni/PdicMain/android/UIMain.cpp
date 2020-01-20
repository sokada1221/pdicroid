#include "tnlib.h"
#pragma hdrstop
#include "tnlib.h"
#include "UIMain.h"
#include "japa.h"
#include "LangProc.h"

TUIMain *TUIMain::Instance = NULL;

TUIMain::TUIMain(TPdicMain *main, IWordEdit *edit, IWordList *wordList)
{
	Main = main;
	Edit = edit;
	WordList = wordList;
}
TUIMain::~TUIMain()
{
	Instance = NULL;
}
TUIMain *TUIMain::CreateInstance(TPdicMain *main, IWordEdit *edit, IWordList *wordList)
{
	if (Instance)
		return Instance;
	return Instance = new TUIMain(main, edit, wordList);
}
MPdic *TUIMain::GetActiveDic()
{
	return Main->GetActiveDic();
}
void TUIMain::dispStar(int no, int rev)
{
	WordList->Select(no, rev);
}
tnstr TUIMain::GetWordText()
{
	return Edit->GetText();
}
tnstr TUIMain::GetCompleteWordText()
{
	return Edit->GetText();	//TODO: not complete word
}

void TUIMain::ListClear()
{
	WordList->Clear();
}
void TUIMain::ListAdd(tnstr *word, Japa *japa, int dicno, int level)
{
	WordList->Add(find_cword_pos(word->c_str()), japa->japa, japa->pron, japa->exp);
}
void TUIMain::ListInsert(int index, tnstr *word, Japa *japa, int dicno, int level)
{
	WordList->Insert(index, find_cword_pos(word->c_str()), japa->japa, japa->pron, japa->exp);
}
void TUIMain::ListDel(int index)
{
	WordList->Delete(index);
}
void TUIMain::ListDel(int index1, int index2)
{
	for (int i=index1;i<=index2;i++){
		WordList->Delete(i);
	}
}
