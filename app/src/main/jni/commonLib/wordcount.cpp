#include "tnlib.h"
#pragma	hdrstop
#include "jtype.h"
#include "wordcount.h"

// - ハイフンは区切りとして扱う
// 語中の#は構成文字として扱う : SortHitWords()でABC#1を１語と見なす必要があるため
int WordCount( const tchar *word, const tchar *endp )
{
	int num = 0;
	tuchar c;
	while ( word<endp && ((c=*word)!=0)){
		if (isalphanum(c) || c=='\'' || c>=0xc0){
			num++;
			while ((c=*word)!=0){
				if ( isalphanum(c) || c=='\'' || c>=0xc0 || c=='#'){
					word++;
					continue;
				}
				break;
			}
			if ( !word[0] )
				break;
		}
		word++;
	}
	return num;
}

int BitCount( int bits )
{
	int n = 0;
	for ( ;bits; ){
		n++;
		bits &= (bits-1);
	}
	return n;
}

