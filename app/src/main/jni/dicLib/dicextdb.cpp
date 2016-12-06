//---------------------------------------------------------------------------

#include "tnlib.h"
#pragma hdrstop

#include "filestr.h"
#include "dic.h"
#include "LangProc.h"
#include "LangProcMan.h"
#ifdef USE_ESTDB
#include "dicest.h"	// Estraier DB
#endif
#ifdef USE_HBTREE
#include "dichbt.h"	// HBTree DB
#endif
#ifdef USE_EXTPDIC
#include "dicextdb-pdic.h"
#endif

// for _KChar
#define	__kwstr(var)	_kwstr(var, GetKCodeTrans())	// for keyword

//---------------------------------------------------------------------------

#pragma package(smart_init)

#if defined(USE_ESTDB) || defined(USE_HBTREE) || defined(USE_EXTPDIC)

//
// 
//
TPdicExtDBBase::TPdicExtDBBase()
{
	//CanCreate = true;
	LangProc = NULL;
	SetLangProc(NULL);
}
TPdicExtDBBase::~TPdicExtDBBase()
{
	if (LangProc){
		LangProc->Release();
		LangProc = NULL;
	}
}

void TPdicExtDBBase::SetLangProc(TLangProc *proc)
{
	if (LangProc)
		LangProc->Release();

	if (proc){
		LangProc = proc;
	} else {
		LangProc = LangProcMan.GetStdLangProc();
	}
}

//TODO: 無駄だ。。
// dic3.cppが_KCharで取り扱うのをやめたらこれは不要
int TPdicExtDBBase::NextAllSearch( _KChar &word, Japa *japa, AllSearchParam *all)
{
	tnstr _word;
	int r = NextAllSearch(_word, japa, all);
	if (r!=AS_FOUND)
		return r;
	word = __kwstr(_word);
	return r;
}
int TPdicExtDBBase::PrevAllSearch( _KChar &word, Japa *japa, AllSearchParam *all)
{
	tnstr _word;
	int r = PrevAllSearch(_word, japa, all);
	if (r!=AS_FOUND)
		return r;
	word = __kwstr(_word);
	return r;
}

const TKCodeTranslateSet *TPdicExtDBBase::GetKCodeTrans() const
{
	return LangProc->GetKCodeTrans();
}

//
// PdicExtDBLayer class
//
PdicExtDBLayer::PdicExtDBLayer()
{
	//ExtDBMode = DICEXT_FASTDB;
	ExtDBMode = 0;
	ExtDB = NULL;
}
PdicExtDBLayer::~PdicExtDBLayer()
{
	if (ExtDB)
		delete ExtDB;
}
// This method should be called before open to apply the mode.
void PdicExtDBLayer::SetExtDBMode(int mode, bool f)
{
	ExtDBMode = (ExtDBMode & ~mode) | (f?mode:0);
}
// dicname == NULL : uses the filename used in the dic object.
// dicname != NULL : uses as the filename used in the dic object.
tnstr PdicExtDBLayer::GetExtDBPath(const tchar *dicname)
{
#ifdef USE_ESTDB
	TEstraierDB db(this);
#endif
#ifdef USE_HBTREE
	TPdicHbt db(this);
#endif
#ifdef USE_EXTPDIC
	TExtPdic db(this);
#endif
	return db.GetDBPath(dicname);
}

int PdicExtDBLayer::Open( const tchar * fname, bool _readonly )
{
	int ret;
	if ((ret=super::Open(fname, _readonly))!=0){
		return ret;
	}

#ifdef _DEBUG
	if (!_tcsncmp(::GetFileName(fname), _t("extdb"), 5) && !_tcscmp(::GetFileExtension(fname), _t("dic")) )
		ExtDBMode |= DICEXT_FASTDB;
#endif

	if (ExtDBMode & DICEXT_FASTDB){
		return OpenExtDB(fname, _readonly);
	}
	
	return 0;
}

int PdicExtDBLayer::Create( const tchar *fname, const tchar *dicname, int size, int dictype, int lang_proc_id )
{
	int ret;
	if ((ret=super::Create(fname, dicname, size, dictype, lang_proc_id))!=0){
		return ret;
	}

#ifdef _DEBUG
	if (!_tcsncmp(::GetFileName(fname), _t("extdb"), 5) && !_tcscmp(::GetFileExtension(fname), _t("dic")) )
		ExtDBMode |= DICEXT_FASTDB;
#endif

	if (ExtDBMode & DICEXT_FASTDB){
		return CreateExtDB(fname);
	}
	return 0;
}

void PdicExtDBLayer::Close( )
{
	if (!IsOpened())
		return;

	CloseExtDB();
	super::Close();
}

int PdicExtDBLayer::record( const tchar * word, const Japa &japa, int srchf )
{
	int ret = super::record(word, japa, srchf);
	if (ret==0){
		if (ExtDB){
			ExtDB->Record(word, japa);
		}
	}
	return ret;
}
int PdicExtDBLayer::update( const tchar *word, const Japa &japa )
{
	int ret = super::update(word, japa);
	if (ret==0){
		if (ExtDB){
			ExtDB->Update(word, japa);
		}
	}
	return ret;
}
int PdicExtDBLayer::erase( const tchar *word )
{
	int ret = super::erase(word);
	if (ret==0){
		if (ExtDB){
			ExtDB->Erase(word);
		}
	}
	return ret;
}

//TODO: [A] SrchMode modeはAllSearchParamに入れるべきではないのか？
void PdicExtDBLayer::SetAllSearch( const tchar *word, SrchMode mode, GENERICREXP *jre, AllSearchParam *_all )
{
	if (!ExtDB || !ExtDB->IsMySearchMode(mode)){
		super::SetAllSearch(word, mode, jre, _all);
		return;
	}
	AllSearchParam &all = _all ? *_all : this->all;
	ExtDB->SetAllSearch(word, all.mode, jre, &all);
}
int PdicExtDBLayer::NextAllSearch( tnstr &word, Japa *japa, AllSearchParam *_all)
{
	if (!ExtDB)
		return super::NextAllSearch(word, japa, _all);

	AllSearchParam &all = _all ? *_all : this->all;
	if (!ExtDB->IsMySearchMode(all.mode)){
		return super::NextAllSearch(word, japa, _all);
	}
	return ExtDB->NextAllSearch(word, japa, &all);
}
int PdicExtDBLayer::PrevAllSearch( tnstr &word, Japa *japa, AllSearchParam *_all)
{
	if (!ExtDB)
		return super::PrevAllSearch(word, japa, _all);
#if 0	//TODO: 必要？
	AllSearchParam &all = _all ? *_all : this->all;
	if (!ExtDB->IsMySearchMode(all.mode)){
		return super::PrevAllSearch(word, japa, _all);
	}
	return ExtDB->PrevAllSearch(word, japa, &all);
#else
	return ExtDB->PrevAllSearch(word, japa, _all);
#endif
}
int PdicExtDBLayer::NextAllSearch(_KChar &word, Japa *japa, AllSearchParam *_all)
{
	if (!ExtDB)
		return super::NextAllSearch(word, japa, _all);
#if 0	//TODO: 必要？
	AllSearchParam &all = _all ? *_all : this->all;
	if (!ExtDB->IsMySearchMode(all.mode)){
		return super::NextAllSearch(word, japa, _all);
	}
	return ExtDB->NextAllSearch(word, japa, &all);
#else
	return ExtDB->NextAllSearch(word, japa, _all);
#endif
}
int PdicExtDBLayer::PrevAllSearch(_KChar &word, Japa *japa, AllSearchParam *_all)
{
	if (!ExtDB)
		return super::NextAllSearch(word, japa, _all);

	AllSearchParam &all = _all ? *_all : this->all;
	if (!ExtDB->IsMySearchMode(all.mode)){
		return super::PrevAllSearch(word, japa, _all);
	}
	return ExtDB->PrevAllSearch(word, japa, &all);
}
int PdicExtDBLayer::OpenExtDB(const tchar *fname, int _readonly, bool create_force)
{
	__assert(!ExtDB);
#ifdef USE_ESTDB
	ExtDB = new TEstraierDB(this);
#endif
#ifdef USE_HBTREE
	ExtDB = new TPdicHbt(this);
#endif
#ifdef USE_EXTPDIC
	ExtDB = new TExtPdic(this);
#endif
	if (!ExtDB){
		super::Close();
		error = DICERR_MEMORY;
		return -1;
	}
	//ExtDB->SetMode(ExtDBMode&DICEXT_FASTDB_CREATE?true:false);
	if (create_force || !ExtDB->Open(fname, _readonly)){
		bool ok = false;
		if (!_readonly && (create_force || (ExtDBMode & DICEXT_FASTDB_CREATE))){
			if (ExtDB->Create(fname)){
				ok = true;
			}
		}
		if (!ok){
			_delete(ExtDB);
#if 0	//TODO: エラー処理→警告程度でよい？
			super::Close();
			error = DICERR_OPEN;
			return -1;
#endif
		}
	}
	return 0;
}

int PdicExtDBLayer::CreateExtDB(const tchar *fname)
{
	return OpenExtDB(fname, 0, true);
}

void PdicExtDBLayer::CloseExtDB()
{
	if (ExtDB){
		ExtDB->Close();
		_delete(ExtDB);
	}
}

#endif	// defined(USE_ESTDB) || defined(USE_HBTREE) || defined(USE_EXTPDIC)

