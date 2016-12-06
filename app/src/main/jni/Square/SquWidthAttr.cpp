//---------------------------------------------------------------------------
#include "tnlib.h"
#pragma hdrstop
#include "defs.h"
#include "SquWidthAttr.h"
#include "tid.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef SMALL
#define	NUM_WA_VIEW	0
#else
#define	NUM_WA_VIEW	1
#endif

#ifdef	USE_MULTIFONT
#define	_WA_VARFONT	WA_VARFONT
#else
#define	_WA_VARFONT	0
#endif

#define	PRON_WA_VIEW	WA_VIEW

WidthAttrInit widthattrinit[ N_SECTION + 1 ] = {
#ifdef SMALL
	{ 32,	0, RGB_BLACK },
	{ 40,	0, RGB_BLACK },
	{ 100,	WA_VARWIDTH | _WA_VARFONT | WA_VIEW, RGB_BLACK },
	{ 110,	WA_VARWIDTH | _WA_VARFONT, RGB_BLACK },
	{ 60,	WA_VARWIDTH, RGB_BLACK },
	{ 320,	_WA_VARFONT | WA_VIEW | WA_PUSH, RGB_BLACK },
	{ 0, 0 }
#else
	{ 32,	NUM_WA_VIEW, RGB_BLACK },	// SN_NUMBER
	{ 40,	WA_VIEW, RGB_BLACK },	// SN_ATTR
	{ 60,	WA_VARWIDTH | _WA_VARFONT | WA_VIEW, RGB_BLACK },	// SN_WORD
	{ 70,	WA_VARWIDTH | _WA_VARFONT | PRON_WA_VIEW, RGB_BLACK },	// SN_PRON
	{ 40,	WA_VARWIDTH | WA_VIEW, RGB_BLACK },	// SN_LEVEL
	{ 320,	_WA_VARFONT | WA_VIEW | WA_PUSH, RGB_BLACK },	// SN_JAPA
	{ 0, 0 }
#endif
};

tnstr WidthAttrInit::GetTitle(int index)
{
	return _LT(index+TID_ITEM_NO);
}

void WidthAttr::Set( WidthAttrInit &wai, int index )
{
	title.set( wai.GetTitle(index) );
	width = wai.width;
	attr = wai.attr;
	color = wai.color;
}

