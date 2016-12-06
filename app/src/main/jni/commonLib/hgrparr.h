#ifndef __HGRPARR_H
#define	__HGRPARR_H

#include "harray.h"

// ヒストリ配列 or 辞書グループ用配列
class HistoryGroupArray : public HistoryArray {
typedef HistoryArray inherited;
protected:
	UINT sel;	// sel が -1 のときはHistoryArrayとして動作(default)
public:
	HistoryGroupArray( int size );
	~HistoryGroupArray();
	UINT GetSel( ) const
		{ return sel; }
	void SetSel( UINT _sel )
		{ sel = _sel; }
	void Get(tnstr_vec &strs);
	void Set(tnstr_vec &array);
	void GetSelName( tnstr &buf );

	void Load(class TRegKey *key, const tchar *name);
	void Save(class TRegKey *key, const tchar *name);
};


#endif	__HGRPARR_H

