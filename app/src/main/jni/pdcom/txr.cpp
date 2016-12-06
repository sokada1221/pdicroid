//---------------------------------------------------------------------------
#include "tnlib.h"
#pragma hdrstop

#include "tid.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

const tchar *LoadText(int id)
{
	if (id<TID_ORIGIN){
		return GetString(id);
	} else {
		return GetTextResource(id);
	}
}

void LoadTextArray(tnstr_vec &array, int id, const tchar *delim)
{
	if (id<TID_ORIGIN){
		LoadStringArray(array, id, delim);
	} else {
		GetTextResourceArray(array, id, delim);
	}
}


