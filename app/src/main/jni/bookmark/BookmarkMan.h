//---------------------------------------------------------------------------

#ifndef BookmarkManH
#define BookmarkManH
//---------------------------------------------------------------------------

//Note:
//	TBookarmkItem objectそのものをdeleteしたい場合は、
//	必ず親のTBookmarkItem.DeleteChild()で削除すること。
//	でなければ、USE_PARENT=1にする必要あり。

#define	USE_PARENT	0

#if USE_PARENT
#else
#endif

//#define	BMF_LEVEL	0x000F
#define	BMF_MARK		0x0020
//#define	BMF_EDIT	0x0040

class TBookmarkItems;

class TBookmarkUI {
public:
	virtual ~TBookmarkUI(){}
	virtual void SetMarks(const tchar *word, bool mark) = 0;
	//virtual void AddWord(const tchar *word, bool mark){}
	virtual bool IsMarked(const tchar *word) = 0;
};

class TBookmarkItem {
friend class TBookmarkItems;
friend class TBookmarkMan;
friend class TBookmarkFile;
public:
	// data format: [word]\t[date]\t[comment]/*\t[mark]\t[title]*/
	tnstr Word;
	timex_t Date;
	int Flags;
	tnstr Comment;
	//tnstr title;

#if USE_PARENT
	TBookmarkItem *Parent;	// reference
#endif
protected:
	TBookmarkItems *Child;	// own
public:
	TBookmarkItem(TBookmarkItem *parent)
#if USE_PARENT
		:Parent(parent)
#endif
	{
		Flags = 0;
		Child = NULL;
	}
	TBookmarkItem(TBookmarkItem *parent, TBookmarkItem &o)
		:Word(o.Word)
		,Date(o.Date)
		,Flags(o.Flags)
		,Comment(o.Comment)
#if USE_PARENT
		,Parent(parent)
#endif
	{
		Child = NULL;
	}
	TBookmarkItem(TBookmarkItem *parent, const tchar *word, timex_t date, int flags, const tchar *comment)
		:Word(word)
		,Date(date)
		,Flags(flags)
		,Comment(comment)
#if USE_PARENT
		,Parent(parent)
#endif
	{
		Child = NULL;
	}
	~TBookmarkItem();
	TBookmarkItems *CreateChild();
	TBookmarkItems *GetChild() const { return Child; }
	void ReleaseAll();
	void Release();
	void ReleaseChild();
	void DeleteChild();
	int GetCount() const;
	bool IsMarked() const
		{ return Flags & BMF_MARK; }
	void SetMark(bool f)
		{ Flags = (Flags&~BMF_MARK) | (f?BMF_MARK:0); }

protected:
	void Release(TBookmarkItem *item);
};

class TBookmarkItems : public FlexObjectArray<TBookmarkItem> {
public:
	TBookmarkItems()
	{
	}
	void ReleaseAll();
	int FindChild(TBookmarkItem *item) const;
	void Release(TBookmarkItem *item);
	void ReleaseParent();

	int SetMarks(const tchar *word, bool flag);
	void GetWords(tnstr_vec &words, map<tnstr, bool> *dup, bool only_marked);
	bool IsMarked(const tchar *word);
};

class TBookmarkMan {
protected:
	tnstr _FileName;	// do not access this directly.
	TBookmarkUI *UI;	// owner
public:
	TBookmarkItem Root;
	bool Dirty;
public:
	TBookmarkMan();
	~TBookmarkMan();
	void SetUI(TBookmarkUI *ui)
		{ if (UI) delete UI; UI = ui; }
	bool IsLoaded() const;
	void FileName(const tchar *filename);
	tnstr FileName() const;
	bool Load();
	bool Save();
	bool SaveIfDirty();

protected:
	FlexObjectArray<TBookmarkItem> AddQueue;
public:
	TBookmarkItem *Add(const tchar *word, bool mem);
	void SetMarks(const tchar *word, bool on);

	void GetWords(tnstr_vec &words, bool only_marked);

	bool IsMarked(const tchar *word);
};

extern TBookmarkMan Bookmark;

#include "pdconfig.h"

#if USE_BM
inline void BM_SetMarks(const tchar *word, bool flag) { Bookmark.SetMarks(word, flag); }
void BM_JSetMark(const tchar *word, class Japa &japa);
//#define	BM_AttrSetMark(word, attr)	do { if (Bookmark.IsMarked(word)) attr |= WA_MEMORY; } while (0);
#define	BM_AttrSetMark(word, attr)	do { \
	if (prof.IsOldMemory()){ if (Bookmark.IsMarked(word)) attr |= WA_MEMORY; } \
	else { attr = (attr & ~WA_MEMORY) | (Bookmark.IsMarked(word) ? WA_MEMORY : 0); } \
	} while (0);

#else
#define	BM_SetMarks(word, flag)
#define	BM_JSetMark(word, japa)
#define	BM_AttrSetMark(word, attr)
#endif

// for Search Bookmark
void LoadBookmark(class TWordHistory &items, bool alphasort, bool reverse);

#endif

