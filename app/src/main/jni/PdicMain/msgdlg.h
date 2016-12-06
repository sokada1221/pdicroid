#ifndef	__MSGDLG_H
#define	__MSGDLG_H

struct TRMessage {
	const tchar *Title;
	const tchar *Message;
	const tchar *Button1;
	const tchar *Button2;
	int helpid;
};

#ifdef OWL18
class TMessageDialog : public TNDialog {
	public:
		TRMessage &tr;
	public:
		TMessageDialog( TWindow *parent, TRMessage &tr );
		virtual void SetupWindow( );
		evHandle( ID_BUTTON1, HButton1 );
		evHandle( ID_BUTTON2, HButton2 );
		evHandle( ID_BUTTON3, HButton3 );
		DECLARE_RESPONSE_TABLE( TMessageDialog );
};
#endif

int MessageDialog( HWND hwnd, const tchar *Title, const tchar *Message,
	const tchar *Button1, const tchar *Button2, int helpid=0 );

#endif	// __MSGDLG_H
