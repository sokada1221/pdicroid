#ifndef	__TNWINLIB_H
#define	__TNWINLIB_H

//	Windows共通便利関数ライブラリ

#ifdef	_WINDOWS

#ifndef __WINS_H
#include	"wins.h"
#endif

#ifndef __TNWINX_H
#include "tnwinx.h"
#endif

#ifndef __TNCOM_H
#include	"tncom.h"
#endif

/*--------------------------------------*/
/*		   WINDOWS用INCLUDEファイル		*/
/*--------------------------------------*/
//#ifndef CBUILDER
#ifndef WINCE
//#include	"profile.h"
#endif
#ifdef WIN32
//#include	"regprof.h"
#endif
#include	"commdlgs.h"

#include	"utf.h"
//#endif

//#include "gdi.h"

#include "tnhtmlhelp.h"
#include "tnhelp.h"

#include "download.h"

#endif	// _WINDOWS

#endif	// __TNWINLIB_H
