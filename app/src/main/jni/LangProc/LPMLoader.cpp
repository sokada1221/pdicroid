//---------------------------------------------------------------------------

#include "tnlib.h"
#pragma hdrstop

#include "LPMLoader.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//
// TLPMLoaderMin class
//
bool TLPMLoaderMin::LoadProcs()
{
	QueryLangId = (FNQueryLangId)GetProc("QueryLangId");

	return QueryLangId ? true : false;
}
//
// TLPMLoader class
//
bool TLPMLoader::LoadProcs()
{
	return super::LoadProcs();
}

