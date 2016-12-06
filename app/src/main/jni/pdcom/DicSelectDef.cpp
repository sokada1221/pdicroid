//---------------------------------------------------------------------------
#include <windows.h>
#pragma hdrstop

#include "DicSelectDef.h"
#include "pddefs.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

void TRItemSelect::SetPartBySqm(int sqm)
{
	itemWord = (sqm&SQM_WORD)?true:false;
	itemJapa = (sqm&SQM_JAPA)?true:false;
	itemPron = (sqm&SQM_PRON)?true:false;
	itemExp  = (sqm&SQM_EXP)?true:false;
//	itemObjTitle = (sqm&SQM_TITLE)?true:false;
//	itemFileLinkFileName = (sqm&SQM_FILENAME)?true:false;
}

int TRItemSelect::GetPartBySqm()
{
	return (itemWord ? SQM_WORD : 0)
		| (itemJapa ? SQM_JAPA : 0)
		| (itemPron ? SQM_PRON : 0)
		| (itemExp ? SQM_EXP : 0);
}

SrchMode TRItemSelect::GetSrchMode()
{
	int mode = 0;
	if (itemWord)
		mode |= SRCH_WORD;
	if (itemJapa)
		mode |= SRCH_JAPA;
	if (itemExp)
		mode |= SRCH_EXP;
	if (itemPron)
		mode |= SRCH_PRON;
#if 0
	if (itemObjTitle)
		mode |= SRCH_TITLE;
	if (itemFileLinkFileName)
		mode |= SRCH_FILENAME;
#endif
	return (SrchMode)mode;
}

