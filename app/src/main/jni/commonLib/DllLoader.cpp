//---------------------------------------------------------------------------

#include "tnlib.h"
#pragma hdrstop

#include "DllLoader.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

void TDllLoader::Unload()
{
	if (Counter && !Counter->Down())
		return;
	if (!hDll)
		return;
	FreeLibrary(hDll);
	hDll = NULL;
}

