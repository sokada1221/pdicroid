//---------------------------------------------------------------------------

#ifndef LPTableH
#define LPTableH
//---------------------------------------------------------------------------

#include "defs.h"
#include "CCTable.h"

// Char code conversion table for Language Processor.

typedef unsigned short t_code;

class TLPTable : public TCCTable {
typedef TCCTable super;
public:
	TLPTable( const tchar *filename, bool readonly=true );

	// Read Only member
	int PopupOptions;
	TString PopupAddWords;
protected:
	void ParseHeader(const char *p);
	bool ReadHeader();
};

#endif

