//---------------------------------------------------------------------------

#include "tnlib.h"
#pragma hdrstop

#include "pdtime.h"
#include "pdstrlib.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef __BORLANDC__
extern "C" {
const void* _getLocaleTimeInfo(LCTYPE info);
int   _getLocaleEra(char* buf, int bufsize, const struct tm *t);
char *             __utoa      (unsigned __value, char *__buf);
}

//#include <_io.h>
//#include <_printf.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <_locale.h>
// BCB‚Ìstrftime()‚Í # ‚ª–¢ŽÀ‘•‚Ì‚½‚ß
size_t strftimex( char *s, size_t maxsize, const char *fmt, const struct tm *t )
{
    char buf[ 255 ];

    char *p = buf;
    char pad;
    size_t  i, len;
	char FormatBuff[128];
	int zpad;


    for ( len = 1; len < maxsize && *fmt; ++fmt, p = buf )
    {

        if ( *fmt != '%' )
        {
            ++len;
            *s++ = *fmt;
        }

        else
        {
			strcpy( buf, "00" );
			zpad = 1;

the_switch:
            switch( *++fmt )
            {

            /* abbreviated day */
            case 'a':
                if (t->tm_wday == 0)
					p = (char *) _getLocaleTimeInfo(LOCALE_SABBREVDAYNAME7);
				else
					p = (char *) _getLocaleTimeInfo(LOCALE_SABBREVDAYNAME1 + t->tm_wday - 1);
				break;

			/* full day */
			case 'A':
				if (t->tm_wday == 0)
					p = (char *) _getLocaleTimeInfo(LOCALE_SDAYNAME7);
				else
					p = (char *) _getLocaleTimeInfo(LOCALE_SDAYNAME1 + t->tm_wday - 1);
				break;

			/* abbreviated month */
			case 'b':
			case 'h':   /* POSIX  */
                p = (char *) _getLocaleTimeInfo(LOCALE_SABBREVMONTHNAME1 + t->tm_mon);
                break;

            /* full month */
            case 'B':
                p = (char *) _getLocaleTimeInfo(LOCALE_SMONTHNAME1 + t->tm_mon);
                break;

            /* date and time */
            case 'c':
                /* set p to current s position */
                p = s;

                /* recurse */
                strftimex( p, maxsize, "%x %X", t );
                break;

            /* month date */
            case 'd':
                __utoa( t->tm_mday, buf + (zpad && t->tm_mday < 10));
                break;

            case 'g':
                strcpy(p, "");
                _getLocaleEra(buf, sizeof(buf), t);
                break;

            /* hour (24) */
            case 'H':
                __utoa( t->tm_hour, buf + (zpad && t->tm_hour < 10));
                break;

            /* hour (12) */
            case 'I':
				i = t->tm_hour % 12;

                if (i == 0)
                    i = 12;

                __utoa( i, buf + (zpad && i < 10) );
                break;

            /* year day */
            case 'j':
                i = t->tm_yday + 1;

                __utoa( i, buf + (i < 10) + (zpad && i < 100) );
                break;

            /* month */
            case 'm':
                i = t->tm_mon + 1;

                __utoa( i, buf + (zpad && i < 10) );
                break;

            /* minute */
            case 'M':
                __utoa( t->tm_min, buf + (zpad && t->tm_min < 10) );
                break;

            /* am/pm */
            case 'p':
                if (t->tm_hour / 12 > 0)
                    p = (char *) _getLocaleTimeInfo(LOCALE_S2359);
                else
                    p = (char *) _getLocaleTimeInfo(LOCALE_S1159);

                break;

            /* seconds */
            case 'S':
                __utoa( t->tm_sec, buf + (zpad && t->tm_sec < 10) );
                break;

            /* week of year (Sunday) */
            case 'U':
                // Adjust weekday to be positive (first week) Sunday
                // based
				i = (t->tm_wday + 7) % 7;

                // Adjust week to be positive (first week)
                i = (t->tm_yday - i + 7) / 7;

                __utoa( i, buf + (zpad && i < 10) );
                break;

            /* day of week */
            case 'w':
                __utoa( t->tm_wday, buf );

                break;

            /* week of year (Monday) */
            case 'W':
                // Adjust weekday to be positive (first week) Monday
                // based
                i = (t->tm_wday + 6) % 7;

                // Adjust week to be positive (first week)
                i = (t->tm_yday - i + 7) / 7;

                __utoa( i, buf + (zpad && i < 10) );
                break;

            /* date */
            case 'x':
            case 'D':       /* POSIX */
                /* set p to current s position */
                p = s;

                /* recurse */
                strftimex( p, maxsize, (char *) _getLocaleTimeInfo(LOCALE_SSHORTDATE), t );

                break;

            /* time */
            case 'X':
                /* set p to current s position */
                p = s;

                /* recurse */
                strftimex( p, maxsize, (char *) _getLocaleTimeInfo(LOCALE_STIMEFORMAT), t );

				break;

            /* year (no century) */
            case 'y':
                i = t->tm_year % 100;

                __utoa( i, buf + (zpad && i < 10) );
                break;

            /* year (with century) */
            case 'Y':
                __utoa( 1900 + t->tm_year, buf );
                break;

            /* century */
            case 'C':       /* POSIX */
                __utoa( 19, buf );
                break;

            case '+' :
                pad = *++fmt;
                buf[0] = pad;
                buf[1] = pad;
                goto the_switch;


           /* time zone */
            case 'z':
            case 'Z':
               p = _tzname[ t->tm_isdst ];
               break;

            /* literally "%" */
            case '%':
                p = "%";
                break;

            case '#' :
            	switch( *++fmt )
                {

                case  'c' :
                    strcpy(FormatBuff, (char *) _getLocaleTimeInfo(LOCALE_SLONGDATE));
                    strcat ( FormatBuff, " %X");
                	strftimex( p, maxsize, FormatBuff, t );
					break;

                case 'x' :
                	strftimex( p, maxsize, (char *) _getLocaleTimeInfo(LOCALE_SLONGDATE), t );
                    break;

				default :
					zpad = 0;
					fmt--;
                	goto the_switch;

                }
                break;
            }

            i = min( strlen( p ), maxsize - len );
            strncpy( s, p, i );
            len += i;
            s += i;

        } /* if-else */

    } /* for */

    *s = '\0';

    if( *fmt )
        return( 0 );
    else
        return( len - 1 );
}
#elif defined(__ANDROID__)
// •s—vH
#else
#error
#endif

#ifndef __ANDROID__
tnstr FormatDate(const tchar *fmt)
{
	timex_t t = xtime(NULL);
	struct tm *d = xlocaltime(&t);

	__tnstrA fmtA; fmtA.setAnsi(fmt);
	char buf[80]; buf[0] = '\0';
	strftimex(buf, sizeof(buf), fmtA, d);
	tnstr ret; ret.setAnsi(buf);
	return ret;
}
#endif


// YYYYMMDDHHMMMSS -> timex_t
// YYYYMMDDHHMMM   -> timex_t
timex_t str2date(const tchar *s)
{
	int len = _tcslen(s);
	if (len<8)
		return 0;
	struct tm t;
	t.tm_year = atoin(s, 4) - 1900;
	t.tm_mon = atoin(s+4, 2) - 1;
	t.tm_mday = atoin(s+6, 2);
	if (len>=8+4){
		t.tm_hour = atoin(s+8, 2);
		t.tm_min = atoin(s+10, 2);
		if (len>=8+6){
			t.tm_sec = atoin(s+12, 2);
		}
	} else {
		t.tm_hour = 0;
		t.tm_min = 0;
		t.tm_sec = 0;
	}
	return xtimegm(&t);
}

// timex_t -> YYYYMMDDHHMMSS
tnstr date2str1(timex_t tt)
{
	if (tt==0)
		return _t("0");
	struct tm *t = xgmtime(&tt);
	return tnsprintf(_t("%04d%02d%02d%02d%02d%02d"),
		t->tm_year+1900,
		t->tm_mon+1,
		t->tm_mday,
		t->tm_hour,
		t->tm_min,
		t->tm_sec
		);
}

// timex_t -> YYYYMMDDHHMM
tnstr date2str2(timex_t tt)
{
	if (tt==0)
		return _t("0");
	struct tm *t = xgmtime(&tt);
	return tnsprintf(_t("%04d%02d%02d%02d%02d"),
		t->tm_year+1900,
		t->tm_mon+1,
		t->tm_mday,
		t->tm_hour,
		t->tm_min
		);
}
