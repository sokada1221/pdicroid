//---------------------------------------------------------------------------

#include "pdclass.h"
#pragma hdrstop

#include "utydefs.h"
#include "UtyDicView.h"
#include "LevelConvertingDlg.h"
#include "DicConvertingDlg.h"
#include "DicCreateDlg.h"
#include "winmsg.h"
#include "pdprof.h"
#include "LineEditDlg.h"
#include "idh.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//
// base class
//
int TUtyDicView::CreateNewDictionary( TWinControl *parent, const tchar *fname, bool inquiry, tnstr *out_filename )
{
	return ::CreateNewDictionary(parent, fname, inquiry, out_filename);
}

int TUtyDicView::MessageDialog( TWinControl *parent, const tchar *title, const tchar *message )
{
	return ::MessageDialog( parent->Handle, title, message );
}

void TUtyDicView::ErrorMessage( TWinControl *parent, int msgno, const tchar *str, const tchar *_title )
{
	return ::ErrorMessage(parent->Handle, msgno, str, _title);
}

int TUtyDicView::SelectWord( const tchar *word, int slev, int dlev )
{
	return mrAuto;	// auto
}
int TUtyDicView::SelectJapa( const tchar *word, class Japa &srcjapa, Japa &destjapa, Japa &mergejapa, bool over, class Pdic *_bdic )
{
	return mrAuto;	// auto
}

//
// TUtyDicViewVcl class
//
int TUtyDicViewVcl::SelectWord( const tchar *word, int slevel, int dlevel )
{
	return ((TLevelConvertingDialog*)cvtdlg)->SelectWord( word, slevel, dlevel );
}

int TUtyDicViewVcl::SelectJapa( const tchar *word, class Japa &srcjp, Japa &destj, Japa &newj, bool overflg, Pdic *pdic )
{
	return ((TDicConvertingDialog*)cvtdlg)->SelectJapa( word, srcjp, destj, newj, overflg, pdic );
}

// if return null, then cancel.
tnstr TUtyDicViewVcl::GetFLinkCPath(const tchar *error_filename, bool save)
{
	tnstr path = prof.ReadString(PFS_CONVERT, PFS_FLINKCPATH, _t(""));

	if (FLinkCPathSelected && !error_filename && path.exist())
		return path;

	FLinkCPathSelected = true;
	__assert(cvtdlg);
	for (;;){
		tnstr msg;
		if (error_filename){
			if (save){
				msg = tnsprintf(_LT(MSG_CouldNotSaveFile), error_filename);
			} else {
				msg = tnsprintf(_LT(MSG_NOTFINDFILE), error_filename);
			}
		} else {
			msg = _LT(MSG_SelectFolderForEmbFileLink);
		}
		if (!LineEdit( cvtdlg, _LT(CAP_SelectFolderForEmbFileLink), path, -1, msg, HConvertFLinkCPath, true, DT_FOLDER)){
			if (MsgBox( cvtdlg->Handle, MSG_DOYOUABORTCONVERT, CAP_DICCONVERT, MB_YESNO)==IDYES){
				return NULL;
			}
			continue;
		}
		if (path.exist())
			break;
	}
	prof.WriteString(PFS_CONVERT, PFS_FLINKCPATH, path);
	return path;
}


