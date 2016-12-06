#include	"tnlib.h"
#pragma	hdrstop
#include	"file.h"

#if 0
int TIOFile::open( const char *fname, int trunc )
{
	if ( TIFile::open( fname ) )
		return -1;
	if ( TOFile::open( fname, trunc ) ){
		TIFile::close( );
		return -1;
	}
	return 0;
}

void TIOFile::close( void )
{
	TOFile::close();
	TIFile::close();
}

void TIOFile::seek( long l )
{
	TIFile::flush();
	TOFile::seek( l );
}

#endif
