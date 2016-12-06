//---------------------------------------------------------------------------
#ifndef DictionaryH
#define DictionaryH
//---------------------------------------------------------------------------

#include "strlib.h"

#include <vector>

using namespace std;

class TDllDictionaryBase {
protected:
	int error;
	int app;
	int mod;
	int def;
	tnstr name;
public:
	TDllDictionaryBase();
	virtual ~TDllDictionaryBase(){}
	int GetErrorCode( );
	virtual int _getError( )
		{return error;}
	virtual int Open( const tchar *fname, int mode );
	virtual void InitReadPare( MergeMode, int TransFlags ) = 0;
};

// TObjDataT //
class TMemoryObject;
template <class T>
class TObjDataT {
public:
	int objtype;
	int id;
	T *title;
	T *filename;	// for filelink
	TMemoryObject *data;
	// for epwing
	int bookno;
	int pos;		// extra data when JLUFO
public:
	TObjDataT();
	virtual ~TObjDataT();
	int GetObjType() const
		{ return objtype; }
	int GetId() const
		{ return id; }
	const T *GetTitle() const
		{ return title; }
	void SetTitle(const T *title);
	void SetFileName(const T *filename);
	const T *GetFileName() const
		{ return filename; }
	void SetData(char *data);
	TMemoryObject *GetData()
		{ return data; }
	virtual void *GetOleParamImpl() = 0;
	struct _PDCOLEDATAPARAM *GetOleParamPtr()
		{ return (struct _PDCOLEDATAPARAM*)GetOleParamImpl(); }
	virtual DWORD *GetAspectPtr() = 0;
};

template <class T>
TObjDataT<T>::TObjDataT()
{
	objtype = PDCOBJ_BINARY;
	id = -1;
	title = NULL;
	filename = NULL;
	data = NULL;
	bookno = 0;
	pos = 0;
}
template <class T>
TObjDataT<T>::~TObjDataT()
{
	if (title)
		delete[] title;
	if (filename)
		delete[] filename;
	if (data)
		data->deref();
}
template <class T>
void TObjDataT<T>::SetTitle(const T *_title)
{
	if (title)
		delete[] title;
	int len = _strlen(_title);
	title = new T[len+1];
	memcpy(title, _title, len*sizeof(T));
	title[len] = '\0';
}

// JapaT //
template <class T>
class JapaT {
typedef TObjDataT<T> obj_t;
typedef vector<obj_t*> objarray_t;
public:
	unsigned char attr;
	objarray_t objects;	// Not own the object.(reference pointer)
	JapaT();
	virtual ~JapaT();
	int getObjNum() const
		{ return objects.size(); }
	void clearObject()
	{
		objects.clear();
	}
};
template <class T>
JapaT<T>::JapaT()
{
}
template <class T>
JapaT<T>::~JapaT()
{
	for (objarray_t::iterator it=objects.begin();it!=objects.end();it++){
		if (*it){
			delete *it;
		}
	}
}

#if 0
class Japa {
public:
	tnstr japa;
	tnstr pron;
	tnstr exp;
	unsigned char attr;
};
#endif

#define	FOM_READONLY		1
#define	FOM_CREATE			2
#define	FOM_WRITE			3
#define	FOM_OPENMODEMASK	3
#define	FOM_MERGE			8		// Windows版でCanOpen()の問合わせ無しの指示
#define	FOM_NOTREADOLE		0x10	// OLEデータは転送しない
#define	FOM_NOTREADFILE		0x20	// ファイルリンクデータは転送しない
#define	FOM_NOTREADOBJ		0x30	// リンクオブジェクトデータは転送しない
#define	FOM_FILE			0x100	// File

#define	TRS_ALL		0x7fff0000

#if defined(DEFINE_DICTIONARY_CLASS) || !defined(DEF_SRCHMODE)
#define	DEF_SRCHMODE
enum SrchMode {
	//検索対象フラグ
	SRCH_WORD	= 0x01,		//単語の検索		(SRCH_WORDとSRCH_JAPAはOR可能)
	SRCH_JAPA	= 0x02,		//日本語訳の検索
	SRCH_ALL	= 0x04,		//全検索
	SRCH_HIDE	= 0x08,		//隠し単語の検索(辞書Ver.1.10以降)
	SRCH_MEMORY	= 0x0c,		//必須単語の検索(辞書Ver.1.10以降)
	SRCH_JEDIT	= 0x10,		//修正単語の検索
	SRCH_EXP	= 0x20,		// 用例検索
	SRCH_LEVEL	= 0x40,		// 単語レベル検索
	SRCH_PRON	= 0x80,		// 発音記号検索（未対応）
	SRCH_Q		= 0x100,	// ?単語検索
	SRCH_TITLE	= 0x200,	// オブジェクトタイトル検索
	SRCH_FILENAME = 0x400,	// ファイルリンクファイル名

	//検索モードフラグ
	SRCH_IGN	= 0x0800,	//大文字・小文字の区別
	SRCH_HEAD	= 0x1000,	//検索文字が英語部の先頭

	SRCH_REGEXP	= 0x2000,	//正規表現検索
	SRCH_ZENHAN	= 0x4000,	//全角半角同一視
	SRCH_KANA	= 0x8000,	//ひらがな・カタカナ同一視
	SRCH_FUZZY	= 0x10000,	// 曖昧検索
	SRCH_WORDSRCH = 0x20000,	// 単語検索
#ifdef __WIN32__
	SRCH_WORD_S = 0x100000,	// 英単語はシングルバイト文字
	SRCH_JAPA_S = 0x200000,	// 日本語訳はシングルバイト文字
	SRCH_EXP_S  = 0x400000	// 用例はシングルバイト文字
//    SRCH_PRON_S = 0x800000;	// 発音記号は常にシングルバイト
#endif
#ifdef ANSI
	SRCH_dummy = 0xffffffff
#endif
};
#endif	// DEFINE_DICTIONARY_CLASS || !DEF_SRCHMODE

// AllSearch用定数
#define	AS_CONTINUE		0
#define	AS_FOUND		1
#define	AS_ERROR		-1
#define	AS_END			-2
#define	AS_STOP			-3

#define	GETPROC( type, typename ) \
	fn##type = \
		(FN##type)GetProcAddress( hDll, typename );

#define	FNEXEC0( type, typename ) \
	FN##type fn##type; \
	GETPROC( type, typename ); \
	fn##type( );

#define	FNEXEC1( type, typename, arg1 ) \
	FN##type fn##type; \
	GETPROC( type, typename ); \
	fn##type( arg1 );

#define	FNEXEC0R( return ,type, typename ) \
	FN##type fn##type; \
	GETPROC( type, typename ); \
	return = fn##type( );

// Endian Mode //
#define	LITTLEENDIAN	0
#define	BIGENDIAN		1

#define	UTF16			0
#define	UTF8			1
#define	BOCU			2
#define	HYPER3			3	// version 6

#endif

