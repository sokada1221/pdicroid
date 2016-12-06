#include "tnlib.h"
#pragma hdrstop
#include "MainDic.h"

#include "UIMain.h"

MPdic *GetActiveDic()
{
	TUIMain *main = TUIMain::GetInstance();
	if (!main)
		return NULL;
	return main->GetActiveDic();
}
