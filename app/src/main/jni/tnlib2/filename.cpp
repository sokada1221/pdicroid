#include	"tnlib.h"
#pragma	hdrstop
#include	"char.h"
#include	"filename.h"

Filename::Filename( )
{
}

Filename::Filename( const tchar *str )
{
	TNChar::set( str );
}

// ベースファイル名を変更
// strにパスがあってはいけない
void Filename::changeFile( const tchar *str )
{
	tchar *p = getbasename( buf );
	*p = '\0';
	TNChar::cat( str );
}

// パスの変更
// strにはパスのみ（ファイル名があってはいけない）
void Filename::changePath( const tchar *str )
{
	TNChar base( getbasename( buf ) );
	TNChar::set( str );
	if ( base[0] != '\0' ){
		addyen( buf );
		TNChar::cat( base );
	}
}

// 拡張子の変更
// strは必ず３文字以内で、'.' を含まないこと
void Filename::changeExt( const tchar *str )
{
	tchar *p = getextname( buf );
	if ( *p == '\0' ){
		*p++ = '.';
	}
	*p = '\0';
	TNChar::cat( str );
}

// パス名のみを得る
void Filename::getPath( tchar *str )
{
	tchar *p = getbasename( buf );
	int l = FP_DIFF( p, buf );
	_tcsncpy( str, buf, l );
	str[l] = '\0';
}

void Filename::getFile( tchar *str )
{
	_tcscpy( str, getbasename( buf ) );
}

void Filename::getExt( tchar *str )
{
	_tcscpy( str, getextname( buf ) );
}

#ifndef __ANDROID__	//TODO: 未対応
// strからベースネームの先頭を得る
tchar *getbasename( const tchar *str )
{
	const tuchar *p = (const tuchar*)_tmbsrchr( (tuchar*)str, CHR_DIRSEP );
	if ( p != 0 ){
		p++;
	} else {
		if ( str[0] != '\0' && str[1] == ':' ){
			p = (tuchar*)&str[2];
		} else {
			p = (tuchar*)str;
		}
	}
	return (tchar*)p;
}

#if !defined(WINCE) && !defined(UNIX)
void Filename::normalize( )
{
	TNChar path( buf );

	int drive;
	const tchar *p;
	if ( path[0] != '\0' && path[1] == ':' ){
		drive = ::_totupper( path[0] ) - 'A';
		p = &path[2];
	} else {
		drive = getdisk();
		p = path;
	}
	buf[0] = (tchar)(drive + 'A');
	buf[1] = ':';
	if ( *p != CHR_DIRSEP ){
		tchar _buf[ MAXDIR ];
		_buf[0] = '\0';
		if ( _tgetcurdir( drive + 1, _buf ) ){
			_tcscpy( buf, path );
			return;
		}
		buf[2] = CHR_DIRSEP;
		_tcscpy( &buf[3], _buf );
		addyen( buf );
		_tcscat( buf, p );
	} else {
		_tcscpy( &buf[2], p );
	}
}
#endif

// strから拡張子のポインタを返す
// ない場合は文字列の最後の'\0'をポインタを返す
tchar *getextname( const tchar *str )
{
	tchar *p = getbasename( str );
	tchar *q = (tchar*)_tmbschr( (tuchar*)p, '.' );
	if ( q ){
		return q+1;
	} else {
		return p + _tcslen( p );
	}
}

//pathの最後に'\'を追加
// relative path name can be used.
void addyen(tchar *path)
{
	if (path[0] == '\0')
		return;
	tchar *p = (tchar*)_tmbsrchr( (tuchar*)path, CHR_DIRSEP);
	if (p != 0){
		if (*(p+1) != '\0'){
			_tcscat(path, _T(STR_DIRSEP));
		}
	} else {
		if (*(path+_tcslen(path)-1) != ':')
			_tcscat(path, _T(STR_DIRSEP));
	}
}
#endif	// !__ANDROID__

