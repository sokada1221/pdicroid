#include "tndefs.h"
#pragma hdrstop
#include <time.h>
#include "timex.h"

// prototype //
struct tm *xgmltime(const timex_t *t, bool local);
timex_t xtimegml(struct tm *d, bool local);

timex_t xtime(timex_t *t)
{
	return time(t);
}

struct tm *xgmtime(timex_t *t)
{
	return gmtime(t);
}

struct tm *xlocaltime(const timex_t *t)
{
	return localtime(t);
}

timex_t xtimegm(struct tm *d)
{
	return mktime(d);
}

#if 0
timex_t xmktime(struct tm *d)
{
	return xtimegml(d, true);
}

#endif
