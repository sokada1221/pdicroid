#ifndef	__WINS_H
#define	__WINS_H

#ifndef __BC_H
#include "bc.h"
#endif

#ifndef __TNDEFS_H
#include	"tndefs.h"
#endif

#ifdef ANSI
#undef _WINDOWS
#endif

#ifdef _Windows
#	ifndef __WINDOWS_H
#		include	<windows.h>
#	endif
#	ifndef	__COMMDLG_H
#		include	<commdlg.h>
#	endif
#	ifndef	__SHELLAPI_H
#		include	<shellapi.h>
#	endif	// __SHELLAPI_H
#	ifndef	__WINDOWSX_H
#		include	<windowsx.h>
#	endif	// __WINDOWSX_H
#endif

#if defined(__TURBOC__)
#if !defined(CBUILDER) && (__TURBOC__ < 0x520)
#ifndef __BWCC_H
#include <bwcc.h>
#endif
#endif
#endif

#endif	// __WINS_H



