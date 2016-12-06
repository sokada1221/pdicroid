#include "stdafx.h"
#include <windows.h>
#pragma hdrstop
#include "timex.h"

#define	TIMEDIFF		11644473600			// = (system time) - time()
#define FT_TO_SEC(sec)	((sec)/10000000)	// FILETIME to sec
#define	SEC_TO_FT(sec)	((sec)*10000000)	// sec to FILETIME

inline timex_t FileTimeToSec(const FILETIME &ft)
{
	LARGE_INTEGER li;
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	return FT_TO_SEC(li.QuadPart);
}

inline void SecToFileTime(timex_t t, FILETIME &ft)
{
	LARGE_INTEGER li;
	li.QuadPart = SEC_TO_FT(t);
	ft.dwLowDateTime = li.LowPart;
	ft.dwHighDateTime = li.HighPart;
}

inline void SystemTimeToTM(const SYSTEMTIME &st, struct tm &d)
{
	d.tm_year = st.wYear-1900;
	d.tm_mon  = st.wMonth-1;
	d.tm_mday = st.wDay;
	d.tm_hour = st.wHour;
	d.tm_min  = st.wMinute;
	d.tm_sec  = st.wSecond;
}

inline void TMToSystemTime(const struct tm &d, SYSTEMTIME &st)
{
	st.wYear = (WORD)(d.tm_year+1900);
	st.wMonth = (WORD)(d.tm_mon+1);
	st.wDay = (WORD)d.tm_mday;
	st.wHour = (WORD)d.tm_hour;
	st.wMinute = (WORD)d.tm_min;
	st.wSecond = (WORD)d.tm_sec;
	st.wMilliseconds = 0;
}

// prototype //
struct tm *xgmltime(const timex_t *t, bool local);
timex_t xtimegml(struct tm *d, bool local);

timex_t xtime(timex_t *t)
{
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	timex_t ret = FileTimeToSec(ft) - TIMEDIFF;
	if (t)
		*t = ret;
	return ret;
}

struct tm *xgmtime(timex_t *t)
{
	return xgmltime(t, false);
}

struct tm *xlocaltime(const timex_t *t)
{
	return xgmltime(t, true);
}

struct tm *xgmltime(const timex_t *t, bool local)
{
	timex_t _t;

	FILETIME ft, tmp_ft, *pft;
	SYSTEMTIME st;

	// special function
	if (!t){
		_t = xtime(NULL);
		t = &_t;
	}
	
	SecToFileTime(*t+TIMEDIFF, ft);
	if (local){
		FileTimeToLocalFileTime(&ft, &tmp_ft);
		pft = &tmp_ft;
	} else {
		pft = &ft;
	}
	FileTimeToSystemTime(pft, &st);

	static struct tm d;
	SystemTimeToTM(st, d);
	return &d;
}

timex_t xtimegm(struct tm *d)
{
	return xtimegml(d, false);
}

timex_t xmktime(struct tm *d)
{
	return xtimegml(d, true);
}

timex_t xtimegml(struct tm *d, bool local)
{
	SYSTEMTIME st;
	FILETIME ft, tmp_ft, *pft;

	TMToSystemTime(*d, st);
	if (!SystemTimeToFileTime(&st, &ft)){
		return -1;
	}
	if (local){
		LocalFileTimeToFileTime(&ft, &tmp_ft);
		pft = &tmp_ft;
	} else {
		pft = &ft;
	}

	return FileTimeToSec(*pft) - TIMEDIFF;
}

