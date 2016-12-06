#include	"tnlib.h"
#pragma	hdrstop
#include	"multidic.h"
#include	"dicname.h"
#include	"dic3.h"

void SetMultiDicName( DicNames &dicname, MultiPdic &dic )
{
	dicname.clear();
	for ( int i=0;i<dic.GetDicNum();i++ ){
		dicname.add( new DICNAME( dic[i].GetFileName(), dic[i].IsReadOnly()
#ifdef USE_COMP
		 , dic[i].GetCompFlag()
#endif
		 ) );
	}
}

