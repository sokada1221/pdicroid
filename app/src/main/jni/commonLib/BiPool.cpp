#include "tnlib.h"
#pragma hdrstop
#include "BiPool.h"

TBiPool::TBiPool( int preallocSize )
{
	fw.allocate(preallocSize);
}
int TBiPool::BSearch( const tchar *str )
{
	int left = 0;
	int right = get_num();
	if ( right == 0 ) return 0;
	do {
		int mid = ( left + right ) /2;
		int k = _tcscmp( str, fw[mid]);
		if (k == 0){
			return mid;
		}
		if (k < 0){
			right = mid;
		} else {
			left = mid + 1;
		}
	} while (left < right);
	return left;
}

#if 0
int TBiPool::BSearchExact( const tchar *str )
{
	int left = 0;
	int right = get_num();
	if ( right == 0 ) return 0;

	do {
		int mid = ( left + right ) /2;
		int k = _tcscmp( str, fw[mid]);
		if (k == 0){
			return mid;
		}
		if (k < 0){
			right = mid;
		} else {
			left = mid + 1;
		}
	} while (left < right);
	return left;
}
#endif

int TBiPool::Find( const tchar *str )
{
	int r = BSearch( str );
	if ( r >= get_num() ) return -1;
	if ( !_tcscmp( str, fw[r]) ) return r;
	return -1; 
}

TBiPoolJ::TBiPoolJ( int maxnum )
	:super( maxnum )
{
	fj.allocate(maxnum);
}
void TBiPoolJ::Add( const tchar *word, const tchar *japa )
{
	fw.add( word );
	fj.add( japa );
}
void TBiPoolJ::AddSort( const tchar *word, const tchar *japa )
{
	int r = BSearch( word );
	if ( r >= get_num() ){
		// add
		Add(word, japa);
		return;
	}
	if ( !_tcscmp( word, fw[r]) ){
		// overwrite
		fj[r] = japa;
		return;
	}
	// insert
	fw.insert(r, word);
	fj.insert(r, japa);
}

