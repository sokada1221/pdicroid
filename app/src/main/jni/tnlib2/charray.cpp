#include	"tnlib.h"
#pragma	hdrstop
#include	"charray.h"

// •¶š”‚ğ•Ô‚·
int FlexCharArray::GetAllLength( )
{
	int l = 0;
	for ( int i=0;i<get_num();i++ ){
		l += _tcslen( (*this)[i] ) + 1;
	}
	return l;
}

int FlexCharArray::GetMulti( tchar *buf, int maxlen )
{
	int i;
	for ( i=0;i<get_num();i++ ){
		const tchar *sp = (*this)[i];
		int l = _tcslen( sp ) + 1;
		if ( maxlen < l + 1 ){
			break;
		}
		memcpy( buf, sp, LENTOBYTE(l) );
		buf += l;
		maxlen -= l;
	}
	*buf = '\0';
	return i;
}

tchar *FlexCharArray::GetMulti( int *len )
{
	int maxlen = GetAllLength( ) + 1;
	tchar *buf = new tchar[ maxlen ];
	if ( buf ){
		GetMulti( buf, maxlen );
	}
	if ( len )
		*len = maxlen;
	return buf;
}

void FlexCharArray::SetMulti( const tchar *multi )
{
	while ( *multi ){
		add( new TNChar( multi ) );
		multi += _tcslen( multi ) + 1;
	}
}

