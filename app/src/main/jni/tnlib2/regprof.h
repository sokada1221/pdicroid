#ifndef __regprof_h
#define	__regprof_h

#include "profile.h"

class TRegProfile {
protected:
	tnstr IniFileName;
	tnstr Path;
	tnstr SectionName;
	TRegKey *RootKey;
	TRegKey *Section;
	bool SectionCreated;
public:
	TRegProfile( const tchar *path );
	virtual ~TRegProfile( );

	void SetIniFile(const tchar *filename)
		{ IniFileName = filename; }
	const tchar *GetIniFile()
		{ return IniFileName; }
	bool IsIniMode() const
		{ return IniFileName.exist(); }
//	bool GetIniMode() const
//		{ return IniMode; }
	
	// レジストリ操作
	TRegKey *CreateKey(const tchar *sectionname);
	TRegKey *OpenKey(const tchar *sectionname, bool readonly=false);
	TRegKey *OpenKeyRead(const tchar *sectionname)
		{ return OpenKey(sectionname, true); }
protected:
	void CloseAll( );	// すべてクローズする
	virtual void RootKeyOpened(){}
public:
//	bool IsExist( );	// コンストラクタで渡したキーが存在するかどうか調べる

	TRegKey *GetRootKey()
		{ return RootKey; }

	bool Open(const tchar *section);		// すべてオープンする
	void CloseSection( );		// セクションのみクローズする
	void Close()
		{ CloseSection(); }
	const tchar *GetSectionName( )
		{ return SectionName; }
	TRegKey *SetSection( const tchar *sectionname );
	void SelectSection(TRegKey *section);
	TRegKey *GetSectionKey()
		{ return Section; }

	// ReadInteger //
	int ReadInteger( const tchar *name, int defval )
		{ return Section->ReadInteger( name, defval ); }
	int ReadInteger( const tchar *section, const tchar *id, int defval );

	// ReadString //
	tnstr ReadString( const tchar *section, const tchar *key, const tchar *defval);

	// WriteInteger //
	void WriteInteger( const tchar *key, int value )
		{ Section->WriteInteger( key, value ); }
	void WriteInteger( const tchar *name, const tchar *id, int value );

	// WriteString //
	void WriteString(const tchar *section, const tchar *key, const tchar *str);
};

#endif	/* __regprof_h */
