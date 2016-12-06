#include "tnlib.h"
#pragma	hdrstop
#include "dicdbg.h"

#ifdef USE_DD
static DDEClient *dde = NULL;

void DDOpen( )
{
	if ( dde ) return;

	dde = new DDEClient( /* APPCMD_FILTERINITS | CBF_FAIL_ADVISES */0, 1000 );
	if ( !dde->Init( hTNInstance ) ){
		dde->Finish();
		delete dde;
		dde = NULL;
	}
	if ( !dde->Connect( "PDICDebug", "PDDebug" ) ){
		dde->Finish();
		delete dde;
		dde = NULL;
	}
}

void DDClose()
{
	if ( dde ){
		dde->Disconnect();
		dde->Finish();
		delete dde;
		dde = NULL;
	}
}

void DD( int cmd, int p1, int p2, const char *str )
{
	if ( !dde ) return;
	dde->SetItem( "Command" );
	char buf[ 1024 ];
	char *p = buf;
	switch ( cmd ){
		case DD_OPEN:
			*p++ = 1 + ' ';
			break;
		case DD_RECORD:
			*p++ = 2 + ' ';
			break;
		case DD_RECORDEND:
			*p++ = 3 + '  ';
			break;
		case DD_UPDATE:
			*p++ = 4 + ' ';
			break;
		case DD_UPDATEEND:
			*p++ = 5 + ' ';
			break;
		case DD_ERASE:
			*p++ = 6 + ' ';
			break;
		case DD_ERASEEND:
			*p++ = 7 + ' ';
			break;
		case DD_REGIST:
			*p++ = 10 + ' ';
			break;
		case DD_SETNBLOCK:
			*p++ = 11 + ' ';
			break;
		case DD_SETINDEXBLOCK:
			*p++ = 12 + ' ';
			break;
		case DD_FREE:
			*p++ = 13 + ' ';
			break;
		default:
			return;
	}
	wsprintf( p, "%d ", p1 );
	p += strlen(p);
	wsprintf( p, "%d ", p2 );
	p += strlen(p);
	if ( str ){
		strcpy( p, str );
		p += strlen(p);
	}
	*p = '\0';
	dde->Poke( buf );
}
#endif

