#include <windows.h>
#pragma	hdrstop


static HCURSOR hcurWait = NULL;     //* hourglass cursor
static HCURSOR hcurSaved;           //* old cursor
static         iCount = 0;

void ResetHourglass( )
{
	iCount = 0;
	if ( hcurSaved ){
		SetCursor( hcurSaved );
		hcurSaved = NULL;
	}

}

void Hourglass( BOOL bOn )
{
   if (bOn)
   {
	  iCount++;
	  if (!hcurWait)
		 hcurWait = LoadCursor(NULL, IDC_WAIT);
	  if (!hcurSaved)
		 hcurSaved = SetCursor(hcurWait);
   }
   else if (!bOn)
   {
	  if (--iCount < 0 )
		 iCount = 0;
	  else if (!iCount)
	  {
		 SetCursor(hcurSaved);
		 hcurSaved = NULL;
	  }
   }
}


