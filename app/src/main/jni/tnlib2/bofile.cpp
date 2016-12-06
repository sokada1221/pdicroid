#include	"tnlib.h"
#pragma	hdrstop
#include	"file.h"

int BOFile::open(const tchar *_filename )
{
	err = 0;
	text = 0;
	filename = _filename;
	if ( fd == -1 ){
		fd = ::_topen(filename, O_WRONLY|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
		if (fd == -1){
			err = 1;
			return -1;
		}
	}
	return 0;
}

int BOFile::create(const tchar *_filename )
{
	err = 0;
	text = 0;
	filename = _filename;
	if ( fd == -1 ){
		fd = ::_topen(filename, O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);
		if (fd == -1){
			err = 1;
			return -1;
		}
#if defined(_UNICODE) && defined(__BORLANDC__) && (__BORLANDC__>=0x0550 && __BORLANDC__<=0x0560)
		// C++Builderのバグ？_wopenでは必ずreadonlyのファイルを作成してしまう
		_tchmod(filename,S_IREAD|S_IWRITE);
#endif
	}
	return 0;
}

int BOFile::write( const void *buf, int len )
{
	if ( ::write( fd, buf, len ) < len ){
		return -1;
	}
	return 0;
}

