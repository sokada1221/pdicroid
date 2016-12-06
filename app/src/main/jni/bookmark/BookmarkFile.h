#ifndef __BookmarkFile_h
#define	__BookmarkFile_h

class TBookmarkItems;
class TBookmarkItem;

class TBookmarkFile {
protected:
	int CurTab;
	TBookmarkItem *TempItem;
public:
	TBookmarkItem &Root;
public:
	TBookmarkFile(TBookmarkItem &root)
		:Root(root)
	{
	}
	bool Load(const tchar *filename);
	bool Save(const tchar *filename);
protected:
	bool LoadOne(TIFile &file, TBookmarkItem &parent, int level);
	bool SaveOne(TOFile &file, TBookmarkItems &items, int level);
};


#endif	// __BookmarkFile_h
