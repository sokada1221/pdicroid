#ifndef	__HARRAY_H
#define	__HARRAY_H

#include "tnstr.h"

#define	TRH_IGNORECASE	1

class HistoryArrayItem {
public:
	HistoryArrayItem( const tchar *_string )
		:string( _string ){}
	tnstr string;
};

// ƒqƒXƒgƒŠ”z—ñ
class HistoryArray : FlexObjectArray<HistoryArrayItem> {
typedef FlexObjectArray<HistoryArrayItem> inherited;
protected:
	int flags;
public:
	HistoryArray( int maxsize );
	virtual ~HistoryArray();
	void Add( HistoryArrayItem *obj );
	void AddString( const tchar *string )
		{ Add( new HistoryArrayItem( string ) ); }
	void Clear( )
		{ inherited::clear(); }
	void Delete( int i )
		{ inherited::del(i); }
	void BringToTop( int i );
	virtual int Search( HistoryArrayItem *obj );
	HistoryArrayItem *GetTop( );
	const tchar *GetTopString()
		{ return GetCount() ? (const tchar*)GetTop()->string : NULL; }
	int GetMaxNum() const
		{ return inherited::get_maxnum(); }
	HistoryArrayItem &operator [](int i)
		{ return *(HistoryArrayItem*)array[i]; }
	int GetCount( ) const
		{ return inherited::get_num(); }
	void DeleteTop( )
		{ Delete( inherited::get_num() - 1 ); }
	void DeleteBottom( )
		{ Delete( 0 ); }

	// item parser
	virtual tnstr GetKey(const tchar *s);
protected:
	void _Add( HistoryArrayItem *obj );
};

#endif	// __HARRAY_H
