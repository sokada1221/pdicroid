#ifndef __SquFrm_h
#define	__SquFrm_h

#include "SquItemView.h"

class TSquareIF;

class TSquareFrame {
protected:
	class TSquUIMain *UIMain;
	class TSquUIWord *UIWord;
public:
	TSquItemView ItemView;
	TSquareIF *squi;
public:
	TSquareFrame(TSquUIMain *main);
	~TSquareFrame();
	class TSquUIMain *GetUIMain(){ return UIMain; }
	class TSquUIWord *GetUIWord(){ return UIWord; }

	class MPdic *GetDic();

	// Message //
	void SetMessage(class TSquareIF *sender, const tchar *msg, int pasttime, int count);
	void SetMessage(class TSquare *sender, const tchar *msg, int pasttime, int count);

	bool IsAutoSpecialSearchEnabled();

	// word list update //
	void ListClear();
	void ListAdd(tnstr *word, class Japa *japa, int dicno, int level);
	void ListInsert(int index, tnstr *word, Japa *japa, int dicno, int level);
	void ListDel(int index);
	void ListDel(int index1, int index2);
};

//TODO: Ç∆ÇËÇ†Ç¶Ç∏Ç±Ç±Ç…ÅBÇ†Ç∆Ç≈ìKêÿÇ»headerÇ÷
class TPopupSearch
{
public:
	__override bool Close(bool wait=false);
	__override bool IsOpened( ) const { return false; }	//TODO: PSW
	void Free(int id);
};

bool DicConvertWizard(TWinControl *parent, const tchar *filename, tnstr &newname, bool typeconvert=false);
int CreateNewDictionary( TWinControl *parent, const tchar *fname, bool inquiry, tnstr *out_filename=NULL );
int CreateNewDictionary( TWinControl *parent, const tchar *fname, bool inquiry, tnstr *out_filename, bool confirmMode );

#endif

