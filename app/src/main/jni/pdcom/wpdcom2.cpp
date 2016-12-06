//---------------------------------------------------------------------------
// vcl‚Æshlobj.h‚ª‚Ô‚Â‚©‚é‚½‚ßAshlobj‚È‚ÇVCL‚Æ‘Š«‚Ìˆ«‚¢API‚ğg‚¤ê‡
#include "tnlib.h"
#pragma hdrstop
#include <shlobj.h>
#include "defs.h"
#include "wpdcom2.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

tnstr GetSpecialFolder(int csidl)
{
	wchar_t path[MAX_PATH];
	if (!SHGetSpecialFolderPath(NULL, path, csidl, FALSE)){
		return _t("");
	}
	return tnstr(path);
}
