#include "tnlib.h"
#pragma	hdrstop
#include "id.h"
#include "wpdcom.h"
#include "filestr.h"
#include "pdfilestr.h"
#include "pdstrlib.h"
#include "hyplink.h"
#include "windic.h"
#include "assexec.h"

#include <shlwapi.h>	// for AssocQueryString()
#pragma link "shlwapi.lib"

tnstr GetExecFileExt(const tchar *ext);

// Global Variable //
static FNTextLinkProc TextLinkProc = NULL;

void SetTextLinkProc(FNTextLinkProc _TextLinkProc)
{
	TextLinkProc = _TextLinkProc;
}

bool GetRegKey( HKEY Key, const tchar *SubKey, tchar *RetData, long maxlen )
{
	HKEY hk;

	int Result = RegOpenKeyEx( Key, SubKey, 0, KEY_QUERY_VALUE, &hk) == ERROR_SUCCESS;

	if ( Result ){
#ifdef WINCE
		DWORD type = REG_SZ;
		RegQueryValueEx( hk, NULL, NULL, &type, (LPBYTE)RetData, (DWORD*)&maxlen );
#else
		RegQueryValue(hk, NULL, RetData, &maxlen);
#endif
		RegCloseKey(hk);
	}
	return Result;
}

// [O] filename
static void GetTextLinkParam(const tchar *param, tnstr &filename, int &line, int &col)
{
	tchar *buf = new tchar[ _tcslen( param )+1 ];
	_tcscpy( buf, param );
	const tchar *s = _tmbstok( buf, _T("|") );
	if ( s )
		filename.set( s );
	else filename.clear();
	s = _tmbstok( NULL,_T( "|") );
	if ( s ){
		line = _ttoi( s );
		if ( line > 0 ) line--;
	} else line = 0;
	s = _tmbstok( NULL, _T("|") );
	if ( s ){
		col = _ttoi( s );
		if ( col > 0 ) col--;
	} else col = 0;
	delete[] buf;
}

// filename|line|num 文字列を変換
// filenameは見つかったfullpath
void GetTextLinkParam( const tchar *param, tnstr &filename, int &line, int &col, Pdic *dic )
{
	GetTextLinkParam(param, filename, line, col);

	tnstr fullpath;
	if (FindFileLinkPath(dic, fullpath, filename, true)){
		filename = fullpath;
	}
}

// filename|line|num 文字列を変換
// filenameは見つかったfullpath
void GetTextLinkParam( const tchar *param, tnstr &filename, int &line, int &col, class MPdic *dic )
{
	GetTextLinkParam(param, filename, line, col);

	tnstr fullpath;
	if (dic->SearchFileLinkPath(filename, FileLinkPath, fullpath)){
		filename = fullpath;
	}
	
	if (FindFileLinkPath(fullpath, filename, NULL, NULL, NULL, true)){
		filename = fullpath;
	}
}

// HLT_TEXT/HLT_FILE以外
// FileLinkPathを使用しないexecutionのみ
bool AssociateExecute( HWND hwnd, const tchar *filename, int type, bool activate )
{
	__assert(type!=HLT_TEXT && type!=HLT_FILE);
	return AssociateExecuteEx(hwnd, filename, type, activate, NULL);
}

Pdic *FindFileLinkPath(const tchar *filename)
{
	MPdic *mdic = GetActiveDic();
	if (mdic){
		tnstr fullpath;
		int i = mdic->SearchFileLinkPath(filename, FileLinkPath, fullpath);
		if (i!=-1){
			return &(*mdic)[i];
		}
	}
	return NULL;
}

// dic is required if type is HLT_TEXT or HLT_FILE.
bool AssociateExecuteEx(HWND hwnd, const tchar *filename, int type, bool activate, Pdic *dic)
{
#if 1
	tchar key[256];
	tchar *p;
	tchar *dp;
	tchar *buf;
	const tchar *name;
	tnstr fullpath;
	bool r;
	int show = activate ? SW_SHOW : SW_SHOWMINNOACTIVE;
	int shownormal = activate ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE;
	switch ( type ){
		case HLT_WORD:
		case HLT_WORD2:
			return true;
		case HLT_HTTP:
		case HLT_HTML_HREF:
			// http://www...はOK
			// http:www....は駄目
#ifdef WINCE
			if ( !_ShellExecute( NULL, _T("open"), filename, NULL, NULL, show ) )
#else
			if ( (UINT)ShellExecute( NULL, _T("open"), filename, NULL, NULL, show ) <= 32 )
#endif
			{
				// もしエラーがでれば
				key[0] = '\0';
				if ( GetRegKey( HKEY_CLASSES_ROOT, _T(".htm"), key, tsizeof(key) ) ){
					_tcscat( key, _T("\\shell\\open\\command") );
					if ( GetRegKey( HKEY_CLASSES_ROOT, key, key, tsizeof(key) ) ){
						p = _tcsstr( key, _T("\"%1\"") );
						if ( !p ){
							p = _tcsstr( key, _T("%1") );
							if ( p )
								memmove( p-1, p+2, LENTOBYTE(_tcslen(p+2)+1) );
						} else
							memmove( p-1, p+4, LENTOBYTE(_tcslen(p+4)+1) );
						_tcscat( key, _T(" ") );
						_tcscat( key, filename );
						return _WinExec( key, show ) ? true : false;
					}
				}
			}
			else return true;
			return false;
		case HLT_MAILTO:
			if ( GetRegKey(HKEY_CLASSES_ROOT, _T("mailto\\shell\\open\\command"), key, tsizeof(key) ) ){
//				if ( GetRegKey( HKEY_CLASSES_ROOT, key, key, tsizeof(key) ) )
				{
					p = _tcsstr( key, _T("\"%1\"") );
					if ( !p ){
						p = _tcsstr( key, _T("%1") );
						if ( p )
							memmove( p-1, p+2, LENTOBYTE(_tcslen(p+2)+1) );
					} else
						memmove( p-1, p+4, LENTOBYTE(_tcslen(p+4)+1) );
					_tcscat( key, _T(" ") );
					_tcscat( key, filename );
					return _WinExec( key, show ) ? true : false;
				}
			}
			return false;
		case HLT_HTML:
			return false;
		case HLT_TEXT:
			{
				if (!dic){
					dic = FindFileLinkPath(filename);
				}
				tnstr fname;
				int line, col;
				GetTextLinkParam( filename, fname, line, col, dic );
				// if ( ! )
				{
					if (TextLinkProc){
						return TextLinkProc(fname, line, col);
					} else {
						__assert__;
						// PDICではここに来ないので未定義
						return false;
					}
				}
#if 0
				else {
					// エディターの指定がある場合
					tchar buf[ 10 ];
					if ( !ReplaceString( key, "\"%1\"", filename ) )
						ReplaceString( key, "%1", filename );
					wsprintf( buf, "%d", line );
					if ( !ReplaceString( key, "\"%2\"", buf ) )
						ReplaceString( key, "%2", buf );
					wsprintf( buf, "%d", col );
					if ( !ReplaceString( key, "\"%3\"", buf ) )
						ReplaceString( key, "%3", buf );
					r = WinExec( key, show ) > 31;
					return r;
				}
				return true;
#endif
			}
#ifndef SMALL
		case HLT_TEXTFILE:
			buf = new tchar[ _tcslen(filename) + 256 ];
			r = false;
			// 拡張子 .txt の関連付け実行ファイルを取得
			{
				tchar temp[MAXPATH];
				GetTempPath(tsizeof(temp),temp);
				addyen( temp );
				tchar *dp = temp + _tcslen(temp);	// 最後
				for ( int i=0;;i++ ){
					wsprintf(dp,_T("%d.txt"),i);
					if ( fexist(temp) )
						continue;
					break;
				}
				int fd = _topen(temp,O_WRONLY|O_CREAT); close(fd);	// 空ファイル作成
				if ( (int)FindExecutable( temp, _T(""), buf ) > 32 ){
					_tcscat( buf, _T(" \"") );
					_tcscat( buf, filename );
					_tcscat( buf, _T("\"") );
					r = WinExec( _mustr(buf), shownormal ) > 31;
				}
				DeleteFile( temp );
			}
			delete[] buf;
			return r;
#endif
		case HLT_FILE:
			if (!dic){
				dic = FindFileLinkPath(filename);
			}
			if (FindFileLinkPath(dic, fullpath, filename, true)){
				filename = fullpath;
			}
			// fall through
		default:
			// 書式：
			// [|Operation|]filename[|[parameter][|[directory][|ShowCommand]]]
			// 先頭に | がある場合は、Operationを示す(open,printなど)
			{
				tnstr exename;
				tnstr parameter2;

				buf = new tchar[ _tcslen( filename )+1 ];
				_tcscpy( buf, filename );
				dp = buf;
				const tchar *operation = NULL;
				if ( *dp == '|' ){
					dp++;
					operation = dp;
					while ( *dp != '|' && *dp ) dp++;
					if ( *dp ){
						*dp = '\0';
						dp++;
					}
					if ( !*operation ) operation = NULL;
				}
				name = _tmbstok( dp, _T("|") );
				const tchar *parameter = _tmbstok( NULL, _T("|") );
				const tchar *directory = _tmbstok( NULL, _T("|") );
//				const tchar *ShowCommand = _tmbstok( NULL, _T("|") );
				bool r;
				if (!name){
					// error
					r = false;
				} else
				{
	// ShellExecute configuration //
	#define	USE_FINDEXECUTABLE
	//#define	USE_CANONICALNAME
#ifdef WINCE
				r = _ShellExecute( hwnd, operation, name, parameter, directory, show );
#else	// !WINCE
#ifdef	USE_FINDEXECUTABLE
				if (!operation){
					// as for .htm file ShellExecute cannot run correctly in TaN's environment.
					// to avoid this, uses exe filename directly.
					const tchar *ext = GetFileExtension(name);
					if (!ext || (_tcsicmp(ext,_T("exe"))&&_tcsicmp(ext,_T("bat"))&&_tcsicmp(ext,_T("com")))){
						exename = GetExecFileExt(ext);
						if (exename.exist()){
							// get executable name successfully
							parameter2 = _T("\"");
							parameter2 += name;
							parameter2 += _T("\"");
							if (parameter){
								parameter2 += _T(" ");
								parameter2 += parameter;
							}
							parameter = parameter2;
							name = exename;
						}
					}
				}
#endif
#ifdef USE_CANONICALNAME
				char *canoname = new char[strlen(name)+10];
				// とりあえずテスト用に決め打ち
				strcpy(canoname, "file:///");
				strcat(canoname, "d:/temp/");
				strcat(canoname, GetFileName(name));
				name = canoname;
#endif
				r = ((UINT)ShellExecute( hwnd, operation, name, parameter, directory, show ) > 32);
#endif	// !WINCE
				}	// no error
#ifdef USE_CANONICALNAME
				delete[] canoname;
#endif
				delete[] buf;
				return r;
			}
	}
#else
	const tchar *ext = GetFileExtension( filename );
	tnstr app;
	if ( !ext || !GetAssociationFile( ext, app, filename ) ){
		if ( !ext || !IsExecuteExtension( ext ) ){
			if ( p ){
				// ファイルの実行
				tnstr cmdstr( filename );
				LineEditDialog *ldlg = new LineEditDialog( p, _LT(CAP_EXECFILE), cmdstr, MAX_PATH );
				ldlg->Style |= LEDS_MUSTINPUT;
				if ( ldlg->Execute( ) == IDOK ){
					WinExec( cmdstr, show );
				}
			}
//			MessageBox( hwnd, _LT(MSG_CANTFINDASSOCIATION), _LT(CAP_EXECUTEOBJECT), MB_OK|MB_ICONSTOP );
			return FALSE;
		} else {
			WinExec( filename, shownormal );
			return TRUE;
		}
	} else {
		WinExec( app, shownormal );
		return TRUE;
	}
#endif
}

tnstr GetExecFileExt(const tchar *_ext)
{
	if (!_ext || !_ext[0])
		return NULL;	// no extension -> error

	tnstr ext;
	if (_ext[0]=='.')
		ext = _ext;
	else
		ext = _t(".");
		ext += _ext;
	tchar exename[MAXPATH];
	DWORD size = tsizeof(exename);
	if (AssocQueryString(0, ASSOCSTR_EXECUTABLE, ext, _t("open"), exename, &size)!=S_OK)
		return NULL;
	return tnstr(exename);
}

// 仕様通りの検索を行なう
// commonpath can be null.
// extra : CommandPathおよびcurrent dirも検索するか？
// filenameがpathのみの場合(terminated with \)、それがfilelink pathかどうかのチェックを行う
bool FindFileLinkPath( tnstr &fullpath, const tchar *filename, const tchar *dicpath, const tchar *dicflink, const tchar *commonpath, bool extra)
{
	if (!filename || !filename[0])
		return false;

	bool path_check = false;
		
	if (IsFullPath(filename)){
		tchar c = getlastchar(filename);
		if (c=='\\' || c=='/'){
			// filename is a path.
			path_check = true;
		} else {
			if (fexist(filename)){
				fullpath = filename;
				return true;
			}
			filename = GetFileName(filename);
		}
	}

	// 辞書ファイルのpathで探す
	if (dicpath){
		if (FindFileForLink( fullpath, dicpath, filename, NULL )){
			return true;
		}
	}
	// 辞書ごとのユーザー指定によるパスで探す
	if (dicflink){
		if (FindFileForLink( fullpath, dicflink, filename, dicpath )){
			return true;
		}
	}
	// commonpathによる検索
	if (commonpath){
		if (FindFileForLink( fullpath, commonpath, filename, dicpath )){
			return true;
		}
	}

	if (extra){
		if (FindFileForLink(fullpath, CommandPath, filename, NULL)){
			return true;
		}
		if (!path_check){
			// current directory.
			return fexist(filename);
		}
	}
	return false;
}

// fullpathのfilenameを dicpath/dicflink/commonpathからの相対パスに変換する
// 相対パスにできない場合は、relpathにfilenameそのものが入る
// filenameがpathのみの場合(terminated with \)、それがfilelink pathかどうかのチェックを行う
bool GetRelFileLinkPath( tnstr &relpath, const tchar *filename, const tchar *dicpath, const tchar *dicflink, const tchar *commonpath, bool inc_drive)
{
	if (!filename || !filename[0])
		return false;

	// 辞書ごとのユーザー指定によるパスで探す
	if (dicflink){
		if (GetRelPathForLink( relpath, dicflink, filename, dicpath, inc_drive )){
			return true;
		}
	}

	// 辞書ファイルのpathで探す
	if (dicpath){
		if (GetRelPathForLink( relpath, dicpath, filename, NULL, inc_drive )){
			return true;
		}
	}

	// commonpathによる検索
	if (commonpath){
		if (GetRelPathForLink( relpath, commonpath, filename, dicpath, inc_drive )){
			return true;
		}
	}

	if (inc_drive)
		return false;

	return GetRelFileLinkPath(relpath, filename, dicpath, dicflink, commonpath, true);
}

