#ifndef __PDCLASS_h
#define	__PDCLASS_h


#if defined(__BORLANDC__) && !defined(UNIX)
#include <vcl.h>
#ifndef __VCL
#define	__VCL
#endif
#endif	// __BORLANDC__

#if defined(__ANDROID__)
#if defined(__BORLANDC__)
#include <System.hpp>
#else
#define	PACKAGE
#endif
#endif

#include "tnlib.h"

#if defined(__BORLANDC__) && defined(GUI)
#include "vclcom.h"
#endif

#include "pdconfig.h"
#include "pddefs.h"

#ifdef WINCE
#include "pdicce\wincealt.h"
#endif

#ifdef __ANDROID__
#include "defs_android.h"
#endif

#endif

