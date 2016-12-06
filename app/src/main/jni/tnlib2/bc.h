#ifndef	__BC_H
#define	__BC_H

#if HAVE_CONFIG_H
#include "config.h"
#else
#define	HAVE_MALLOC_H
#endif

#include "tndefs.h"

#ifdef ANSI
//#define	_RTLENTRY
#ifndef __cdecl
#define	__cdecl
#endif
#endif

#if defined(UNIX)
#include <unistd.h>
#endif

#if !defined(UNIX) && !defined(WINCE)
#include	<dos.h>
#include	<conio.h>
#endif

#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#if !defined(WINCE)
#include	<errno.h>
#include	<time.h>
#include	<fcntl.h>
#include	<sys/stat.h>
#if !defined(UNIX) && !defined(__ANDROID__)
#include	<direct.h>
#include	<io.h>
#include	<share.h>
#endif	/* !UNIX */
#endif	/* !WINCE */
#ifdef _MSC_VER
#include	<memory.h>
#else	/* !_MSC_VER */
#if !defined(UNIX) && !defined(__ANDROID__)
#include	<mem.h>
#endif	/* !UNIX */
#endif	/* !UNIX */
#if (defined(__TURBOC__) || defined(_MSC_VER)) && !defined(UNIX) && !defined(WINCE)
  #include	<mbctype.h>
  #include	<mbstring.h>
#endif
#ifdef HAVE_MALLOC_H
#include	<malloc.h>
#endif
#include	<ctype.h>

#ifdef _MSC_VER
#include	"msc.h"
#endif


#if defined(UNIX) && !defined(__ANDROID__)	//TODO: dirent.hでwindef.hが見つからないエラーが出るため外した@android
#include <dirent.h>
#include "unixlib.h"
#endif

#ifdef ANSI
#ifndef _FAR
#define	_FAR
#endif
int strcmpi(const char *s1, const char *s2);
int strnicmp(const char *s1, const char *s2, size_t maxlen);
#endif

#endif	/* __BC_H */
