#ifndef __msgbox_h
#define	__msgbox_h

// MessageBox //
int MsgBox(HWND hwnd, LPCTSTR text, LPCTSTR caption, UINT type);	// defined in wpdcom.cpp
int MsgBox(HWND hwnd, int text, int caption, UINT type);	// defined in wpdcom3.cpp
int MsgBox(LPCTSTR text, LPCTSTR caption, UINT type);	// defined in wpdcom.cpp
int MsgBox(int text, int caption, UINT type);	// defined in wpdcom3.cpp

#endif	// __msgbox_h
