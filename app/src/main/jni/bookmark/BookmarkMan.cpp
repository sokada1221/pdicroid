//---------------------------------------------------------------------------

#include "tnlib.h"
#pragma hdrstop

#include "BookmarkMan.h"
#include "BookmarkFile.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

TBookmarkMan Bookmark;

TBookmarkItem::~TBookmarkItem()
{
	if (Child){
#if USE_PARENT
		Child->ReleaseParent();
#endif
		delete Child;
	}
#if USE_PARENT
	if (Parent)
		Parent->Release(this);
#endif
}

TBookmarkItems *TBookmarkItem::CreateChild()
{
	if (Child)
		return Child;
	return Child = new TBookmarkItems();
}

void TBookmarkItem::ReleaseAll()
{
	if (!Child)
		return;
	Child->ReleaseAll();
	delete Child;
	Child = NULL;
}

#if USE_PARENT
// é©ï™Ç∆ÇªÇÍà»â∫ÇÃäKëwÇdiscardÇ∑ÇÈ
void TBookmarkItem::Release()
{
	if (Parent){
		Parent->Release(this);
		Parent = NULL;
	}
	ReleaseAll();
}

void TBookmarkItem::Release(TBookmarkItem *item)
{
	if (Child)
		Child->Release(item);
}
#endif

void TBookmarkItem::ReleaseChild()
{
	if (Child){
		Child->ReleaseAll();
		delete Child;
		Child = NULL;
	}
}

void TBookmarkItem::DeleteChild()
{
	if (Child){
		delete Child;
		Child = NULL;
	}
}

#if 0
int TBookmarkItem::GetCount() const
{
	return Child ? Child->get_num() : 0;
}

void TBookmarkItem::DeleteChild(TBookmarkItem *item)
{
	if (!Child)
		return;
	Child->DeleteChild(item);
}
#endif

// TBookmarkItems //

// äKëwâ∫Ç‡ÇµÇ¡Ç©ÇËdiscard
void TBookmarkItems::ReleaseAll()
{
	for (int i=0;i<get_num();i++){
		(*this)[i].ReleaseAll();
	}
	discard();
}

#if USE_PARENT
int TBookmarkItems::FindChild(TBookmarkItem *item) const
{
	for (int i=0;i<get_num();i++){
		if (&(*this)[i] == item){
			return i;
		}
	}
	return -1;
}
void TBookmarkItems::Release(TBookmarkItem *item)
{
	int index = FindChild(item);
	if (index<0)
		return;
	discard(index);
}

void TBookmarkItems::ReleaseParent()
{
	for (int i=0;i<get_num();i++){
		(*this)[i].Parent = NULL;
	}	
}
#endif

int TBookmarkItems::SetMarks(const tchar *word, bool flag)
{
	int count = 0;
	for (int i=0;i<get_num();i++){
		TBookmarkItem &item = (*this)[i];
		if (!_tcscmp(item.Word, word)){
			item.SetMark(flag);
			count++;
		}
		if (item.Child && item.Child->get_num())
			count += item.Child->SetMarks(word, flag);
	}
	return count;
}

void TBookmarkItems::GetWords(tnstr_vec &words, map<tnstr, bool> *dup, bool only_marked)
{
	for (int i=0;i<get_num();i++){
		TBookmarkItem &item = (*this)[i];
		if (!only_marked || item.IsMarked()){
			if (dup){
				if (dup->count(item.Word))
					continue;	// duplicated
				(*dup)[item.Word] = true;
			}
			words.push_back(item.Word);
		}
		if (item.Child && item.Child->get_num())
			item.Child->GetWords(words, dup, only_marked);
	}
}

//TODO: slow version
bool TBookmarkItems::IsMarked(const tchar *word)
{
	for (int i=0;i<get_num();i++){
		TBookmarkItem &item = (*this)[i];
		if (item.IsMarked()){
			if (!_tcscmp(item.Word, word)){
				return true;
			}
		}
		if (item.Child && item.Child->get_num())
			if (item.Child->IsMarked(word))
				return true;
	}
	return false;
}

// TBookmarkMan class //
TBookmarkMan::TBookmarkMan()
	:Root(NULL)
{
	UI = NULL;
	Dirty = false;
}

TBookmarkMan::~TBookmarkMan()
{
	if (UI)
		delete UI;
}
inline bool TBookmarkMan::IsLoaded() const
{
	return Root.Child ? true : false;
}
void TBookmarkMan::FileName(const tchar *filename)
{
	_FileName = filename;
}

tnstr TBookmarkMan::FileName() const
{
	return _FileName;
}

bool TBookmarkMan::Load()
{
	if (IsLoaded())
		return true;	// already loaded

	TBookmarkFile bf(Root);
	if (!bf.Load(FileName())){
		return false;
	}
	if (AddQueue.get_num()>0){
		Root.CreateChild();
		// temporary load
		for (int i=0;i<AddQueue.get_num();i++){
			Root.Child->add(new TBookmarkItem(NULL, AddQueue[i]));
		}
	}
	return true;
}
bool TBookmarkMan::Save()
{
	//__assert(Root.Child);	// should be loaded(except no data)
	TBookmarkFile bf(Root);
	if (bf.Save(FileName())){
		Dirty = false;
		AddQueue.clear();
		return true;
	}
	return false;
}
bool TBookmarkMan::SaveIfDirty()
{
	if (!Dirty)
		return false;	// not dirty

	if (IsLoaded()){
		// already loaded
	} else {
		if (!Load())
			return false;	// failed toload
	}
	return Save();
}
TBookmarkItem *TBookmarkMan::Add(const tchar *word, bool mem)
{
	timex_t t;
	xtime(&t);
	TBookmarkItem *item = new TBookmarkItem(NULL, word, t, mem?BMF_MARK:0, NULL);
	if (IsLoaded()){
		Root.Child->add(item);
	} else {
		AddQueue.add(item);
	}
	Dirty = true;
	return item;
}
// ìØàÍíPåÍÇ™Ç†ÇÍÇŒÇ∑Ç◊ÇƒmarkÇ∑ÇÈ
// ë∂ç›ÇµÇ»ÇØÇÍÇŒí«â¡Ç∑ÇÈ
void TBookmarkMan::SetMarks(const tchar *word, bool on)
{
	if (UI){
		UI->SetMarks(word, on);
		return;
	}

	Load();
	int count;
	if (Root.Child)
		count = Root.Child->SetMarks(word, on);
	else
		count = 0;

	if (count){
		Dirty = true;
	} else {
		// not exist
		// onÇÃèÍçáÇÃÇ›add
		if (on){
			Add(word, on);
			Dirty = true;
		}
	}
}

void TBookmarkMan::GetWords(tnstr_vec &words, bool only_marked)
{
	map<tnstr, bool> dup;

	if (Root.Child)
		Root.Child->GetWords(words, &dup, only_marked);
}

bool TBookmarkMan::IsMarked(const tchar *word)
{
	if (UI){
		return UI->IsMarked(word);
	}
	Load();
	if (!Root.Child)
		return false;
	return Root.Child->IsMarked(word);
}



// Helpers //
#include "japa.h"
#include "pdprof.h"
void BM_JSetMark(const tchar *word, class Japa &japa)
{
	if (Bookmark.IsMarked(word)){
		japa.SetMemory();
	} else {
		if (!prof.IsOldMemory()){
			japa.ClearMemory();
		}
	}
}

#include "histfile.h"

// bookmark loader param
struct TBMLParam {
	TWordHistory &out;		// [O]
	int option;				// [I]
	bool alphasort;

	TFastReadHistory fast;	// [W]

	TBMLParam(TWordHistory &items, bool _alphasort)
		:fast(items)
		,out(items)
		,alphasort(_alphasort)
	{
	}
};

void LoadBookmarkOne(TBookmarkItem &item, TBMLParam &p);
void LoadBookmark(class TWordHistory &out, bool alphasort, bool reverse)
{
	Bookmark.Load();
	TBMLParam param(out, alphasort);
	LoadBookmarkOne(Bookmark.Root, param);

#if 0
	if (sort & HF_SORT_FREQ){
		//TODO: SortByNumReversed()ÇçÏÇ¡ÇƒçÇë¨âª
		WordHist.SortByNum();
	} else
#endif
	{
		param.fast.Sort();
	}

	if (reverse)
		out.SortReverse();
}

void LoadBookmarkOne(TBookmarkItem &item, TBMLParam &p)
{
	if (!item.GetChild())
		return;
	TBookmarkItems &items = *item.GetChild();
	for (int i=0;i<items.get_num();i++){
		if (p.alphasort){
			p.out.AddAlphaSorted(items[i].Word);
		} else {
			bool pre_reverse = false;	//TODO: what is used for?
			p.fast.Add(items[i].Word, pre_reverse);
		}
		//p.out.add(new HISTFREQ(items[i].Word));
		if (items[i].GetChild()){
			LoadBookmarkOne(items[i], p);
		}
	}
	
}

