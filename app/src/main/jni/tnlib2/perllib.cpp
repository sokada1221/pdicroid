//---------------------------------------------------------------------------
// Perl Like Library for C language
//---------------------------------------------------------------------------
// UTF-8(i.e. Single Byte Code)‚ð‘O’ñ(wide char‚Í‚â‚ç‚È‚¢-‚â‚è‚½‚­‚È‚¢)
#include <windows.h>
#pragma hdrstop
#include <sys/stat.h>

#include "perllib.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

const string string_null;

string_vec *split_new(const char *delim, const char *str)
{
	string_vec *vec = new string_vec;
	string buf(str);
	char *p = _tcstok(const_cast<char*>(buf.c_str()), delim);
	while (p){
		vec->push_back(p);
		p = _tcstok(NULL, delim);
	}
	return vec;
}
// Œø—¦‚ªˆ«‚¢‚Ì‚Å‚ ‚Ü‚èŽg—p‚µ‚È‚¢
string_vec split(const char *delim, const char *str)
{
	string_vec strs;
	split(str, strs, delim);
	return strs;
}
void split(const char *str, string_vec &array, const char *delim)
{
	array.clear();

	string s(str);
	const char *p = _tcstok(const_cast<char*>(s.c_str()), delim);
	for (;p!=NULL;){
		array.push_back(p);
		p = _tcstok(NULL, delim);
	}
}
void push(string_vec &vec, string_vec *s)
{
	foreach_string_vec(*s, it){
		vec.push_back(*it);
	}
}

// svec = keys smap;
void keys(string_map &smap, string_vec &svec)
{
	svec.clear();
	foreach_string_map(smap, it){
		svec.push_back(it->first);
	}
}

bool IsFile(const char *file)
{
	WIN32_FIND_DATA wfd;
	HANDLE h = FindFirstFile(file, &wfd);
	if (h==INVALID_HANDLE_VALUE)
		return false;
	FindClose(h);
	if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		return false;
	return true;
}
bool IsDirectory(const char *dir)
{
	WIN32_FIND_DATA wfd;
	HANDLE h = FindFirstFile(dir, &wfd);
	if (h==INVALID_HANDLE_VALUE)
		return false;
	FindClose(h);
	if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		return true;
	return false;
}
// TODO: not debugged
__int64 FileSize(const char *file)
{
	HANDLE h = CreateFile( file, GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
	if (h==INVALID_HANDLE_VALUE)
		return -1;
	DWORD high;
	DWORD size = GetFileSize(h, &high);
	CloseHandle(h);
	return ((__int64)high)<<32 | size;
}
#if 0	//TBD:
time_t st_mtime(const char *file)
{
#if 0	// under construction
	HANDLE h = CreateFile( file, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );
	if (h==INVALID_HANDLE_VALUE)
		return 0;
	FILETIME ft;
	if (!GetFileTime(h, NULL, NULL, &ft))
		return 0;
#endif
	struct stat sbuf;
	if (stat(file, &sbuf)==-1)
		return 0;
	return sbuf.st_mtime;
}
bool utime(time_t atime, time_t mtime, const string &file)
{
	FILETIME aft;
	FILETIME mft;
	time_t_to_filetime(atime, &aft);
	time_t_to_filetime(atime, &mft);
	HANDLE h = CreateFile( file.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );
	BOOL ret = SetFileTime( h, NULL, &aft, &mft );
	CloseHandle(h);
	return ret;
}
#endif	//TBD:
int touch(const char *file, time_t date)
{
	if (date==0){
		return utime(time(NULL), time(NULL), file);
	} else {
		return utime(date, date, file);
	}
}
// YYYYMMDDHHMMSS -> time_t
time_t timelocal(string &date)
{
	struct tm t;
	memset(&t, 0, sizeof(tm));
	t.tm_year = atoi(date.substr(0,4).c_str())-1900;
	t.tm_mon = atoi(date.substr(4,2).c_str())-1;
	t.tm_mday = atoi(date.substr(6,2).c_str());
	t.tm_hour = atoi(date.substr(8,2).c_str());
	t.tm_min = atoi(date.substr(10,2).c_str());
	t.tm_sec = atoi(date.substr(12,2).c_str());
	return mktime(&t);
}
int compare(const char *file1, const char *file2)
{
	//TODO: file compare
	FILE *fp1 = fopen(file1, "rb");
	if (!fp1)
		return -1;
	FILE *fp2 = fopen(file2, "rb");
	if (!fp2)
		return -1;
	const int bufsize = 2048;
	char *buf1 = new char[bufsize];
	char *buf2 = new char[bufsize];
	int ret = 1;	// diff
	for (;;){
		int rb1 = fread(buf1, 1, bufsize, fp1);
		if (rb1<=0)
			break;
		int rb2 = fread(buf2, 1, bufsize, fp2);
		if (rb2<=0)
			break;
		if (rb1!=rb2)
			break;
		ret = memcmp(buf1, buf2, rb1);
		if (ret!=0)
			break;
	}
	delete[] buf1;
	delete[] buf2;
	return ret;
}
void time_t_to_filetime(time_t t, FILETIME *ft)
{
	__int64 li = ((__int64)t+11644473600)*10000000;	// time_t -> FILETIME conversion
	ft->dwLowDateTime = (DWORD)li;
	ft->dwHighDateTime = (DWORD)(li>>32);
}
// trim left and right of string
void trim(string &s)
{
	trimleft(s);
	trimright(s);
}
void trimleft(string &s)
{
	unsigned i;
	for (i=0;i<s.length();i++){
		if (s.at(i)==' '||s.at(i)=='\t')
			continue;
		break;
	}
	if (i!=0)
		s.erase(0, i);
}
void trimright(string &s)
{
	unsigned i;
	unsigned lastpos = -1;
	for (i=0;i<s.length();i++){
		if (s.at(i)==' '||s.at(i)=='\t')
			lastpos = i;
		lastpos = -1;
	}
	if (lastpos!=-1)
		s.erase(lastpos);
}
// find s1 in s, and replace all matches to s2
void replace(string &s, const char *s1, const char *s2)
{
	int len1 = strlen(s1);
	int len2 = strlen(s2);
	for(unsigned pos=0;pos<s.length();){
		int fpos = s.find(s1, pos);
		if (fpos<0)
			break;
		s.replace(fpos, len1, s2);
		pos = fpos + len2;
	}
}
string itoa(int integer)
{
	char buf[20];
	::itoa(integer, buf, 10);
	return string(buf);
}

// erase characters after a character which is one of delim.
void erase(string &s, const char *delim)
{
	for (unsigned i=0;i<s.length();i++){
		if (strchr(delim, s.at(i))){
			s.erase(i);
			break;
		}
	}
}

void chomp(string &s)
{
	erase(s, "\r\n");
}

// helper library for STL //
string sprintf(const char *format, ...)
{
	va_list argptr;
	va_start(argptr, format);

	char buf[1024];
	string ret;
	int size = vsnprintf(buf, sizeof(buf), format, argptr);
	if (size<sizeof(buf)){
		ret = buf;
	}

	va_end(argptr);

	return ret;
}

