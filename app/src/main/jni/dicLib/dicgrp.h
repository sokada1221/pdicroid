#ifndef	__DICGRP_H
#define	__DICGRP_H

#ifdef SMALL
#define	MAX_GROUPDIC	8	// グループ辞書の最大数
#else
#define	MAX_GROUPDIC	64	// グループ辞書の最大数
#endif
#define	L_GROUPNAME	40		// グループ名の最大長

#include "dicname.h"

struct DicGroup {
	static int LastId;
	tnstr name;		// Dictionary group name
	int id;			// Dictionary group ID
	DicGroup( const tchar *_name, int _id )
		:name(_name)
		,id(_id)
	{
	}
	DicGroup( const tchar *_name )
		:name(_name)
	{
		id = LastId++;
	}
	DicGroup()
	{
		id = LastId++;
	}
	void Copy(const DicGroup &dg)
	{
		name = dg.name;
		id = dg.id;
	}
	void Assign(const DicGroup &dg)
	{
		name = dg.name;
	}
	DicGroup &operator = ( const DicGroup &dg )
	{
		__assert(id==dg.id);	// 異なる場合は状況に応じてCopyまたはAssignを使用する
		Assign(dg);
		return *this;
	}
	bool IsValid() const
	{
		return !name.empty();
	}
};

// 辞書グループの辞書
class GroupArray : public FlexObjectArray<DicGroup> {
public:
	int GetNum()
		{ return get_num(); }
	int Add( DicGroup *obj, DicNames &names );
	int Insert(int index, DicGroup *obj, DicNames &names);

	// registry operations //
	bool Write(int i, DicNames &names);
	bool Write(const DicGroup &dg, DicNames &names);
	bool Read( int i, DicGroup &dg, DicNames &names, bool fOnlyFileName=false);
	bool Read(const DicGroup &dg, DicNames &names, bool fOnlyFileName=false);

	bool Delete( int i );
	int Modify( int i, DicGroup &dg, DicNames &names );
	void Rename( int i, const tchar *newname );
	int Search( const tchar *grpname, int expindex=-1 );
	int Find(const tchar *grpname)
		{ return Search(grpname); }
	int Find(const DicGroup &dg);
	void Exchange( int i, int j );
	tchar *CreateNewName( tchar *name );
	virtual void SetSel(int no){}
};

class SelGroupArray : public GroupArray {
typedef GroupArray super;
protected:
	int groupsel;
public:
	SelGroupArray();
	void SetSel( int no );
	int GetSel( );
	DicGroup *GetSelGroup()
	{
		int index = GetSel();
		if (index<0 || groupsel>=get_num()){
			return NULL;
		}
		return &(*this)[index];
	}
	const tchar *GetSelGroupName()
	{
		int groupsel = GetSel();
		return groupsel >= 0 ? (const tchar *)(*this)[groupsel].name : _T("");
	}
	DicGroup &GetCur()		// この関数は確実にgroupselが-1で無いときのみ利用可能
	{
		__assert(GetSel()>=0);
		return (*this)[ GetSel() ];
	}
	bool GetCurNames( DicNames &names, bool fOnlyFileName=false);
	const tchar *GetCurGroupName( );		// 現在の辞書グループ名
};


int ReadGroupDicList( );

class TRegKey;

// DicNamesを与えた場合、所有権は移らないので呼出側で削除！
//BOOL MakeNewGroupDic( TWindow *parent, int &inx, BOOL &fModify, DicNames *preset=NULL, const tchar *groupname=NULL );
//BOOL ModifyGroupDicProc( TWindow *parent, int i, BOOL &fDeleted );
bool WriteGroupDic(DicGroup &dg, DicNames &names, TRegKey *reg=NULL);
bool ReadGroupDic( const tchar *grpname, DicGroup &dg, DicNames &names, bool fOnlyFileName=false, TRegKey *reg=NULL );
bool ReadGroupDicFiles(const tchar *grpname, tnstr_vec &filenames);
void WriteGroupNames();
BOOL RewriteGroupDic( );

extern SelGroupArray dicgrp;
//extern HistoryArray dicgrphist;
//extern bool dicgrphistfixed;	// コマンドライン指定による辞書グループ切り替えの指定

bool GetCurDicNames( DicNames &names );		// 現在の辞書グループリストを得る
void SetCurDicNames( DicNames *names );		// 一時的な辞書グループリストをセットする(所有権は渡らない）
											// NULLをセットすると元に戻る
class TRegKey *CreateSectionU(const tchar *name, TRegKey *groupKey=NULL);
class TRegKey *CreateGroupSection( const tchar *grpname, int &uniq );

bool ValidateGroupName(const tchar *name);

bool CopyUniqData(const tchar *sname, const tchar *dname);

// Dictionary Version //
bool ValidateUpgradeKey(const tchar *key);
bool ValidateVersion(const tchar *s);
bool ReadUpgradeKeys(tnstr_map *ukey_to_name, tnstr_map *ukey_to_ver);

#endif	// __DICGRP_H

