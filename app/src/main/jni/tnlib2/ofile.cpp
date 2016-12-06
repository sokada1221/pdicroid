#include	"tnlib.h"
#pragma	hdrstop
#include	"file.h"


int OFile::open( const tchar *_filename, int trunc )
{
	err = 0;
	filename = _filename;
	if ( fd == -1 ){
#ifdef WINCE
		if ( trunc ){
			fd = _tcreat( filename, O_WRONLY );
		} else {
			fd = _topen( filename, O_WRONLY );
			if ( fd == -1 ){
				fd = _tcreat( filename, O_WRONLY );
			}
		}
#else
		fd = ::_topen(filename, O_RDWR|O_BINARY|O_CREAT|(trunc?O_TRUNC:0), S_IWRITE|S_IREAD);
#if defined(_UNICODE) && defined(__BORLANDC__) && (__BORLANDC__>=0x0550 && __BORLANDC__<=0x0560)
		// C++Builderのバグ？_wopenでは必ずreadonlyのファイルを作成してしまう
		_tchmod(filename,S_IREAD|S_IWRITE);
#endif
#endif
		if (fd == -1){
			err = 1;
			return -1;
		}
	}
	return 0;
}

