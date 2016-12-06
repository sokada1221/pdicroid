#ifndef	__FILENAME_H
#define	__FILENAME_H

//#ifndef __CHAR_H
//#include "char.h"
//#endif

#ifndef __TNDEFS_H
#include "tndefs.h"
#endif

class Filename : TNChar {
	public:
		Filename( const tchar *str );
		Filename();
//		void set( const tchar *str );
		void changeFile( const tchar *str );
		void changePath( const tchar *str );
		void changeExt( const tchar *str );
		void getPath( tchar *buf );
		void getFile( tchar *buf );
		void getExt( tchar *buf );
		void normalize( );					// 相対パスから絶対パスへ変換
};

void addyen(tchar *path);
tchar *getbasename( const tchar *str );
tchar *getextname( const tchar *str );
BOOL makefilename( TNChar &fullpath, const tchar *path, const tchar *filename );

#define	PSF_WILDCARD	0x0001	// ワイルドカード(*?)がある
#define	PSF_DIRECTORY	0x0002	// \マークが含まれている
#define	PSF_DRIVE		0x0004	// ドライブマーク : がある
#define	PSF_NETWORK		0x0008	// 先頭に\\マークがある
#define	PSF_EXTENSION	0x0010	// 拡張子.マークがある(拡張子があるかどうかはわからない）
#define	PSF_ILLEGAL		0x1000	// 不正な文字がある
int parsefilename( const tchar *filename );	// ファイル名の解析
											// 戻り値はPSF_...

#ifndef __ANDROID__	//TODO: 未対応
// ファイル検索用クラス
class FindFile {
	protected:
		TNChar path;
		BOOL fFirst;
#ifdef WINCE
		HANDLE hFind;
#endif
	public:
		int flag;
#if defined(__TURBOC__) && !defined(WINCE)
		_tffblk ffb;
#endif
	public:
		FindFile( const tchar *path, int attr );
		~FindFile();
		BOOL Find( TNChar &filename );
};
#endif

// 未完成
#if 0
class FindFile32 {
};
#endif

#endif	// __FILENAME_H
