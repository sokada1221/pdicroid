//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "BrowserObj.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

TBrowserObject::TBrowserObject()
{
}
TBrowserObject::~TBrowserObject()
{
}
void TBrowserObject::SetSize(int left, int top, int width, int height)
{
	SetLeft(left);
	SetTop(top);
	SetWidth(width);
	SetHeight(height);
}


