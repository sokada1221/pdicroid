//---------------------------------------------------------------------------
// Text Resource

#ifndef txrH
#define txrH
//---------------------------------------------------------------------------

#include "TextResource.h"

const tchar *LoadText(int id);	// = GetTextResource() + GetString()
void LoadTextArray(tnstr_vec &array, int id, const tchar *delim);

#define	_LT(id)	LoadText(id)

#endif

