#include "tnlib.h"
#pragma	hdrstop
#include"profile.h"

#include "hgrparr.h"
//#include "dicgrp.h"

#define	DELIM_HISTORY	'\t'

HistoryGroupArray::HistoryGroupArray( int size )
	:HistoryArray( size )
{
	sel = -1;
}

HistoryGroupArray::~HistoryGroupArray( )
{
}

//TOOD: ‚¢‚¸‚êtnlib‚Ö
void HistoryGroupArray::Get(tnstr_vec &strs)
{
	for (int i=0;i<GetCount();i++){
		strs.add((*this)[i].string);
	}
}
void HistoryGroupArray::Set(tnstr_vec &carr)
{
	for ( int i=0;i<carr.get_num();i++ ){
		_Add( new HistoryArrayItem( carr[i] ) );
	}
}

#if 0
void HistoryGroupArray::GetSelName( tnstr &buf )
{
	if ( sel == -1 ){
		buf.set( GetTopString() );
	} else {
		DicNames names;
		dicgrp.GetCurNames( names );
		buf.set( names[ sel ] );
	}
}
#endif

void HistoryGroupArray::Load(TRegKey *key, const tchar *name)
{
	Clear();
	tnstr_vec strs;
	key->Read(name, strs, DELIM_HISTORY);
	Set(strs);
}
void HistoryGroupArray::Save(TRegKey *key, const tchar *name)
{
	tnstr_vec strs;
	Get(strs);
	key->Write(name, strs, DELIM_HISTORY);
}

