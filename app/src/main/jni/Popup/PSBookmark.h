//---------------------------------------------------------------------------

#ifndef PSBookmarkH
#define PSBookmarkH

#include "defs.h"

enum ePSBMStyle {
	PSBM_BOLD = 0x1,
    PSBM_ITALIC = 0x2,
    PSBM_UNDERLINE = 0x4,
    PSBM_STRIKEOUT = 0x8,
};

struct TPSBMFile {
	tnstr filename;
	int position;
	timex_t date;
	tnstr remotename;
	tnstr revision;
};

struct TPSBMItem {
	int position;
	int length;
	int style;
	unsigned color;	// AARRGGBB
	tnstr marked_word;
	tnstr comment;
};

void OpenPSBookmark(const tchar *filename);
void ClosePSBookmark();

void SavePSBookmark(const tchar *filename, const TPSBMItem &item);
bool SavePSFileInfo(const tchar *filename, int position, const tchar *revision);
bool DeletePSBookmark(const tchar *filename, const TPSBMItem &item);
void LoadPSBookmark(const tchar *filename, vector<TPSBMItem> &items);
bool LoadPSBookmarkItem(const tchar *filename, int position, TPSBMItem &items);
void LoadPSBookmarkFiles(tnstr_vec &files, bool sort);
bool LoadPSFileInfo(const tchar *filename, int &position, tnstr *revision);
const tchar *GetPSBookmarkFileName();

//---------------------------------------------------------------------------
#endif

