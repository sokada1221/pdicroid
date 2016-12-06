#include	"tnlib.h"
#pragma	hdrstop
#include	"japa.h"
#include	"dictext.h"
#include "zen2han2.h"

// PDUTY.EXE,PDICW.EXE ‚Å‚Ì‚ÝŽg—p‚·‚é
// DIC_UTF8/16‚Å‚Íˆ³k‚µ‚½Œã‚Å‚µ‚©‚í‚©‚ç‚È‚¢

bool Japa::IsOverFlow( ) const
{
	return _tcslen( japa ) > LJAPA
		|| _tcslen( exp ) > LEXP
		|| _tcslen( pron ) > LPRON;
}
bool Japa::operator == ( const Japa &j ) const
{
	return
		japa == j.japa
		&& exp == j.exp
		&& pron == j.pron;
}
bool Japa::operator != ( const Japa &j ) const
{
	return
		japa != j.japa
		|| exp != j.exp
		|| pron != j.pron;
}

int Japa::Cut( )
{
	int r = 0;
	r |= ::jcut(japa, LJAPA);
	r |= ::jcut(exp, LEXP);
	r |= ::jcut(pron,  LPRON);
	return r;
}

#if defined(NEWDIC2) && !defined(ND3ONLY)
int Japa::CutOld( )
{
	int r = 0;
	r |= japa.jcut( LJAPA_OLD );
	r |= exp.jcut( LEXP_OLD );
	pron.clear( );
#ifdef USE_JLINK
	jlinks.clear( );
#endif
	return r;
}

bool Japa::IsOld( ) const
{
	return
#ifdef USE_JLINK
		jlinks.get_num() == 0 &&
#endif
		_tcslen( japa ) <= LJAPA_OLD && _tcslen( exp ) <= LEXP_OLD && !pron[0];
}
#endif

// ‘SŠp•¶Žš‚ðl—¶‚µ‚Ä‚¢‚È‚¢
void Japa::SetAll2( const tchar *src, bool dist )
{
	clear();
	const tchar *p;
#if USE_PRON
	if ( *src == '[' ){
		src++;
		p = src;
		while ( *p ){
			if ( *p == ']' )
				break;
			p++;
		}
		pron.set( src, STR_DIFF( p, src ) );
		if ( !*p )
			return;
		src = p + 1;
	}
#endif
	if ( dist ){
		p = src;
		while ( *p ){
			if ( !_tcsncmp( p, StrExpSepa, L_StrExpSepa ) ){
				break;
			}
			p++;
		}
		japa.set( (tchar*)src, STR_DIFF( p, src ) );
		if ( !*p )
			return;
		exp.set( p + 3 );
	} else {
		japa.set( (tchar*)src );
	}
}

