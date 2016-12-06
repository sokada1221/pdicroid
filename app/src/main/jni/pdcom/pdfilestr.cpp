#include "tnlib.h"
#pragma hdrstop
#include "dicdef.h"
#include "filestr.h"
#include "pdfilestr.h"
#include "pdstrlib.h"

#ifndef max
#define	max(x,y)	((x)>(y)?(x):(y))
#endif

// filenameが相対パスの場合、カレントディレクトリからのパスに変換される。(pathは使用されない)
// それが不便な場合はmakefile2()を使用する。
//path+filename -> fullpath (FLEN制限あり）
// filenameにパスがあればそれを利用し、相対パスの場合はカレントディレクトリからのfullpathに変換
// filenameにパスが無ければpathを利用
// filenameをNULLにすると相対パスpathを絶対パスに変換するのみ（カレントディレクトリが使われるので要注意！！）
bool makename( tnstr &fullpath, const tchar *path, const tchar *filename )
{
#if defined(WINCE) || defined(UNIANSI)
	// とりあえず、パスは考えない？？？
	fullpath.set( path ? path : filename );
	return true;
#else
	if ( filename ){
		const tchar *base = GetFileName( filename );
		if (base != filename){
			// including path in filename
			tchar *p = _tfullpath( NULL, filename, FLEN );
			if ( p ){
#if defined(DOS)
				jstrupr( p );
#endif
				fullpath.set( p );
				free( p );
				return true;
			}
			return false;
		}
	}
	if ( !path || !path[0] )
		return false;	// parameter error 
	tchar *p = new tchar[ max(MAX_PATH,_tcslen(path)) + 1 ];
	if (!p)
		return false;	// memory error
	_tcscpy( p, path );
	if ( filename ){
		addyen( p );
		_tcscat( p, filename );
	}
	tchar *q = _tfullpath( NULL, p, FLEN );
	delete[] p;
	if ( q ){
#if defined(DOS)
		jstrupr( q );
#endif
		fullpath.set( q );
		free( q );
		return true;
	}
	return false;
#endif
}

//path+filename -> fullpath (FLEN制限あり）
//filenameが\で始まっている場合は [pathのdrive] + filename -> fllpath
// pathとfilenameが両方相対パスの場合、カレントディレクトリから絶対パスを作成
// filenameをNULLにすると相対パスpathを絶対パスに変換するのみ（カレントディレクトリが使われるので要注意！！）
bool makename2( tnstr &fullpath, const tchar *path, const tchar *filename )
{
	if (!path)
		path = _t("");
	tchar *p = new tchar[ max(MAX_PATH,_tcslen(path)) + 1 ];
	if (!p)
		return false;	// memory error
	_tcscpy( p, path );
	if ( filename ){
		if (IsRootPath(filename)){
			tchar *rootp = SkipDriveLetter(p);
			_tcscpy(rootp, filename);
		} else {
			addyen( p );
			_tcscat( p, filename );
		}
	}
	tchar *q = _tfullpath( NULL, p, FLEN );
	delete[] p;
	if ( q ){
#if defined(DOS)
		jstrupr( q );
#endif
		fullpath.set( q );
		free( q );
		return true;
	}
	return false;
}

#if defined(_Windows)
#include "wpdcom.h"

// CommandPathをもとに、絶対パスから相対パスに変換する
// ただし、CommandPath以下のディレクトリのみ対象
void AbsToRel( tnstr &name )
{
	int l = _tcslen(CommandPath);
	if ( !_tcsnicmp( CommandPath, name, l ) ){
		const tchar *filename = (const tchar*)name + l;
		const tchar *p = GetFileName( filename );
		if ( p == filename ){
			tnstr b( filename );
			name = b;
		}
	}
}
void RelToAbs( tnstr &in, tnstr &out )
{
#if 1	// 2009.9.14
	if (!IsFullPath(in))
#else
	if ( GetFileName( in ) == (const tchar *)in )
#endif
	{
		out = CommandPath;
		out += in;
	} else { out = in; }
}
#endif

// File Linkのためのファイル検索
// filenameが絶対パスの場合はfexist(filename)で返す
// それ以外では、path + filenameで探し、見つかればpath + filenameをfullpathへ
// ただし、pathが相対パスの場合は、dicpath + path + filenameで探す。
// pathが絶対パスでなく、dicpathがNULLの場合はfalse
// filenameがpathのみの場合(terminated with \)、それがfilelink pathかどうかのチェックを行う
// return : false : error/not exist
bool FindFileForLink( tnstr &fullpath, const tchar *path, const tchar *filename, const tchar *dicpath )
{
	bool path_check = false;

	if (IsFullPath(filename)){
		tchar c = getlastchar(filename);
		if (c=='\\' || c=='/'){
			// path only in filename.
			path_check = true;
		} else {
			if (fexist(filename)){
				return true;
			}
			filename = GetFileName(filename);
		}
	}

	tnstr _path;
	if (path && IsFullPath(path)){
		_path = path;
	} else {
		if (!dicpath || !dicpath[0]){
			return false;
		}
		_path = AddYen(dicpath) + path;	// pathが相対path
	}
	if (path_check){
		return IsSameFile(AddYen(_path), filename);
	} else
	if ( makename2( fullpath, _path, filename ) ){
		if ( _tmbsicmp( fullpath, filename ) ){
			// (dicpath +) path + filename
			if ( fexist( fullpath ) ){
				return true;
			}
		}
	}
	return false;	// error/not found
}

// File Linkのためのファイルパス変換
// ただし、pathが相対パスの場合は、dicpath + path + filenameで探す。
// pathが絶対パスでなく、dicpathがNULLの場合はfalse
// filenameがpathのみの場合(terminated with \)、それがfilelink pathかどうかのチェックを行う
// return : false : error/not exist
bool GetRelPathForLink( tnstr &relpath, const tchar *path, const tchar *filename, const tchar *dicpath, bool inc_drive )
{
	tnstr _path;
	if (path && IsFullPath(path)){
		_path = path;
	} else {
		if (!dicpath || !dicpath[0]){
			return false;
		}
		_path = AddYen(dicpath) + path;	// pathが相対path
	}
	relpath = MakeRelativePath(filename, _path, inc_drive);
	if (relpath.empty() || IsSameFileName(relpath, filename)){
		return false;
	}
	return true;
}
