//---------------------------------------------------------------------------

#ifndef pdtimeH
#define pdtimeH
//---------------------------------------------------------------------------

size_t strftimex( char *s, size_t maxsize, const char *fmt, const struct tm *t );
tnstr FormatDate(const tchar *fmt);
timex_t str2date(const tchar *s);
tnstr date2str1(timex_t tt);	// timex_t -> YYYYMMDDHHMMSS
tnstr date2str2(timex_t tt);	// timex_t -> YYYYMMDDHHMM

#endif
