#ifndef __TNASSERT_H
#define	__TNASSERT_H

#include "tndefs.h"

#ifdef _MSC_VER
#define	__FUNC__	""
#endif

#if defined(__ANDROID__) && !defined(__BORLANDC__)
#define	__assert__	__assert_message(_t("%hs:%hs:%d"), __FILE__, __FUNCTION__, __LINE__)
#else
#define	__assert__	__assert_message(_t("%hs:%hs:%d"), __FILE__, __FUNC__, __LINE__)
#endif

#if defined(_DEBUG) || defined(DEBUG)
#define	__assert(x)		do { if(!(x)){ __assert__; } } while (0)
#define	__assert_debug	__assert__
#else
#define	__assert(x)
#define	__assert_debug
#endif

#define	__assert_release(x)	do { if(!(x)){ __assert__; } } while (0)

void __assert_message(const tchar *format, ...);

#if 0	// old version
#define	__assert(x) \
	if (!(x)){ __assertion_error(__FILE__, __LINE__); }

void __assertion_error(const char *file, int line);
#endif

#endif	/* __TNASSERT_H */
