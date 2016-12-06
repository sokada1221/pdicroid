#include	<windows.h>
#include	"bc.h"
#pragma	hdrstop
#include	<commdlg.h>
#include	"comfont.h"
#include	"tnwinx.h"

ChooseFontDialog::ChooseFontDialog( HWND hwnd, LOGFONT *lf )
{
	cf.hwndOwner = hwnd;
	cf.lpLogFont = lf;
	Initialize();
	cf.Flags |= CF_SCREENFONTS;
}

ChooseFontDialog::ChooseFontDialog( HWND hwnd, LOGFONT *lf, HDC hdc )
{
	cf.hwndOwner = hwnd;
	cf.lpLogFont = lf;
	Initialize();
	UseWYSIWYG( hdc );
}

// hDCÇÕèâä˙âªÇ≥ÇÍÇÈÇÃÇ≈íçà”ÅIÅI
void ChooseFontDialog::Initialize( )
{
	HWND hwnd = cf.hwndOwner;
	LPLOGFONT lf = cf.lpLogFont;

	memset( &cf, 0, sizeof( CHOOSEFONT ) );
	cf.lStructSize = sizeof( CHOOSEFONT );
	cf.hwndOwner = hwnd;
	cf.lpLogFont = lf;

	cf.Flags |= CF_INITTOLOGFONTSTRUCT;
}

BOOL ChooseFontDialog::Execute( )
{
	return ChooseFont( &cf ) != 0;
}

void ChooseFontDialog::SetLimitSize( int min, int max )
{
	cf.nSizeMin = min;
	cf.nSizeMax = max;
	cf.Flags |= CF_LIMITSIZE;
}

void ChooseFontDialog::SetTemplate( LPCTSTR lpTemplateName )
{
	cf.Flags |= CF_ENABLETEMPLATE;
	cf.lpTemplateName = lpTemplateName;
//	cf.hInstance = GetWindow_HINSTANCE( cf.hwndOwner );
	cf.hInstance = hTNInstance;
}

void ChooseFontDialog::SetTemplate( HINSTANCE hInstance )
{
	cf.Flags |= CF_ENABLETEMPLATEHANDLE;
	cf.hInstance = hInstance;
}

void ChooseFontDialog::SetHook( UINT (CALLBACK *lpfnHook)(HWND, UINT, WPARAM, LPARAM), LPARAM lCustData )
{
	cf.Flags |= CF_ENABLEHOOK;
	cf.lpfnHook = lpfnHook;
	if ( lCustData ){
		cf.lCustData = lCustData;
	} else {
		cf.lCustData = (LONG)this;
	}
}

void ChooseFontDialog::UseWYSIWYG( HDC hdc )
{
	cf.Flags |= CF_WYSIWYG | CF_BOTH | CF_SCALABLEONLY;
	cf.hDC = hdc;
}

