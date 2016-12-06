//---------------------------------------------------------------------------
/*
Abstract:
Interface class to communicate with the user appllication from PopupDll/Application.


*/

#ifndef IPopupSearchH
#define IPopupSearchH
//---------------------------------------------------------------------------

// This class is a base class for the Popup Search model class.
class IPopupSearch {
public:
	IPopupSearch(){}
	virtual ~IPopupSearch(){}
	virtual void AssignParameters(IPopupSearch *ps) = 0;
	virtual IPopupSearch *GetParent() const = 0;
	virtual void SetParent(IPopupSearch *parent) = 0;
	virtual IPopupSearch *GetRoot() = 0;
	virtual bool IsYourWindow( HWND hwnd, bool recur, HWND stop ) const = 0;
	virtual bool IsOpened() const = 0;
	virtual HWND GetChildHandle() const = 0;
	virtual IPopupSearch *GetChild() const = 0;
	virtual void SetChild(IPopupSearch *ps) = 0;
	virtual bool Close(bool wait=false) = 0;
	virtual void CloseNotify() = 0;
	virtual void GetNums(int &entry, int &numHitWords) = 0;
	virtual int GetStyle() = 0;
	virtual void SetStyle(int style) = 0;
	virtual const tchar *GetCompleteWord() = 0;
};

#endif

