#include	"tnlib.h"
#pragma	hdrstop
#include	"file.h"

int BIOFile::open( const tchar *fname )
{
	if ( BIFile::open( fname ) )
		return -1;
	if ( BOFile::open( fname ) ){
		BIFile::close( );
		return -1;
	}
	return 0;
}
