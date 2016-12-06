#include	"tnlib.h"
#pragma	hdrstop
#include "pdconfig.h"
#include "dic.h"
#include "pdtypes.h"
#include "DicStr.h"
#include "pdstrlib.h"

#ifndef SMALL

// If you use this method, you must add the pdcom2/DicStr.cpp in your application.
void HEADER::GetDicType( tnstr &text ) const
{
	text = GetDicTypeStr(dicorder, dictype);
}

#endif	// !SMALL

tnstr HEADER::GetIdStr() const
{
	return bintohex(dicident, sizeof(dicident));
}

