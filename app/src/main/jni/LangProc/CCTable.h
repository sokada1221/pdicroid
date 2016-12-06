//---------------------------------------------------------------------------

#ifndef CCTableH
#define CCTableH
//---------------------------------------------------------------------------

#include "defs.h"

typedef unsigned short t_code;

class TCCTable {
protected:
	tnstr FileName;
	bool ReadOnly;
	bool IncFill;
	int LineCount;
public:
	TCCTable( const tchar *filename, bool readonly=true );
	virtual ~TCCTable();
	void SetIncFill(bool enabled)
		{ IncFill = enabled; }
	virtual void ClearTable( t_code def );
	bool Read( bool header=false );
	bool Write();
	void ReverseTable();
	bool IsMultiByte( unsigned char c )
		{ return FlagTable[c] ? true : false; }
	bool IsValidString(const tchar *str);
	bool ConvertWA(const tchar *str, __tnstrbufA &out);

	// Read Only member
	tnstr Name;	// table name
	t_code UndefAnsi;
	t_code UndefUnicode;
	t_code *Table;
	char FlagTable[256];
	int Num;

protected:
	void ParseHeader(const char *p);
};

#endif

