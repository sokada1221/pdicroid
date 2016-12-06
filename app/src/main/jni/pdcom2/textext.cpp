#include "tnlib.h"
#pragma	hdrstop
#include "pdconfig.h"
#include "textext.h"
//#include "pdprof.h"

TextExtension textext(NULL);

const tchar *TextExtension::DefTextExts = _t("txt;c;cpp;h;java");

TextExtension::TextExtension(const tchar *text_exts)
{
	if (text_exts){
		Set(text_exts);
	} else {
		Set(DefTextExts);
	}
}

TextExtension::~TextExtension( )
{
}

#if 0
void TextExtension::LoadProfile( )
{
#ifdef GUI
	tnstr str = prof.ReadString( PFS_COMMON, PFS_TEXTEXT, DefTextExts );
	Set( str );
#else
	//** Œ»Ý‚Ì‚Æ‚±‚ëŒÅ’è
	Set( DefTextExts );
#endif
}
#ifdef GUI
void TextExtension::SaveProfile( )
{
	tnstr str;
	Get( str );
	prof.WriteString( PFS_COMMON, PFS_TEXTEXT, str );
}
#endif
#endif

void TextExtension::Set( const tchar *str )
{
	exts.clear( );
	while ( *str ){
		const tchar *p = str;
		while ( *p ){
			if ( *p == ';' )
				break;
			p = CharNext( p );
		}
		tnstr *cp = new tnstr;
		if ( p != str )
			cp->set( str, STR_DIFF( p, str ) );
		if ( IsText( *cp ) ){
			delete cp;
			continue;
		}
		exts.add( cp );
		if ( *p == ';' )
			p++;
		str = p;
	}
}

void TextExtension::Get( tnstr &str )
{
	if ( !exts.get_num() ){
		str.clear();
		return;
	}
	int l = 0;
	int i;
	for ( i=0;i<exts.get_num();i++ ){
		l += _tcslen( exts[i] ) + 1;
	}
	l++;	// ÅŒã‚ÌI’[•”
	tchar *p = new tchar[ l ];
	tchar *dp = p;
	for ( i=0;i<exts.get_num();i++ ){
		_tcscpy( dp, exts[i] );
		dp += _tcslen( dp );
		*dp++ = ';';
	}
	*(dp-1) = '\0';
	str.setBuf( p );
}

bool TextExtension::IsText( const tchar *ext )
{
	for ( int i=0;i<exts.get_num();i++ ){
		if ( !_tcsicmp( ext, exts[i] ) )
			return true;
	}
	return false;
}


