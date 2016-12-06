//---------------------------------------------------------------------------
// External DB for Estraier
//---------------------------------------------------------------------------
//TODO: 検索処理 SRCH_MEMORY, SRCH_JEDIT, SRCH_LEVELを追加
//TODO: estraierはPDIC faultの影響をなるべく受けないような構造にする

#include "tnlib.h"
#pragma hdrstop
#include "uustr.h"

#include "dic.h"
#include "japa.h"

#include "est/estdbdll.h"
#include "dicest.h"
#include "pdprof.h"
#include "wpdcom.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#define	WORD_ALT_SEPARATOR	0x7F
const char TAG_INFO[] = "i:";
const char TAG_WORD[] = "w:";
const char TAG_JAPA[] = "j:";
const char TAG_EXP[] = "e:";
const char TAG_PRON[] = "p:";

static int PronEnabled = -1;

static bool IsPronEnabled();

static TEstraierDllLoader Dll;
static int DllLoadCounter = 0;

TEstraierDB::TEstraierDB(PdicBase *_dic)
	:dic(_dic)
{
	__assert(dic);
	db = NULL;
	ecode = 0;
	cond = NULL;
	resnum = 0;
	result = NULL;
}
TEstraierDB::~TEstraierDB()
{
	Close();
}
	
bool TEstraierDB::Open( const tchar * fname, int _readonly )
{
	return OpenDB(fname, _readonly?true:false);
}

void TEstraierDB::Close( )
{
	CloseDB();
}
bool TEstraierDB::Record(const tchar *word, const Japa &japa)
{
	if (!db){
		ecode = EESTDB_NOT_OPENED;
		return false;
	}
	if (!RecordItem(TAG_WORD, word, word)){
		return false;
	}
	if (!RecordItem(TAG_JAPA, word, japa.japa)){
		return false;
	}
	if (!RecordItem(TAG_EXP, word, japa.exp)){
		return false;
	}
	if (IsPronEnabled()){
		if (!RecordItem(TAG_PRON, word, japa.pron)){
			return false;
		}
	}

	return true;
}
bool TEstraierDB::Erase(const tchar *word)
{
	if (!db){
		ecode = EESTDB_NOT_OPENED;
		return false;
	}

	if (!DeleteItem(TAG_WORD, word)){
		return false;
	}
	if (!DeleteItem(TAG_JAPA, word)){
		return false;
	}
	if (!DeleteItem(TAG_EXP, word)){
		return false;
	}
	if (IsPronEnabled()){
		if (!DeleteItem(TAG_PRON, word)){
			return false;
		}
	}

	return true;
}
bool TEstraierDB::Update(const tchar *word, const Japa &japa)
{
	return Record(word, japa);
}

int TEstraierDB::GetGeneration()
{
	if (!db){
		ecode = EESTDB_NOT_OPENED;
		return -1;
	}

	return atoi(ReadInfo("generation"));
}


bool TEstraierDB::LoadDll()
{
	if (!Dll.Load()){
		return false;
	}
	DllLoadCounter++;
	return true;
}
void TEstraierDB::UnloadDll()
{
	DllLoadCounter--;
	if (DllLoadCounter==0){
		Dll.Unload();
	}
}

// To make the path,
// _dicname == NULL : uses the filename used in the dic object.
// _dicname != NULL : uses as the filename used in the dic object.
tnstr TEstraierDB::GetDBDir(bool create_dir, const tchar *_dicname)
{
	tnstr path = ChangeFileExt((_dicname ? _dicname : dic->GetFileName()), _t("estdb"));
	if (create_dir){
		_tmkdir(path);
	}
	path += _t("\\");
	path += dic->GetHeader()->GetIdStr();
#if 0
	if (create_dir){
		_tmkdir(path);
	}
#endif
	return path;
}

bool TEstraierDB::OpenDB(const tchar *fname, bool readonly)
{
	if (db){
		ecode = EESTDB_ALREADY_OPENED;
		return false;	// Already opened.
	}

	if (!LoadDll()){
		ecode = EESTDB_DLL;
		return false;
	}
	
	tnstr path = GetDBDir(true);
	
	if (readonly){
		if((db = Dll.est_db_open(_uustr8(path), ESTDBREADER, &ecode))==NULL){
//			fprintf(stderr, "error: %s\n", _est_err_msg(ecode));
			UnloadDll();
			return false;
		}
	} else {
		if (CanCreate){
			db = Dll.est_db_open(_uustr8(path), ESTDBWRITER | ESTDBCREAT, &ecode);
		}
		if (!db){
			if((db = Dll.est_db_open(_uustr8(path), ESTDBWRITER, &ecode))==NULL){
				//TODO: 辞書が壊れている判断はできるか？
				// できるのであれば強制的にcreateする
//				fprintf(stderr, "error: %s\n", _est_err_msg(ecode));
				UnloadDll();
				return false;
			}
		}
	}
	return true;
}
void TEstraierDB::CloseDB()
{
	if (!db)
		return;

	ResetResult();

	if (!Dll.est_db_close(db, &ecode)){
//		fprintf(stderr, "error: %s\n", _est_err_msg(ecode));
		return;
	}
	db = NULL;

	UnloadDll();
}

bool TEstraierDB::RecordItem(const char *type, const tchar *key, const tchar *text)
{
	if (!text[0]){
		return true;	// no text
	}

	/* 文書オブジェクトを生成する */
	ESTDOC *doc = Dll.est_doc_new();
	if (!doc){
		ecode = Dll.est_db_error(db);
		return false;
	}

	bool ret = true;
	
	/* 文書オブジェクトに属性を追加する */
	tnstr uri(type);
	uri += key;
	for (int i=0;i<uri.length();i++){
		if (uri[i]=='\t')
			uri[i] = WORD_ALT_SEPARATOR;
	}
	Dll.est_doc_add_attr(doc, "@uri", _uustr8(uri));
	//Dll.est_doc_add_attr(doc, "@title", "Over the Rainbow");

	/* 文書オブジェクトに本文を追加する */
	uustr text8(text);
	Dll.est_doc_add_text(doc, text8);

	/* 文書オブジェクトをデータベースに登録する */
	if(!Dll.est_db_put_doc(db, doc, ESTPDCLEAN)){
//		fprintf(stderr, "error: %s\n", _est_err_msg(Dll.est_db_error(db)));
		ecode = Dll.est_db_error(db);
		ret = false;
	}

	/* 文書オブジェクトを破棄する */
	Dll.est_doc_delete(doc);

	return ret;
}

bool TEstraierDB::DeleteItem(const char *type, const tchar *text)
{
	tnstr uri(type);
	uri += text;
	int id = Dll.est_db_uri_to_id(db, _uustr8(uri));
	if (id==-1){
		ecode = EESTDB_NO_URI;
		return false;
	}
	int option = ESTODCLEAN;	//TODO: 高速モードを用意する(docは記録しないから不要？)
	return Dll.est_db_out_doc(db, id, option);
}

bool TEstraierDB::SetAllSearch( const tchar *word, SrchMode mode, GENERICREXP *jre, AllSearchParam *all )
{
	if (!db){
		ecode = EESTDB_NOT_OPENED;
		return false;
	}

	if (cond)
		Dll.est_cond_delete(cond);

	/* 検索条件オブジェクトを生成する */
	cond = Dll.est_cond_new();
	if (!cond){
		ecode = EESTDB_MEMORY;
		return false;
	}

	dic->SetupAllSearch(word, mode, jre, all);
	
	/* 検索条件オブジェクトに検索式を設定する */
	Dll.est_cond_set_phrase(cond, _uustr8(word));

	// Search Options
	// ESTCONDSIMPLE : 簡便書式(Google like)
	// ESTCONDSURE : Uses al N-
	int option = ESTCONDSIMPLE;	// 簡便書式(Google like)
	Dll.est_cond_set_options(cond, option);

	/* データベースから検索結果を得る */
	result = Dll.est_db_search(db, cond, &resnum, NULL);
	result_index = 0;

	return true;
}
int TEstraierDB::NextAllSearch( tnstr &word, Japa *japa, AllSearchParam *all)
{
	if (!db){
		ecode = EESTDB_NOT_OPENED;
		return false;
	}
	__assert(all);

	// Retract from the result list. //

	bool found = false;
	for (;result_index<resnum;result_index++){
		int ret = FoundProc(word, japa, *all);
		switch (ret){
			case 0:		// Not found.
				continue;
			case -1:	// Error
				return AS_ERROR;
		}
		found = true;
		result_index++;
		break;
	}

	return found ? AS_FOUND : AS_END;
}
int TEstraierDB::PrevAllSearch( tnstr &word, Japa *japa, AllSearchParam *all)
{
	if (!db){
		ecode = EESTDB_NOT_OPENED;
		return false;
	}
	__assert(all);

	// Retract from the result list. //

	bool found = false;
	for (;result_index>0;result_index--){
		int ret = FoundProc(word, japa, *all);
		switch (ret){
			case 0:		// Not found.
				continue;
			case -1:	// Error
				return AS_ERROR;
		}
		found = true;
		result_index--;
		break;
	}

	return found ? AS_FOUND : AS_END;
}
bool TEstraierDB::IsMySearchMode(SrchMode mode)
{
#if 1
	return mode & SRCH_FAST ? true : false;
#else
	// １つでも条件に合わなければfalse
	if (mode & (SRCH_ALL|SRCH_HEAD|SRCH_PRON|SRCH_Q|SRCH_TITLE|SRCH_FILENAME)){
		// 対応できない検索
		return false;
	}
	if ((mode & (SRCH_WORD|SRCH_JAPA|SRCH_EXP))==0){
		// 対象となる検索項目がない
		return false;
	}
	return true;
#endif
}
tnstr TEstraierDB::GetDBPath(const tchar *dicname)
{
	return GetDBDir(false, dicname);
}
void TEstraierDB::ResetResult()
{
	if (cond){
		Dll.est_cond_delete(cond);
		cond = NULL;
	}

	resnum = 0;
	result_index = 0;
	if (result){
		free(result);
		result = NULL;
	}
}
// 0:not found
// 1:found
// -1:error
int TEstraierDB::FoundProc(tnstr &word, Japa *japa, AllSearchParam &all)
{
	/* 文書オブジェクトを取得する */
	ESTDOC *doc;
	if((doc = Dll.est_db_get_doc(db, result[result_index], 0))==NULL){
		ecode = Dll.est_db_error(db);
		return -1;
	}

	int ret = 0;
	for (;;){
		const char *value;
		if((value = Dll.est_doc_attr(doc, "@uri")) == NULL){
			// No information.
			break;
		}

		const char *uri_p = strchr(value, ':');
		if (!uri_p){
			// Unknown format.
			break;
		}
		tnstr kword((const tchar*)_uustr8(uri_p+1));
		for (int i=0;i<kword.length();i++){
			if (kword[i]==WORD_ALT_SEPARATOR)
				kword[i] = '\t';
		}

		ret = dic->read((const tchar*)_uustr8(kword), japa);
		if (ret!=1)
			break;	// PDIC dictionary error.

		if (!strncmp(value, TAG_WORD, sizeof(TAG_WORD)-1)){
			if (!(all.mode & SRCH_WORD)){
				break;
			}
		} else
		if (!strncmp(value, TAG_JAPA, sizeof(TAG_JAPA)-1)){
			if (!(all.mode & SRCH_JAPA)){
				break;
			}
		} else
		if (!strncmp(value, TAG_EXP, sizeof(TAG_EXP)-1)){
			if (!(all.mode & SRCH_EXP)){
				break;
			}
		} else
		if (IsPronEnabled() && !strncmp(value, TAG_PRON, sizeof(TAG_PRON)-1)){
			if (!(all.mode & SRCH_PRON)){
				break;
			}
		}

		word = kword;
		ret = 1;	// Found.
		break;
	}
		
	/* 文書オブジェクトを破棄する */
	Dll.est_doc_delete(doc);

	return ret;
}

// Record method for auxiary information.
bool TEstraierDB::WriteInfo(const char *key, const char *value)
{
	if (!db){
		ecode = EESTDB_NOT_OPENED;
		return false;
	}

	ESTDOC *doc = Dll.est_doc_new();
	if (!doc){
		ecode = Dll.est_db_error(db);
		return false;
	}

	bool ret = true;
	
	/* 文書オブジェクトに属性を追加する */
	tnstrA uri(TAG_INFO);
	uri += key;
	Dll.est_doc_add_attr(doc, "@uri", uri);
	Dll.est_doc_add_attr(doc, "@value", value);

	/* 文書オブジェクトをデータベースに登録する */
	if(!Dll.est_db_put_doc(db, doc, ESTPDCLEAN)){
		ecode = Dll.est_db_error(db);
		ret = false;
	}

	/* 文書オブジェクトを破棄する */
	Dll.est_doc_delete(doc);

	return ret;
}

tnstrA TEstraierDB::ReadInfo(const char *key)
{
	if (!db){
		ecode = EESTDB_NOT_OPENED;
		return "";
	}

	tnstrA uri(TAG_INFO);
	uri += key;
	int id = Dll.est_db_uri_to_id(db, uri);
	if (id==-1){
		return "";
	}
	char *value = Dll.est_db_get_doc_attr(db, id, "@value");
	if (!value){
		return "";
	}
	tnstrA ret(value);
	free(value);
	return ret;
}

bool TEstraierDB::EraseInfo(const char *key)
{
	if (!db){
		ecode = EESTDB_NOT_OPENED;
		return "";
	}

	tnstrA uri(TAG_INFO);
	uri += key;
	int id = Dll.est_db_uri_to_id(db, uri);
	if (id==-1){
		return "";
	}
	int option = ESTODCLEAN;
	return Dll.est_db_out_doc(db, id, option) ? true : false;
}

static bool IsPronEnabled()
{
	if (PronEnabled!=-1)
		return PronEnabled;
	return PronEnabled = prof.IsFastSearchPronEnabled();
}


