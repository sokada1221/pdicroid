//---------------------------------------------------------------------------

#include "tnlib.h"
#pragma hdrstop
#include "dictype.h"

#include "DicAttr.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

TDicAttr DicAttrs[] =
{
	{ DT_EXTPDICTEXT, RI_WORD|RI_JAPA|RI_EXP, 0},
	{ DT_CSV,         RI_WORD|RI_JAPA|RI_EXP|RI_LEVEL|RI_MEMORY|RI_MODIFY|RI_PRON|RI_FILELINK, 0},
	{ 0 }
};

