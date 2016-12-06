//---------------------------------------------------------------------------

#include "tnlib.h"
#pragma hdrstop

#include "UserTemp.h"
#include "pdprof.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

tnstr TUserTemplate::GetFileName( int index, bool save )
{
	tnstr name = tnsprintf(_t("tpl%02d.htm"), index+1 );
	return prof.GetTemplatePath(name, save);
}

