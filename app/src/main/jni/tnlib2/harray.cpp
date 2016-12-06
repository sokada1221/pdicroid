#include "tnlib.h"
#pragma	hdrstop
#include	"char.h"
#include	"harray.h"

HistoryArray::HistoryArray( int maxsize )
{
	flags = 0;
	set_maxnum(maxsize);
}

HistoryArray::~HistoryArray()
{
}

void HistoryArray::Add( HistoryArrayItem *obj )
{
	int i = Search( obj );
	if ( i == -1 ){
		_Add( obj );
	} else {
		BringToTop( i );
		delete array[num-1];
		array[num-1] = obj;
	}
}

void HistoryArray::_Add( HistoryArrayItem *obj )
{
	if ( GetCount() == GetMaxNum() )
		Delete( 0 );
	inherited::add( obj );
}

int HistoryArray::Search( HistoryArrayItem *obj )
{
	int r = -1;
	tnstr key = GetKey(obj->string);
	for ( int i=0;i<GetCount();i++ ){
		if ( flags & TRH_IGNORECASE ){
			if ( _tcsicmp( key, GetKey((*this)[i].string) ) == 0 ){
				r = i;
				break;
			}
		} else {
			if ( _tcscmp( key, GetKey((*this)[i].string) ) == 0 ){
				r = i;
				break;
			}
		}
	}
	return r;
}

void HistoryArray::BringToTop( int i )
{
	HistoryArrayItem *p = (HistoryArrayItem*)array[i];
	int j;
	for ( j=i;j<num-1;j++ ){
		array[j] = array[j+1];
	}
	array[j] = p;
}

HistoryArrayItem *HistoryArray::GetTop( )
{
	if ( GetCount() == 0 ){
		return NULL;
	}
	return &(*this)[GetCount()-1];
}

tnstr HistoryArray::GetKey(const tchar *s)
{
	return s;
}
