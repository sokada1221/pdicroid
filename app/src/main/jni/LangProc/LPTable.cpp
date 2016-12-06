//---------------------------------------------------------------------------

#include "tnlib.h"
#pragma hdrstop

#include "LPTable.h"
#include "strlib.h"
#include <string>
using namespace std;

//---------------------------------------------------------------------------

#define	MAX_HEADER_LINES	100	// header=true時の最大ライン数

//TODO: サロゲートペアへの対応を考える（基本的に無視でよいかな。。）

TLPTable::TLPTable( const tchar *filename, bool readonly )
	:super(filename, readonly)
{
	PopupOptions = 0;
}
void TLPTable::ParseHeader(const char *p)
{
	const char *q;
	if ((q=strstr(p, "PopupOptions:"))!=NULL){
		p = skipspc(q+13);
		if (*p){
			PopupOptions = atov(p, NULL, 0);
		}
	} else
	if ((q=strstr(p, "PopupAddWords:"))!=NULL){
		p = skipspc(q+14);
		if (*p){
			PopupAddWords.setUTF8(p);
		}
	} else {
		super::ParseHeader(p);
	}
}
