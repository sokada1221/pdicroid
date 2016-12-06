#include "tnlib.h"
#pragma hdrstop
#include "dic.h"
#include "japa.h"
#include "LangProc.h"
#include "dicextdb-pdic.h"
#include "filestr.h"

static int fwords_comp(const tchar *w1, int len1, const tchar *w2, int len2)
{
	int len = min(len1, len2);
	int r = _tcsncmp(w1, w2, len);
	if (r==0){
		return len1-len2;
	} else {
		return r;
	}
}

int TExtPdic::fwords_t::fwords_comp( const void *_o1, const void *_o2 )
{
#if 1
	return ::fwords_comp(
		((TStrLoc*)_o1)->word, ((TStrLoc*)_o1)->length,
		((TStrLoc*)_o2)->word, ((TStrLoc*)_o2)->length
		);
#else
	TStrLoc &o1 = *(TStrLoc*)_o1;
	TStrLoc &o2 = *(TStrLoc*)_o2;
	int len = min(o1.length, o2.length);
	int r = _tcsncmp(o1.word, o2.word, len);
	if (r==0){
		return o1.length-o2.length;
	} else {
		return r;
	}
#endif
}

TExtPdic::TExtPdic(PdicBase *_dic)
	:dic(_dic)
{
	db = NULL;
	TakePart = TP_WORD|TP_JAPA|TP_EXP;
	FoundJapa = NULL;
}

TExtPdic::~TExtPdic()
{
	Close();
}

bool TExtPdic::Open( const tchar * fname, bool _readonly )
{
	return OpenDB(fname, _readonly);
}

bool TExtPdic::Create(const tchar *fname)
{
	return CreateDB(fname);
}

void TExtPdic::Close( )
{
	CloseDB();
}
bool TExtPdic::Record(const tchar *word, const Japa &japa)
{
	if (!db){
		//ecode = EHBTDB_NOT_OPENED;
		return false;
	}

	tnstr_vec words;
	if (TakePart&TP_WORD)
		LangProc->BuildMorphWords(find_cword_pos(word), words, NULL);
	if (TakePart&TP_JAPA)
		LangProc->BuildMorphWords(japa.japa, words, NULL);
	if (TakePart&TP_EXP)
		LangProc->BuildMorphWords(japa.exp, words, NULL);
	//LangProc->BuildMorphWords(japa.pron, words);

	const tchar *cword = find_cword_pos(word);
	
	foreach_tnstr_vec(words, s){
		RecordItem("", s->c_str(), cword);
	}
	
	return true;
}
bool TExtPdic::Erase(const tchar *word)
{
	if (!db){
		//ecode = EHBTDB_NOT_OPENED;
		return false;
	}

	return true;
}
bool TExtPdic::Update(const tchar *word, const Japa &japa)
{
	return Record(word, japa);
}

// To make the path,
// _dicname == NULL : uses the filename used in the dic object.
// _dicname != NULL : uses as the filename used in the dic object.
tnstr TExtPdic::GetDBName(bool /*create_dir*/, const tchar *_dicname)
{
	return ChangeFileExt((_dicname ? _dicname : dic->GetFileName()), _t("ext"));
}

bool TExtPdic::OpenDB(const tchar *fname, bool readonly)
{
	if (db){
		//ecode = EHBTDB_ALREADY_OPENED;
		return false;	// Already opened.
	}

	tnstr path = GetDBName(true);
	
	db = Pdic::CreateInstance(path);
	if (!db){
		//ecode = EHBTDB_MEMORY;
		return false;
	}
	if (db->Open(path, readonly)!=0){
		db->Release();
		db = NULL;
		return false;
	}
	return true;
}
bool TExtPdic::CreateDB(const tchar *fname)
{
	if (db){
		return false;	// already opene.d
	}

	tnstr path = GetDBName(true);
	
	db = Pdic::CreateInstance(path);
	if (!db){
		//ecode = EHBTDB_MEMORY;
		return false;
	}
	if (db->CreateHyper(path)!=0){
		db->Release();
		db = NULL;
		return false;
	}
	return true;
}

void TExtPdic::CloseDB()
{
	if (!db)
		return;

	ResetResult();
	//FoundWords.clear();
	delete FoundJapa;
	FoundJapa = NULL;

	db->Release();
	db = NULL;
}

bool TExtPdic::RecordItem(const char *type, const tchar *key, const tchar *text)
{
	__assert(key[0] && text[0]);
	if (!db)
		return false;	// not opened

	Japa japa;
	fwords_t words;
	if (db->read(key, &japa)==1){
		// found and find the insert-point.
		const tchar *ip = FindInsertPoint(japa.japa, text);
		if (!ip)
			return false;	// already recorded.
		Japa recj;
		if (japa.japa != ip){
			recj.japa.set(japa.japa, STR_DIFF(ip, japa.japa.c_str()));
		}
		if (!*ip)
			recj.japa += _t("\a");	// delemeter
		recj.japa += text;
		if (*ip){
			recj.japa += _t("\a");	// delemeter
			recj.japa += ip;
		}
		return db->Record(key, recj);
	} else {
		// new record
		japa.japa = text;
		return db->Record(key, japa);
	}
}

bool TExtPdic::DeleteItem(const char *type, const tchar *text)
{
	//TODO:
	return false;
}

bool TExtPdic::SetAllSearch( const tchar *word, SrchMode mode, GENERICREXP *jre, AllSearchParam *all )
{
	if (!db){
		//ecode = EHBTDB_NOT_OPENED;
		return false;
	}

	tnstr_vec words;
	LangProc->BuildMorphWords(find_cword_pos(word), words, NULL);

	if (words.size()==0){
		// not enough word count.
		return false;
	}

	//WordIndex = 0;

	if (!FoundJapa)
		FoundJapa = new Japa;
	else
		FoundJapa->clear();

	db->read(words[0], FoundJapa);
	SetupFoundWords();
	
	return true;
}

int TExtPdic::NextAllSearch( tnstr &word, Japa *japa, AllSearchParam *all)
{
	if (!db){
		return AS_END;
	}
	__assert(all);

	// Retract from the result list. //
#if 0
	if (WordIndex>=FoundWords.size())
		return AS_END;
	FoundWords[WordIndex++].get(word);
#else
	if (!WordPtr || !*WordPtr)
		return AS_END;
	const tchar *top = WordPtr;
	while (*WordPtr && *WordPtr !='\a') WordPtr++;
	word.set(top, STR_DIFF(WordPtr, top));
	if (*WordPtr=='\a') WordPtr++;	// next pointer
#endif

	bool found = false;
	if (japa){
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
int TExtPdic::PrevAllSearch( tnstr &word, Japa *japa, AllSearchParam *all)
{
	if (!db){
		return AS_END;
	}
	__assert(all);

#if 0
	if (WordIndex==0)
		return AS_END;
	FoundWords[WordIndex--].get(word);
#else
	if (!WordPtr || WordPtr==FoundJapa->japa.c_str())
		return AS_END;
	const tchar *top = FoundJapa->japa.c_str();
	const tchar *p = WordPtr;
	while (p>top && *p!='\a') p--;
	if (*p=='\a') p++;
	word.set(p, STR_DIFF(WordPtr-1, p));
	WordPtr = p;	// next pointer
#endif

	bool found = false;

	if (japa){
		if (dic->read(LangProc->CompositeWord(word), japa)==1){
			found = true;
		}
	}

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
bool TExtPdic::IsMySearchMode(SrchMode mode)
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
tnstr TExtPdic::GetDBPath(const tchar *dicname)
{
	return GetDBName(false, dicname);
}
void TExtPdic::ResetResult()
{
	//WordIndex = 0;
	WordPtr = NULL;
}
void TExtPdic::SetupFoundWords()
{
	//ParseFoundWords(FoundWords, FoundJapa->japa);
	WordPtr = FoundJapa->japa;
}

void TExtPdic::ParseFoundWords(fwords_t &words, const tchar *text)
{
	const tchar *top = text;
	const tchar *p = top;
	while (true){
		if (*p=='\a' || !*p){
			if (top!=p){
				words.add(new TStrLoc(top, STR_DIFF(p,top)));
			}
			if (!*p)
				break;
			top = ++p;
		} else {
			p++;
		}
	}
}

const tchar *TExtPdic::FindInsertPoint(const tchar *words, const tchar *text)
{
	int textlen = _tcslen(text);

	const tchar *p = words;
	const tchar *top = p;
	while (true){
		if (*p=='\a' || !*p){
			if (top!=p){
				int r = fwords_comp(top, STR_DIFF(p,top), text, textlen);
				if (r>=0){
					if (r==0)
						return NULL;	// found the same word
					return top;
				}
			}
			if (!*p)
				break;
			top = ++p;
		} else {
			p++;
		}
	}
	return p;
}


// 0:not found
// 1:found
// -1:error
int TExtPdic::FoundProc(tnstr &word, Japa *japa, AllSearchParam &all)
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
bool TExtPdic::WriteInfo(const char *key, const char *value)
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

tnstrA TExtPdic::ReadInfo(const char *key)
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

bool TExtPdic::EraseInfo(const char *key)
{
	if (!db){
		//ecode = EHBTDB_NOT_OPENED;
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

