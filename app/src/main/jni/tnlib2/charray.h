#ifndef	__CHARRAY_H
#define	__CHARRAY_H

//#include	"char.h"
#include "tnstr.h"

#if 0	// いらない？
class CharArray {
protected:
	__Char **array;
	int num;
	int maxnum;
public:
	CharArray( int _maxnum );
	virtual ~CharArray();
//		void set( const tchar *str );
	int add( const tchar *str );
	int add( __Char *obj );
	void del( const tchar *str );
	void _del( int i );
	void clear( void );				//全てのクリア
	void clear( int i )			{ array[i]->clear(); }
	int is_clear( void );
	int is_clear( int i )
		{ return array[i]->is_clear(); }
	int get_num( void )	{return num;}
	const tchar *operator [](int i)	{return *array[i];}
};
#endif

#include "tnarray.h"

class FlexCharArray : public FlexObjectArray<tnstr> {
public:
	FlexCharArray( int slotsize=16 ){}
	int add( const tchar *str )
		{ return FlexObjectArray<tnstr>::add( new tnstr( str ) ); }
	int add( tnstr *obj )
		{ return FlexObjectArray<tnstr>::add( obj ); }
	int insert( int i, const tchar *str )
		{ return FlexObjectArray<tnstr>::insert( i, new tnstr( str ) ); }
	int insert( int i, tnstr *obj )
		{ return FlexObjectArray<tnstr>::insert( i, obj ); }
	int GetAllLength( );	// すべての文字列の長さ(NULLを含める)
							// MULTI_SZ用のバッファを確保する場合は+1する必要がある
#if 0
	void SetMulti( const tchar *multisz );	// MULTI_SZ形式をセットする(セットする前にクリアされない)
	int GetMulti( tchar *buf, int maxlen );	// MULTI_SZ形式を得る,maxlenはNULLを含めた長さ
											// 返り値は取得できた項目数
	tchar *GetMulti( int *len=NULL );		// MULTI_SZ形式で取得し、その先頭ポインターを返す
											// 返り値がNULLでない場合はdeleteする必要がある！
											// バッファの長さが欲しい場合はlenを渡す
#endif
};

#endif	// __CHARRAY_H
