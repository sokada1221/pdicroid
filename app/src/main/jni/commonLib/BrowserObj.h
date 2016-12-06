//---------------------------------------------------------------------------

#ifndef BrowserObjH
#define BrowserObjH
//---------------------------------------------------------------------------

class TBrowserObject {
public:
	TBrowserObject();
	virtual ~TBrowserObject();
	virtual void SetAlignClient() = 0;
	void SetSize(int left, int top, int width, int height);
	virtual void SetTop(int top) = 0;
	virtual void SetLeft(int left) = 0;
	virtual void SetWidth(int width) = 0;
	virtual void SetHeight(int height) = 0;
	virtual void Navigate(const wchar_t *url) = 0;
};

#endif

