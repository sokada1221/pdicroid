#include <windows.h>
#pragma hdrstop
#include "tndefs.h"
#include "regprof.h"

#ifdef _UNICODE
#include "fexist.h"
#include "file.h"
#endif

//tnstr TRegProfile::IniFileName;	// Uses ini file if not null.

TRegProfile::TRegProfile(const tchar *_path)
	:Path(_path)
{
	RootKey = NULL;
	Section = NULL;
	SectionCreated = false;
}

TRegProfile::~TRegProfile( )
{
	CloseAll( );
}

bool TRegProfile::Open(const tchar *newsecname)
{
	if (!RootKey){
		if (IniFileName.exist()){
#ifdef _UNICODE
			// .iniƒtƒ@ƒCƒ‹‚ðUTF-16‚É‚·‚é‚½‚ß
			if (fexist(IniFileName)){
				TOFile file;
				if (!file.open(IniFileName)){
					if (file.tell()==0){
						file.bom();
					}
				}
			} else {
				TOFile file;
				if (!file.create(IniFileName)){
					file.bom();
				}
			}
#endif
			RootKey = new TRegKeyIni(IniFileName, _T(""));
			if (!RootKey){
				return false;
			}
		} else {
			TRegKeyR *key = new TRegKeyR(HKEY_CURRENT_USER);
			if (!key){
				return false;
			}
			RootKey = key->CreateKey(Path);
			delete key;
			if (!RootKey){
				return false;
			}
		}
		RootKeyOpened();
	}
	if (!newsecname || !newsecname[0]){
		CloseSection();
		return true;
	}
	if (_tcscmp(newsecname, SectionName)==0){
		// Same section.
		return true;
	}
	SectionName = newsecname;
	if ( Section ){
		if (SectionCreated)
			delete Section;
		Section = NULL;
	}
	Section = RootKey->CreateKey(SectionName);
	if (!Section){
		return false;
	}
	SectionCreated = true;
	return true;
}

void TRegProfile::CloseAll( )
{
	CloseSection();
	if (RootKey){
		delete RootKey;
		RootKey = NULL;
	}
}

void TRegProfile::CloseSection( )
{
	if (Section){
		if (SectionCreated)
			delete Section;
		Section = NULL;
		SectionName.clear();
	}
}
TRegKey *TRegProfile::SetSection( const tchar *_appname )
{
	Open(_appname);
	return Section;
}
TRegKey *TRegProfile::CreateKey(const tchar *name)
{
	return RootKey->CreateKey(name);
}
TRegKey *TRegProfile::OpenKey(const tchar *name, bool readonly)
{
	return RootKey->OpenKey(name, readonly);
}

//Note: _section is not deleted in this class.
void TRegProfile::SelectSection(TRegKey *_section)
{
	if (Section==_section)
		return;
	CloseSection();
	SectionCreated = false;
	Section = _section;
}
int TRegProfile::ReadInteger( const tchar *section, const tchar *id, int defval )
{
	if (section)
		SetSection( section );
	return Section->ReadInteger( id, defval );
}
tnstr TRegProfile::ReadString( const tchar *section, const tchar *id, const tchar *defval)
{
	if (section)
		SetSection( section );
	return Section->ReadString( id, defval );
}
void TRegProfile::WriteInteger( const tchar *section, const tchar *id, int value )
{
	if (section)
		SetSection( section );
	Section->WriteInteger( id, value );
}
void TRegProfile::WriteString(const tchar *section, const tchar *key, const tchar *str)
{
	if (section)
		SetSection(section);
	Section->WriteString(key, str);
}