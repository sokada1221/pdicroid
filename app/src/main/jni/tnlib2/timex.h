#ifndef __tntime64_h
#define	__tntime64_h

#include <time.h>

#ifdef _Windows
typedef __int64 timex_t;
#else
typedef time_t timex_t;
#endif

timex_t xtime(timex_t *t);

struct tm *xgmtime(timex_t *t);
struct tm *xlocaltime(const timex_t *t);

timex_t xtimegm(struct tm *);
timex_t xmktime(struct tm *t);


#endif

