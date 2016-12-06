#ifndef	__PDPROF_H
#define	__PDPROF_H

#ifdef UNIX
#include "pdprof_unix.h"
#endif

#ifdef _Windows

#include "profile.h"	// tnlib

#define	REG_SOFTWARE	"Software"
#define	REG_COMPANYNAME	"ReliefOffice"
#define	REG_APPNAME		"PDIC2"

#define	USE_REG

#if defined(_UNICODE) && (defined(DIC_UTF8)||defined(DIC_UTF16))
extern const tchar PFS_GROUPSEL_UNI[];
#  define	_PFS_GROUPSEL	
#else
#  define	_PFS_GROUPSEL	PFS_GROUPSEL
#endif


void SaveAllProfile( );
void LoadAllProfile( );

void SaveImeProfile();
void LoadImeProfile();

void WriteHistory( const tchar *section, class HistoryArray &ha, bool path=true );
void ReadHistory( const tchar *section, class HistoryArray &ha, bool path=true );

#if defined(USE_REG)

#include "ini2reg.h"

class PdIni2Reg : public Ini2Reg {
public:
	virtual bool Record( TRegKey *curkey, const tchar *value, const tchar *data );
};

#define	JE_ZENHAN	0x001	// 全角半角変換
#define	JE_IME		0x002	// 自動IME ON/OFF
#define	JE_LASTSEL	0x004	// 起動時の選択を最後に

#include "regprof.h"

//TODO: RegProfile2はいずれtnlib2へ
// 従来のプロファイルに互換性のあるクラス
// もし、互換性を考えなければ、RegKeyクラスだけでも利用可能。
class RegProfile2 : public TRegProfile {
typedef TRegProfile super;
protected:
public:
	RegProfile2( const tchar *path );
	virtual ~RegProfile2( );

public:
//	bool IsExist( );	// コンストラクタで渡したキーが存在するかどうか調べる

	// 以下のmethodはいずれ止める？ //
	// RegKeyによるアクセスにする
	TRegKey *SetSection( const tchar *sectionname )
		{ return super::SetSection(sectionname); }
	TRegKey *SetSection(int id)
		{ return super::SetSection(GetString(id)); }

	// ReadInteger //
	int ReadInteger( const tchar *section, const tchar *id, int defval )
		{ return super::ReadInteger(section, id, defval); }
	int ReadInteger( const tchar *name, int defval )
		{ return super::ReadInteger(name, defval); }
	int ReadInteger( int section, int id, int defval )
		{ return ReadInteger( GetString( section ), GetString( id ), defval ); }
	int ReadInteger( const tchar *section, int id, int defval )
		{ return ReadInteger( section, GetString( id ), defval ); }
	int ReadInteger( int id, int defval )
		{ return Section->ReadInteger( GetString( id ), defval ); }

	// ReadString //
	tnstr ReadString( const tchar *section, const tchar *key, const tchar *defval)
		{ return super::ReadString(section, key, defval); }
	tnstr ReadString(const tchar *section, int key, const tchar *defval)
		{ return ReadString(section, GetString(key), defval); }
	tnstr ReadString( int key, const tchar *defval=NULL)
		{ return Section->ReadString(GetString(key), defval); }
	tnstr ReadString(const tchar *key, const tchar *defval=NULL)
		{ return Section->ReadString(key, defval); }

	void Read( const tchar *name, tnstr_vec &array, int delim )	// REG_SZ形式
		{ Section->Read(name, array, delim); }
	bool Write( const tchar *name, tnstr_vec &array, int delim )	// REG_SZ形式
		{ return Section->Write(name, array, delim); }

	// WriteInteger //
	void WriteInteger( const tchar *key, int value )
		{ super::WriteInteger(key, value); }
	void WriteInteger( const tchar *name, const tchar *id, int value )
		{ super::WriteInteger(name, id, value); }
	void WriteInteger( int section, int id, int value )
		{ WriteInteger( GetString( section ), GetString( id ), value ); }
	void WriteInteger( const wchar_t *section, int id, int value )
		{ WriteInteger( section, GetString( id ), value ); }
	void WriteInteger( int id, int value )
		{ Section->WriteInteger( GetString( id ), value ); }

	// WriteString //
	void WriteString(const tchar *section, const tchar *key, const tchar *str)
		{ super::WriteString(section, key, str); }
	void WriteString( int id, const tchar *str )
		{ Section->WriteString( GetString( id ), str ); }
	void WriteString(const tchar *key, const tchar *str)
		{ Section->WriteString(key, str); }

	// 準削除予定 //
	void EraseAllEntry( )					// サブキーは削除されないので注意！！
		{ Section->DeleteValue( NULL ); }
	void EraseKey( const tchar *keyname )	// サブキーkeynameを削除(NULLにすると全てのサブキーを削除(entryは削除されない))
		{ Section->DeleteKey( keyname ); }
	void EraseEntry( LPCTSTR keyname )
		{ Section->DeleteValue( keyname ); }
	void EraseEntry( int id ) { EraseEntry( GetString( id ) ); }
};

class PdProfile : public RegProfile2 {
typedef RegProfile2 super;
protected:
	// 頻繁にアクセスするregistryはここへ //
	TRegKey *rkCommon;
	bool Loaded;
	bool NetAutoDownload;
	bool OldMemory;
	bool WebWindow;
	bool OldWheelScroll;
	bool OnDemandAccess;
	bool AutoDicCheck;
	void Setup();
	int IniModeLevel;
public:
	enum {
		USBProtected = 2,
	};
public:
	PdProfile( );
	~PdProfile( );

	void SetIniMode(int ini_level);
	int GetIniModeLevel() const
		{ return IniModeLevel; }
	bool IsProtectedMode() const
		{ return IniModeLevel==USBProtected; }

	TRegKey *GetCommonKey();
		
	// Path //
	tnstr GetDictionaryPath(bool create=true);
	void SetDictionaryPath(const tchar *path);
	tnstr GetEstDBPath();
	tnstr GetPersonalPath(bool create=true);
	tnstr GetWorkingPath();
	void SetWorkingPath(const tchar *path);
	tnstr GetLangProcPath(bool create=true);
	tnstr GetSWDPath(bool create=true);

	tnstr GetDefIrregDicName(bool save=false);
	tnstr GetAdditionalIrregDicName(bool save=false);
	tnstr GetLabelFileName(bool save);
	tnstr GetTemplatePath(const tchar *filename, bool save);
	tnstr GetPersonalFile(const tchar *filename, bool save=true);
	tnstr GetCCTablePath();
	tnstr GetCTTPath();
	tnstr GetPronTableFileName();

	// Environment //
	UINT GetJEFlags(int sqm);
	void SetJEFlags(int sqm, int type, bool on);
	tnstr GetDefFontName();
	tnstr GetDefPronFontName();
	bool IsFastSearchPronEnabled();

	// Global Configurations //
	bool IsDebug();
	bool IsVUPNotify();
	bool IsTextFileBOM();
	//bool SaveAsRTF();
	int GetTextFileCode();
	bool IsOleFull();
	bool IsNetAutoDownload();
	bool IsAltEnter();
	bool IsTaskTray();
	bool IsOldMemory();
	void SetOldMemory(bool on);
	bool IsWebWindow();
	bool IsOldWheelScroll();
	void SetOldWheelScroll(bool on);
	bool IsOnDemandAccess();
	void SetOnDemandAccess(bool on);
	bool IsAutoDicCheck();
	void SetAutoDicCheck(bool on);

	// Additional read/write //
	void WritePath(const tchar *key, const tchar *str, TRegKey*reg=NULL);
	void WritePath(int id, const tchar *str, TRegKey*reg=NULL);
	tnstr ReadPath(const tchar *key, const tchar *defval=_t(""), TRegKey*reg=NULL);
	tnstr ReadPath(int id, const tchar *defval=_t(""), TRegKey*reg=NULL);

	bool WritePath(const tchar *key, tnstr_vec &array, int delim, TRegKey *reg=NULL);
	void ReadPath(const tchar *key, tnstr_vec &array, int delim, TRegKey *reg=NULL);
protected:
	__override void RootKeyOpened();
};
#else
class PdProfile : public PrivateProfile {
public:
	PdProfile( );
	~PdProfile( );
	void _SetSection( const tchar *name )
		{ SetSection( name ); }
};

extern const tchar StrIniFile[];
#endif

extern TRegKey *CurGroupReg;

extern tnstr LastExePath;

extern const tchar CompanyName[];
extern const tchar AppRegName[];
extern const tchar AppRegPath[];
extern const tchar AppRegLoc[];
extern const tchar AppLanguage[];

#define	UQ_FONT			0x0001
//#define	UQ_EFONT	0x0002
#define	UQ_VIEW			0x0004
#define	UQ_IME			0x0008
#define	UQ_ZEN2HAN		0x0010
#define	UQ_IMEZEN2HAN	0x0018
#define	UQ_AUTOSRCH		0x0020
#define	UQ_PFONT		0x0040
#define	UQ_WTEST		0x0080		// 単語テスト
#define	UQ_AUTOLINK		0x0100
#define	UQ_POPUP		0x0200		// 検索条件

extern int uniq;

#include "pfs.h"

// Helper functions //
UINT LoadInitWindow( TRegKey &reg, HWND hwnd, const tchar *pfsWndSize, const tchar *pfsWndShow, int nCmdShow, RECT *_rc=NULL );
void SaveInitWindow( TRegKey &reg, HWND hwnd, const tchar *pfsWndSize, const tchar *pfsWndShow );

void CopyRegistry(TRegKeyR *src, TRegKey *dst);
void CopyRegistry(TRegKey *src, TRegKey *dst);

#include "diclist.h"
void Read(TRegKey *reg, const tchar *section, diclist_t &dl);
void Write(TRegKey *reg, const tchar *section, const diclist_t &dl);

void ReadArray(TRegKey *reg, const tchar *section, int *array, int num);
void WriteArray(TRegKey *reg, const tchar *section, const int *array, int num);

#endif	// _Windows

extern PdProfile prof;

#endif	// __PDPROF_H


