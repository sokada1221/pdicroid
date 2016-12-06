#include "tnlib.h"
#pragma	hdrstop
#include "pdconfig.h"
#include "pddefs.h"
#include	"winsqu.h"
#include	"winmsg.h"

COLORREF GetGrayReverse(COLORREF a, COLORREF b);

int sn2type( int sn )
{
	switch ( sn )
	{
		case SN_WORD: return 0;
		case SN_PRON: return 1;
		case SN_JAPA: return 2;
		case SN_EXP1: return 3;
		default: return -1;
	}
}

int sn2snfont(int sn)
{
	switch ( sn )
	{
		case SN_WORD: return SNFONT_WORD;
		case SN_PRON: return SNFONT_PRON;
		case SN_JAPA: return SNFONT_JAPA;
		case SN_EXP1: return SNFONT_EXP;
		case SN_EPWING: return SNFONT_EPWING;
		default: return SNFONT_COMMON;
	}
}
