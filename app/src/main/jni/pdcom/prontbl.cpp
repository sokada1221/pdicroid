#include "tnlib.h"
#pragma	hdrstop
#include "prontbl.h"
#include "pdstrlib.h"

static int fcmp( const void *o1, const void *o2 )
{
	return _tcscmp( (*(PRONTABLE**)o1)->key, (*(PRONTABLE**)o2)->key );
}

PronTable::PronTable()
{
}

PronTable::~PronTable()
{
}

int PronTable::bsearch( const tchar *key )
{
	PRONTABLE pt, *ppt;
	pt.key.set( key );
	ppt = &pt;
	PRONTABLE** o = (PRONTABLE**)_bsearch( &ppt, array, num, sizeof( PRONTABLE* ), fcmp );
	return ( (unsigned int)(o) - (unsigned int)(array) ) / sizeof( PRONTABLE* );
}

// -2‚ð•Ô‚·‚Æd•¡
int PronTable::add( const tchar *key, const tchar *code )
{
	PRONTABLE *pt = new PRONTABLE;
	pt->key.set( key );
	pt->code.set( code );

	if ( num == 0 ){
		FlexObjectArray<PRONTABLE>::add( pt );
		return 0;
	}
	int i = bsearch( key );
	BOOL f = FALSE;
	if ( i < num ){
		if ( !_tcscmp( (*this)[i].key, key ) ){
			f = TRUE;
		}
	}
	if ( insert( i, pt ) == -1 ){
		delete pt;
		return -1;
	}
	return f ? -2 : 0;
}

int PronTable::search( const tchar *key )
{
	if ( !num ) return -1;
	int i = bsearch( key );
	if ( i < num && !_tcsncmp( (*this)[i].key, key, _tcslen((*this)[i].key) ) )
		return i;
	// key‘S‘Ì‚Åbsearch‚ð‚©‚¯‚Ä‚¢‚é‚½‚ßA
	// just‚Åhit‚µ‚È‚¢ê‡‚ª‚ ‚é
	i--;
	for(;i>=0;i--){
		PRONTABLE &p = (*this)[i];
		if ( p.key[0] != key[0] )
			break;	// ‚à‚¤ˆê’v‚µ‚È‚¢
		if ( !_tcsncmp( p.key, key, _tcslen(p.key) ) )
			return i;
	}
	return -1;
}

const tchar *PronTable::find( const tchar *key, tchar **next )
{
	int i = search( key );
	if ( i == -1 ) return NULL;
	if ( next ) *next = (tchar*)key + _tcslen((*this)[i].key);
	return (*this)[i].code;
}

PronTable *OpenPronTable( const tchar *path )
{
	TIFile tif;
	if ( tif.open( path ) ){
		return NULL;
	}
	PronTable *pt = new PronTable;
	tnstr line;
	while ( tif.getline( line ) >= 0 ){
		if ( !line[0] ) continue;
		const tchar *sp = &line[0];
		const tchar *ssp = sp;
		while ( *ssp ){
			if ( *ssp == '\t' ) break;
			ssp = CharNext(ssp);
		}
		if ( !*ssp ) continue;
		*(tchar*)ssp = '\0';
		if ( !*sp ) continue;
		const tchar *key = sp;
		sp = ssp+1;
		while ( *sp == '\t' ){ sp++; }
		if ( !*sp ) continue;
		tchar buf[ 20 ];
		tchar *dp = buf;
		while ( *sp ){
			if ( *sp == '\\' ){
				sp++;
				if ( *sp != '\\' ){
					tuchar c;
					if ( *sp == 'x' ){
						c = (tuchar)atox( sp+1, &(tchar*)sp );
					} else {
						c = (tuchar)_ttoi( sp );
						while ( (tuchar)*sp >= '0' && (uchar)*sp <= '9' ) sp++;
					}
					*dp++ = c;
					continue;
				}
			}
			tchar *np = CharNext( sp );
			*dp++ = *sp++;
			if ( np != sp ) *dp++ = *sp++;
		}
		*dp = '\0';
		pt->add( key, buf );
	}
	return pt;
}

