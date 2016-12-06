#include "tnlib.h"
#pragma hdrstop
#include "MouseCapBase.h"

TMouseCaptureBase::TMouseCaptureBase(TMouseViewIFBase &view)
	:View(view)
{
	fCapturing = HT_NONE;
}

