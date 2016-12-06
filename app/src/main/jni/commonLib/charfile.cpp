#include "tnlib.h"
#pragma	hdrstop
#include "charfile.h"

//TODO: flush()‚ðoverride‚µ‚Ä‚à³‚µ‚­“®ì‚µ‚È‚¢II

TCharOFile::TCharOFile()
{
}

int TCharOFile::open( const tchar *filename )
{
	str = (tnstr*)filename;
	return 0;
}

int TCharOFile::create( const tchar *filename )
{
	str = (tnstr*)filename;
	str->clear();
	return 0;
}

int TCharOFile::flush( )
{
	if ( curp != 0 ){
		str->cat( writebuff, curp );
		curp = 0;
	}
	return 0;
}

