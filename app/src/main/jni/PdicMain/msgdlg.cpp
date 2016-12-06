#include	"tnlib.h"
#pragma	hdrstop
#include	"id.h"
#include	"msgdlg.h"
#include	"helps.h"
#include	"wpdcom.h"

static BOOL CALLBACK DialogProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	static TRMessage *tr;
	switch ( msg ){
		case WM_INITDIALOG:
			MoveCenter(hwnd, NULL);
			tr = (TRMessage*)lParam;
			SetWindowText( hwnd, tr->Title );
			SetWindowText( GetDlgItem( hwnd, ID_MESSAGE ), tr->Message );
			if ( tr->Button1[0] != '~' ){
				SetWindowText( GetDlgItem( hwnd, ID_BUTTON1 ), tr->Button1 );
				EnableWindow( GetDlgItem( hwnd, ID_BUTTON1 ), TRUE );
			} else {
				EnableWindow( GetDlgItem( hwnd, ID_BUTTON1 ), FALSE );
				SetWindowText( GetDlgItem( hwnd, ID_BUTTON1 ), tr->Button1+1 );
			}
			if ( tr->Button2[0] != '~' ){
				EnableWindow( GetDlgItem( hwnd, ID_BUTTON2 ), TRUE );
				SetWindowText( GetDlgItem( hwnd, ID_BUTTON2 ), tr->Button2 );
			} else {
				EnableWindow( GetDlgItem( hwnd, ID_BUTTON2 ), FALSE );
				SetWindowText( GetDlgItem( hwnd, ID_BUTTON2 ), tr->Button2+1 );
			}
#ifndef SMALL
			if ( tr->helpid ){
				SetWindowText( GetDlgItem( hwnd, ID_BUTTON3 ), _T("Help") );
			}
#endif
			return TRUE;
		case WM_KEYDOWN:
			switch ( wParam ){
				case VK_ESCAPE:
					EndDialog( hwnd, IDCANCEL );
					break;
#ifndef SMALL
				case VK_F1:
					if ( tr->helpid )
						HelpContext( hwnd, tr->helpid );
					break;
#endif
			}
			break;
		case WM_COMMAND:
			switch ( LOWORD( wParam ) ){
				case ID_BUTTON1:
				case ID_BUTTON2:
					EndDialog( hwnd, LOWORD( wParam ) );
					break;
				case ID_BUTTON3:
#ifndef SMALL
					if ( tr->helpid )
						HelpContext( hwnd, tr->helpid );
					else
#endif
						EndDialog( hwnd, IDCANCEL );
					break;
				case IDCANCEL:
					EndDialog( hwnd, IDCANCEL );
					break;

			}
			break;
	}
	return FALSE;
}


int MessageDialog( HWND hwnd, const tchar *Title, const tchar *Message,
	const tchar *Button1, const tchar *Button2, int helpid )
{
	TRMessage tr;
	tr.Title = Title;
	tr.Message = Message;
	tr.Button1 = Button1;
	tr.Button2 = Button2;
	tr.helpid = helpid;
//	return new TMessageDialog( parent, tr )->Execute();
	return DialogBoxParam( hTNInstance, MAKEINTRESOURCE(MESSAGEDLG), hwnd, DialogProc, (LPARAM)&tr );
}


#if 0

DEFINE_RESPONSE_TABLE1( TMessageDialog, TNDialog )
	_EV_HANDLE_CLICKED( ID_BUTTON1, HButton1 )
	_EV_HANDLE_CLICKED( ID_BUTTON2, HButton2 )
	_EV_HANDLE_CLICKED( ID_BUTTON3, HButton3 )
END_RESPONSE_TABLE;

TMessageDialog::TMessageDialog( TWindow *parent, TRMessage &_tr )
	:TNDialog( parent, MAKEINTRESOURCE(MESSAGEDLG) )
	,tr( _tr )
{
}

void TMessageDialog::SetupWindow( )
{
	TNDialog::SetupWindow( );
	SetWindowText( tr.Title );
	::SetWindowText( GetItemHandle( ID_MESSAGE ), tr.Message );
	::SetWindowText( GetItemHandle( ID_BUTTON1 ), tr.Button1 );
	::SetWindowText( GetItemHandle( ID_BUTTON2 ), tr.Button2 );
	if ( tr.helpid ){
		::SetWindowText( GetItemHandle( ID_BUTTON3 ), "Help" );
	}
}


void TMessageDialog::HButton1( )
{
	CloseWindow( ID_BUTTON1 );
}

void TMessageDialog::HButton2( )
{
	CloseWindow( ID_BUTTON2 );
}

void TMessageDialog::HButton3( )
{
	if ( tr.helpid )
		HelpContext( HWindow, tr.helpid );
	else
		CloseWindow( IDCANCEL );
}

#endif
