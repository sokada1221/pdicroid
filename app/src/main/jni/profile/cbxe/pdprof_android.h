#ifndef __pdprof_android_h
#define	__pdprof_android_h

class TRegKey {
protected:
	tnstr Name;
public:
	TRegKey(const tchar *name);

	const tchar *GetName() { return Name; }

	TRegKey *CreateKey(const tchar *keyname);
	TRegKey *OpenKey(const tchar *keyname, bool readonly=false){ return new TRegKey(keyname); }

	int ReadInteger( const tchar *id, int defval ){ return defval; }
	void WriteInteger( const tchar *key, int value ){}
	void WriteString(const tchar *key, const tchar *str){}
	void DeleteValue(const tchar *key){}

	void EraseKey( const tchar *keyname );	// サブキーkeynameを削除(NULLにすると全てのサブキーを削除(entryは削除されない))
	void EraseEntry( const tchar *keyname );
	bool PutVarArray( const tchar *entry, UINT *array, int number );
	bool GetVarArray( const tchar *entry, UINT *array, int number );
	tnstr ReadString(const tchar *key, const tchar *defval=_t(""));
	void Read( const tchar *name, tnstr_vec &array, int delim );	// REG_SZ形式
	bool Write( const tchar *name, tnstr_vec &array, int delim ){ return false; }	// REG_SZ形式

	bool ReadMulti( const tchar *name, tnstr_vec &array );	// REG_MULTI_SZ形式
};

class PdProfile
{
protected:
	TRegKey *RegKey;	// current regkey
public:
	// Global Configurations //
	bool IsTextFileBOM() { return true; }
	int GetTextFileCode() { return TFM_DEFAULT; }

	tnstr GetDictionaryPath(bool create);
	tnstr GetCTTPath();
	tnstr GetDefIrregDicName();
	tnstr GetAdditionalIrregDicName();

	tnstr ReadString( const tchar *section, const tchar *key, const tchar *defval);
	int ReadInteger( const tchar *section, const tchar *id, int defval );
	int ReadInteger( const tchar *id, int defval );
	void WriteInteger( const tchar *key, int value );
	void WriteString(const tchar *section, const tchar *key, const tchar *str){}

	bool WritePath(const tchar *key, tnstr_vec &array, int delim, TRegKey *reg=NULL);
	void WritePath(const tchar *key, const tchar *str, TRegKey*reg=NULL){}
	bool ReadMulti( const tchar *name, tnstr_vec &array ){ return false; }	// REG_MULTI_SZ形式
	void ReadPath(const tchar *key, tnstr_vec &array, int delim, TRegKey *reg=NULL);

	TRegKey *CreateKey(const tchar *keyname);
	void Close(){}
	TRegKey *SetSection( const tchar *_appname );
};

#include "diclist.h"
void Read(TRegKey *reg, const tchar *section, diclist_t &dl);
void Write(TRegKey *reg, const tchar *section, const diclist_t &dl);
void ReadArray(TRegKey *reg, const tchar *section, int *array, int num);

inline void CopyRegistry(TRegKey *src, TRegKey *dst){}

#include "pfs.h"

extern tnstr LastExePath;
extern tnstr CommandPath;

#endif

