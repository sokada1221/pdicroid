//---------------------------------------------------------------------------
// TID map
// TID_ORIGIN : 0x10000000 (application depend)
// TID_EXTERN : 0x20000000 外部から追加登録できる
// TID_TEMP   : 0x30000000 一時的に追加できる（寿命はMAX_HOLDER_NUMに依存)

#include "tnlib.h"
#pragma hdrstop

#include <string>
#include <vector>
#include <map>
using namespace std;

#include "defs.h"
#include "tnstr.h"
#include "TextResource.h"

#define	USE_MMAP	0	// NULL終端が必要なのであまり効率よくならない・・・
						// 2007.5.29 途中で断念

#if USE_MMAP
#include "mmf.h"
#include "filebuf.h"
#endif

//---------------------------------------------------------------------------

#pragma package(smart_init)

#define	TID_EXTERN	0x20000000
#define	TID_TEMP	0x30000000

#define	TO_LINEINDEX(x)		((x)>>4)
#define	TO_LINENUM(x)		((x)<<4)
#define	MOD_INDEXSIZE(x)	((x)&0xF)

#define	POSARRAY_SLOTSIZE		16

class TPosArray {
protected:
	int *array;
	int slot_size;
	int slot_num;
public:
	TPosArray()
	{
		array = NULL;
		slot_size = POSARRAY_SLOTSIZE;
		slot_num = 0;
	}
	virtual ~TPosArray()
	{
		if (array){
			free(array);
		}
	}
	bool allocate(int req_num)
	{
		int size = req_num*sizeof(int);
		if ( size <= slot_size * slot_num ){
			return true;	// Not changed (enough space);
		}
		int old_slot_num = slot_num;
		slot_num = (size+slot_size-1) / slot_size;
		if (!realloc()){
			return false;
		}
		// Pad the growed area with -1.
		memset(&array[old_slot_num*slot_size], 0xff, (slot_num-old_slot_num)*slot_size*sizeof(int));
		return true;
	}
	int &operator [] (int i)
		{ return array[i]; }
protected:
	bool realloc( )
	{
		int *_array = (int*)::realloc( array, sizeof(int) * slot_size * slot_num );
		if ( _array == 0 ){
			return false;
		}
		array = _array;
		return true;
	}
};

class TTextResource {
protected:
	TMutex Mutex;
	int OrgIndex;
	TIFile File;
#if USE_MMAP
	TMmfMap *MM;
	char *MMTop;
	int MMPos;
	int FileSize;
#endif
	TPosArray IdPos;	// １０行単位のファイル位置
public:
	TTextResource(const tchar *filename, int orgIndex, int preAllocIdPos=0)
	{
		OrgIndex = orgIndex;
		IdPos.allocate(preAllocIdPos+POSARRAY_SLOTSIZE);
#if USE_MMAP
		MM = NULL;
		MMTop = NULL;
		MMPos = 0;
		FileSize = 0;
#endif
		if (filename){
			Open(filename, orgIndex);
		}
	}
	virtual ~TTextResource()
	{
	}
	void Lock()
	{
		Mutex.Lock();
	}
	void Unlock()
	{
		Mutex.Unlock();
	}
	bool IsOpened()
	{
		return File.is_open();
	}
	bool Open(const tchar *filename, int orgIndex=-1)
	{
		if (File.is_open()){
			return true;
		}
		bool ret;
		Lock();
		if (orgIndex!=-1){
			OrgIndex = orgIndex;
		}
		ret = (File.open(filename)!=-1);
		if (ret){
			IdPos[0] = file_tell();
#if USE_MMAP
			// Get the file size.
			File.end();
			FileSize = File.tell();
			File.seek(IdPos[0]);
			// Create the memory mapping object.
			MM = new TMmfMap((HANDLE)File.get_handle());
			if (MM){
				MMTop = MM->map(0, FileSize, true);
			} else {
				MMTop = NULL;
			}
			if (!MMTop){
				if (MM){
					delete MM;
					MM = NULL;
				}
				return false;
			}
			MMPos = 0;
#endif
		}
		Unlock();
		return ret;
	}
public:
	//TODO: いずれTTextResource -> tnlib
	bool Find(int id, tnstr &str)
	{
		id -= OrgIndex;
		if (!IsOpened()){
			return false;
		}
		if (!IdPos.allocate(id+1)){
			return false;	// Not enough memory.
		}
		Lock();
		int lineindex = TO_LINEINDEX(id);
		int pos = IdPos[lineindex];
		if (pos!=-1){
			file_seek(pos);
		} else {
			// lineindex is not read yet.
			if (IdPos[1]==-1){
				lineindex = 0;
				file_seek(IdPos[0]);
			} else {
				// Find the most large lineindex.
				for (lineindex--;lineindex>=0;lineindex--){
					if (IdPos[lineindex]!=-1){
						// Found the last lineindex.
						file_seek(IdPos[lineindex]);
						break;
					}
				}
				// 必ず見つかる
			}
		}
		for (int i=TO_LINENUM(lineindex);i<=id;i++){
			if (MOD_INDEXSIZE(i)==0){
				if (IdPos[TO_LINEINDEX(i)]==-1){
					IdPos[TO_LINEINDEX(i)] = file_tell();
				}
			}
#if USE_MMAP
			if (MMPos>=FileSize){
				// EOF
				Unlock();
				return false;	// Not found.
			}
#else
			for (;;){
				if (File.getline(str)<0){
					// EOF
					Unlock();
					return false;	// Not found
				}
				// Skip comment line(s).
				if (!IsCommentLine(str)){
					break;
				}
			}
#endif
		}
		Unlock();
		return true;
	}
protected:
	void Close()
	{
#if USE_MMAP
		if (MM){
			delete MM;
			MM = NULL;
		}
#endif
		File.close();
	}
	void file_seek(int pos)
	{
#if USE_MMAP
		MMPos = pos;
#else
		File.seek(pos);
#endif
	}
	int file_tell()
	{
#if USE_MMAP
		return MMPos;
#else
		return File.tell();
#endif
	}
	inline bool IsCommentLine(const tchar *str)
	{
		return str[0]=='\0' || str[0]==';' || str[0]=='#';
	}
};

static TTextResource *TR = NULL;

#define	MAX_HOLDER_NUM		8
static tnstr_vec StrHolder;
static int CurIndex = 0;

#define	MAX_EXTERN_NUM	16
static tchar *ExternStrings[MAX_EXTERN_NUM] = {NULL};


bool InitTextResource(const tchar *filename, int orgIndex, int preAllocSize)
{
	if (TR){
		return true;
	}
	for (int i=0;i<MAX_EXTERN_NUM;i++){
		ExternStrings[i] = NULL;
	}
	TR = new TTextResource(filename, orgIndex, preAllocSize);
	return TR!=NULL;
}

void CleanupTextResource()
{
	if (TR){
		delete TR;
		TR = NULL;
	}
	for (int i=0;i<MAX_EXTERN_NUM;i++){
		if (ExternStrings[i]){
			delete[] ExternStrings[i];
			ExternStrings[i] = NULL;
		}
	}
}

const tchar *GetTextResource(int id)
{
	if (id>=TID_EXTERN){
		if (id>=TID_TEMP){
			if (id<TID_TEMP+MAX_HOLDER_NUM){
				return StrHolder[id-TID_TEMP];
			} else {
				DBW("id error:%x", id);
				return _t("");
			}
		} else {
			if (id<TID_EXTERN+MAX_EXTERN_NUM){
				return ExternStrings[id-TID_EXTERN];
			} else {
				return _t("");
			}
		}
	}
	if (!TR)
		return _t("");
	TR->Lock();
	CurIndex &= (MAX_HOLDER_NUM-1);
	if (StrHolder.get_num()<MAX_HOLDER_NUM){
		StrHolder.add(_T(""));
	}
	tnstr str;
	bool ret = TR->Find(id, str);
	TR->Unlock();
	if (!ret){
		return _t("");
	}
	// Skip ID.
	const tchar *p = str;
	for (;;){
		if (!*p)
			break;
		if (*p=='='){
			p++;
			break;
		}
		p++;
	}
	// Unescape control.
	tnstr &dstr = StrHolder[CurIndex++];
	dstr.clear();
	const tchar *startp = p;
	for (;*p;){
		if (!*p)
			break;
		if (*p=='\\'){
			dstr.cat(startp, STR_DIFF(p, startp));
			p++;
			switch (*p){
				case 'r':
					dstr += _t("\r");
					break;
				case 'n':
					dstr += _t("\n");
					break;
				case 't':
					dstr += _t("\t");
					break;
				default:
					startp = p;
					goto jnext;
			}
			p++;
			startp = p;
			continue;
		}
jnext:;
		p = CharNext(p);
	}
	if (startp<p){
		dstr.cat(startp, STR_DIFF(p,startp));
	}
	return dstr;
}
bool GetTextResource(int id, __tnstr &str)
{
	return TR->Find(id, str);
}

void GetTextResourceArray(tnstr_vec &array, int id, const tchar *delim)
{
	array.clear();
	tchar *str = (tchar*)GetTextResource(id);
	if (!str)
		return;
	tchar *p = _tcstok(str, delim);
	while (p){
		array.add(p);
		p = _tcstok(NULL, delim);
	}
}

// text=NULL : delete the string of id.
int SetTextResource(const tchar *text, int id)
{
	if (id<TID_EXTERN || id>=TID_EXTERN+MAX_EXTERN_NUM)
		return -1;
	int index = id-TID_EXTERN;
	if (ExternStrings[index]){
		delete[] ExternStrings[index];
		ExternStrings[index] = NULL;
	}
	if (text){
		ExternStrings[index] = _tcsdup(text);
	}
	return id;
}
int SetTextTempResource(const tchar *text)
{
	CurIndex &= (MAX_HOLDER_NUM-1);
	if (StrHolder.get_num()<MAX_HOLDER_NUM){
		StrHolder.add(_T(""));
	}
	StrHolder[CurIndex++] = text;
	return TID_TEMP+CurIndex-1;
}

