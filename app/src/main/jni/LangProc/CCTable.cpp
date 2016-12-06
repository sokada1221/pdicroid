#include "tnlib.h"
#pragma hdrstop
#include "CCTable.h"
#include "strlib.h"

#include <string>
using namespace std;

//---------------------------------------------------------------------------
#pragma package(smart_init)

#define	MAX_HEADER_LINES	100	// header=true時の最大ライン数

TCCTable::TCCTable( const tchar *filename, bool readonly )
	:FileName( filename )
	,ReadOnly( readonly )
{
	Table = new t_code[0x10000];
	UndefAnsi = 0;
	UndefUnicode = 0;
	Num = 0;
	IncFill = true;
	LineCount = 0;
}

TCCTable::~TCCTable()
{
	if (Table)
		delete[] Table;
}

void TCCTable::ClearTable( t_code def )
{
	if (Table){
		if (IncFill){
			for ( int i=0;i<0x10000;i++ ){
				Table[i] = (t_code)i;
			}
		} else {
			for ( int i=0;i<0x10000;i++ ){
				Table[i] = def;
			}
		}
	}
	memset( FlagTable, 0, sizeof(FlagTable) );
	UndefAnsi = 0;
	UndefUnicode = 0;
	Num = 0;
	LineCount = 0;
}

// header : headerのみreadする場合はtrue
bool TCCTable::Read( bool header )
{
	TIFile tif;
	if (tif.open( FileName ))
		return false;

	ClearTable( 0 );

	string line;
	LineCount = 0;
	while (tif.getline(line)>=0){
		LineCount++;
		if (header && LineCount>=MAX_HEADER_LINES){
			break;
		}
		long code1 = -1;
		long code2 = -1;
		const char *p = skipspc(line.c_str());
		for(;*p;){
			unsigned char c = *p++;
			if (c == '#' || c == ';'){
				ParseHeader(p);
				break;	// end of line
			} else
			if ((c>='0'&&c<='9')
				|| (c>='A'&&c<='F')
				|| (c>='a'&&c<='f')
				){
				if (code1==-1){
					code1 = atov( p-1, &p, 0 );
				} else {
					code2 = atov( p-1, &p, 0 );
					break;	// end of line
				}
			}
		}
		if (code1!=-1 && code2!=-1){
			if (Num==0){
				if (header)
					break;	// end of file
				ClearTable( UndefUnicode );
			}
#if 0
			if (Table[code1]!=UndefUnicode){
				// 重複コード！！！
			}
#endif
			Table[code1] = code2;
			if (code1>=0x100){
				// multibyte
				FlagTable[code1>>8] = 1;
			}
			Num++;
		}
	}
	return true;
}

bool TCCTable::Write()
{
	return false;
}

// Table[]の検索方向を逆にする
// Ansi→UnicodeをUnicode→Ansiにする
void TCCTable::ReverseTable()
{
	if (!Table) return;
	t_code *table = new t_code[0x10000];
	int i;
	for ( i=0;i<0x10000;i++ ){
		table[i] = UndefAnsi;
	}
	for ( i=0;i<0x10000;i++ ){
		if (Table[i]!=UndefUnicode){
			table[Table[i]] = i;
		}
	}
	delete[] Table;
	Table = table;
}

// strの文字はすべてTable[]に定義されているか？
bool TCCTable::IsValidString(const tchar *str)
{
	//Note: __assert(UndefAnsi==0 && UndefUnicode==0)
	if (!Table)	return false;
	for (;*str;){
		if (!Table[*str])
			return false;
		str++;
	}
	return true;
}

bool TCCTable::ConvertWA(const tchar *str, __tnstrbufA &out)
{
	if (!Table)	return false;

	bool ret = true;
	for (;;){
		tchar c = *str++;
		if (!c)
			break;
		c = Table[c];
		if (c){
			if (c>=0x100){
				// multibyte
				out += c>>8;
			}
			out += c&0xFF;
		} else {
			ret = false;
		}
	}
	return ret;
}

void TCCTable::ParseHeader(const char *p)
{
	const char *q;
	if ((q=strstr(p,"Name:"))!=NULL){
		p = skipspc(q+5);
		if (*p){
			Name.setAsc(p);
		}
	} else
	if ((q=strstr(p,"UndefAnsi:"))!=NULL){
		p = skipspc(q+6);
		if (*p){
			UndefAnsi = atov( p, NULL, 0 );
		}
	} else
	if ((q=strstr(p,"UndefUnicode:"))!=NULL){
		p = skipspc(q+6);
		if (*p){
			UndefUnicode = atov( p, NULL, 0 );
		}
	}
}

