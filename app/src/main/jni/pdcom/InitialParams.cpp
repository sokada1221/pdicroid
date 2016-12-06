#include "tnlib.h"
#pragma hdrstop
#include "InitialParams.h"
#include "pdstr.h"

TInitialParams::TInitialParams()
{
	State = 0;
	ShowState = SW_SHOW;

	StartupSem = NULL;
	StartupOk = false;
	GroupIndex = -1;
}
TInitialParams::~TInitialParams()
{
	if (StartupSem){
		if (StartupOk){
			ReleaseStartup();
		}
		delete StartupSem;
	}
}
bool TInitialParams::WaitStartup()
{
	if (!StartupSem){
		// create semaphore
		tnstr semname(_t("sem:"));
		semname += StrAppName;	// sem:pdic...
		StartupSem = new TStartupSemaphore(semname);
	}
	const int waittime = 5000;
	if (StartupSem->Wait(waittime)){
		StartupOk = true;
		return true;
	}
	StartupOk = false;
	return false;
}
void TInitialParams::ReleaseStartup()
{
	if (!StartupSem)
		return;
	StartupSem->Release();
}

