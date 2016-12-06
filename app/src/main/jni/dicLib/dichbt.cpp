//
// PdicにHBTree機能を追加するクラス
//

//TODO:
// ・THBDic class必要？
// ・SetAllSearch()のSrchMode mode引数いる？

#include "tnlib.h"
#pragma hdrstop
#include "dic.h"
#include "dichbt.h"
#include "HBDic.h"
#include "HBTree/HBTData.h"
#include "HBTree/HBTree.h"
#include "filestr.h"
#include "LangProc.h"
#include "japa.h"

static int PronEnabled = -1;

static bool IsPronEnabled();

TPdicHbt::TPdicHbt(PdicBase *_dic)
	:dic(_dic)
{
	db = NULL;
	ecode = 0;
	DataElems = NULL;
	MaxDataElems = 1000;	//TODO:
	TakePart = TP_WORD|TP_JAPA|TP_EXP;
}
TPdicHbt::~TPdicHbt()
{
	Close();
}

bool TPdicHbt::Open( const tchar * fname, bool _readonly )
{
	return OpenDB(fname, _readonly);
}

void TPdicHbt::Close( )
{
	CloseDB();
}
bool TPdicHbt::Record(const tchar *word, const Japa &japa)
{
	if (!db){
		ecode = EHBTDB_NOT_OPENED;
		return false;
	}

	tnstr_vec words;
	if (TakePart&TP_WORD)
		LangProc->BuildMorphWords(word, words, NULL);
	if (TakePart&TP_JAPA)
		LangProc->BuildMorphWords(japa.japa, words, NULL);
	if (TakePart&TP_EXP)
		LangProc->BuildMorphWords(japa.exp, words, NULL);
	//LangProc->BuildMorphWords(japa.pron, words);
	foreach_tnstr_vec(words, s){
		RecordItem("", s->c_str(), word);
	}
	
	return true;
}
bool TPdicHbt::Erase(const tchar *word)
{
	if (!db){
		ecode = EHBTDB_NOT_OPENED;
		return false;
	}

	return true;
}
bool TPdicHbt::Update(const tchar *word, const Japa &japa)
{
	return Record(word, japa);
}

// To make the path,
// _dicname == NULL : uses the filename used in the dic object.
// _dicname != NULL : uses as the filename used in the dic object.
tnstr TPdicHbt::GetDBName(bool /*create_dir*/, const tchar *_dicname)
{
	return ChangeFileExt((_dicname ? _dicname : dic->GetFileName()), _t(""));
}

bool TPdicHbt::OpenDB(const tchar *fname, bool readonly)
{
	if (db){
		ecode = EHBTDB_ALREADY_OPENED;
		return false;	// Already opened.
	}

	tnstr path = GetDBName(true);
	
	db = new THBDic();
	if (!db){
		ecode = EHBTDB_MEMORY;
		return false;
	}
	if (!db->Open(path, readonly)){
		_delete(db);
		return false;
	}
	return true;
}
void TPdicHbt::CloseDB()
{
	if (!db)
		return;

	ResetResult();
	if (DataElems){
		delete[] DataElems;
		DataElems = NULL;
	}

	_delete(db);
}

bool TPdicHbt::RecordItem(const char *type, const tchar *key, const tchar *text)
{
	__assert(key[0] && text[0]);
	if (!db)
		return false;	// not opened
		
	return db->Record(key, text, _tcslen(text));
}

bool TPdicHbt::DeleteItem(const char *type, const tchar *text)
{
	//TODO:
	return false;
}

bool TPdicHbt::SetAllSearch( const tchar *word, SrchMode mode, GENERICREXP *jre, AllSearchParam *all )
{
	if (!db){
		ecode = EHBTDB_NOT_OPENED;
		return false;
	}

	if (!DataElems){
		DataElems = new HBTDataElem*[MaxDataElems];
	}

	DataElemIndex = 0;
	DataElemCount = db->Hbt->GetData(word, DataElems, MaxDataElems);
	if (DataElemCount<0)
		return false;
	
	return true;
}
int TPdicHbt::NextAllSearch( tnstr &word, Japa *japa, AllSearchParam *all)
{
	if (!db){
		ecode = EHBTDB_NOT_OPENED;
		return false;
	}
	__assert(all);

	// Retract from the result list. //
	if (DataElemIndex>=DataElemCount)
		return AS_END;

	bool found = false;

	word = (tchar*)DataElems[DataElemIndex++]->rawdata;	//TODO: たぶんnull terminator必要
	if (japa){
		//TODO: readではなく、頭だし検索でhashを比較する必要がある
		if (dic->read(LangProc->CompositeWord(word), japa)==1){
			found = true;
		}
	}
		
#if 0
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
#endif

	return found ? AS_FOUND : AS_END;
}
int TPdicHbt::PrevAllSearch( tnstr &word, Japa *japa, AllSearchParam *all)
{
	if (!db){
		ecode = EHBTDB_NOT_OPENED;
		return false;
	}
	__assert(all);

	//TODO: Retract from the result list. //

	bool found = false;

#if 0
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
#endif

	return found ? AS_FOUND : AS_END;
}
bool TPdicHbt::IsMySearchMode(SrchMode mode)
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
tnstr TPdicHbt::GetDBPath(const tchar *dicname)
{
	return GetDBName(false, dicname);
}
void TPdicHbt::ResetResult()
{
	DataElemIndex = 0;
}
// 0:not found
// 1:found
// -1:error
int TPdicHbt::FoundProc(tnstr &word, Japa *japa, AllSearchParam &all)
{
#if 0
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
		
	return ret;
#endif
	return -1;
}

// Record method for auxiary information.
bool TPdicHbt::WriteInfo(const char *key, const char *value)
{
#if 0
	if (!db){
		ecode = EHBTDB_NOT_OPENED;
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
#endif
	return false;
}

tnstrA TPdicHbt::ReadInfo(const char *key)
{
#if 0
	if (!db){
		ecode = EHBTDB_NOT_OPENED;
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
#endif
	return NULL;
}

bool TPdicHbt::EraseInfo(const char *key)
{
	if (!db){
		ecode = EHBTDB_NOT_OPENED;
		return false;
	}

#if 0
	tnstrA uri(TAG_INFO);
	uri += key;
	int id = Dll.est_db_uri_to_id(db, uri);
	if (id==-1){
		return "";
	}
	int option = ESTODCLEAN;
	return Dll.est_db_out_doc(db, id, option) ? true : false;
#endif
	return false;
}

#if 0
static bool IsPronEnabled()
{
	if (PronEnabled!=-1)
		return PronEnabled;
	return PronEnabled = prof.IsFastSearchPronEnabled();
}
#endif


#if 0
int TPdicHbt::_record( const _kchar * word, uint wordlen, Japa &japa)
{
	int ret = super::_record(word, wordlen, japa);
	if (ret<0)
		return ret;
	if (HBDic){
		
//		HBDic->Record();
	}
	return ret;
}
int TPdicHbt::_erase( )
{
	int ret = super::_erase();
	if (ret<0)
		return ret;
	if (HBDic){
		
	}
	return ret;
}
int TPdicHbt::_update( const _kchar * word, uint wordlen, Japa &japa)
{
	int ret = super::_update(word, wordlen, japa);
	if (ret<0)
		return ret;
	if (HBDic){
		
	}
	return ret;
}
#endif

