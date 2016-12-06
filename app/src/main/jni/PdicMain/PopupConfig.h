//---------------------------------------------------------------------------

#ifndef PopupConfigH
#define PopupConfigH
//---------------------------------------------------------------------------

#include "PopupDefs.h"

struct POPUPCONFIG {
	bool fCaseIgnore;
	bool fSuffix;
//	BOOL fReplaceOnes;
#if USE_UKSPELL
	bool fUKSpell;	// 英国式->米国式綴り変換
#endif
#ifndef LIGHT
	bool fUmlaut;
	bool fDeutch;
#endif
//	BOOL fWordDelim;

	bool fLongest;	// 一番長い単語で検索

//	bool fMultiByte;

#ifndef LIGHT
	bool fShowLevel;
	bool fShowPron;	// 発音記号も表示
	bool fShowExp;
	bool fShowObject;
	bool fPlay;		// ヒット時の音声再生
	bool fTTSPlay;
#endif
#ifndef SMALL
	bool fFixWidth;
	bool fFixPos;
	bool fToFront;
	bool fSetFocus;
	int Alpha;
#endif

	bool fToMainSrchWord;	// ヒット時 to main で使用する単語は検索に使った単語

	int GetOption();
	int GetViewFlags();

	void LoadProfile();
	void SaveProfile();
	static class TRegKey *GetSectionKey();
};

extern POPUPCONFIG PopupConfig;

#endif

