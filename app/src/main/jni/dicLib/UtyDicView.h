//---------------------------------------------------------------------------

#ifndef UtyDicViewH
#define UtyDicViewH
//---------------------------------------------------------------------------

class TUtyDicView {
protected:
	class TConvertingDialog *cvtdlg;
	bool FLinkCPathSelected;
public:
	TUtyDicView()
	{
		FLinkCPathSelected = false;
	}
	void Set(TConvertingDialog *dlg)
		{ cvtdlg = dlg; }

	virtual int CreateNewDictionary( TWinControl *parent, const tchar *fname, bool inquiry, tnstr *out_filename=NULL );
	virtual int MessageDialog( TWinControl *parent, const tchar *title, const tchar *message );
	virtual void ErrorMessage( TWinControl *parent, int msgno, const tchar *str=NULL, const tchar *_title=0 );

	// for Pdic2
	virtual int SelectWord( const tchar *word, int slevel, int dlevel );
	virtual int SelectJapa( const tchar *word, class Japa &srcjapa, Japa &destjapa, Japa &mergejapa, bool over, class Pdic *_bdic );

	// for CSV/TSV
	virtual tnstr GetFLinkCPath(const tchar *error_filename=NULL, bool save=false) = 0;
};

class TUtyDicViewVcl : public TUtyDicView {
typedef TUtyDicView super;
public:
	TUtyDicViewVcl()
	{
	}

	// for Pdic2
	virtual int SelectWord( const tchar *word, int slevel, int dlevel );
	virtual int SelectJapa( const tchar *word, class Japa &srcjapa, Japa &destjapa, Japa &mergejapa, bool over, class Pdic *_bdic );

	// for CSV/TSV
	virtual tnstr GetFLinkCPath(const tchar *error_filename=NULL, bool save=false);
};

#endif

