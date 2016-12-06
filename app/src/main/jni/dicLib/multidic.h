#ifndef	__MULTIDIC_H
#define	__MULTIDIC_H

#if	defined(_WINDOWS) || defined(UNIX)
#include	"windic.h"
#else
#ifdef	MINI
#include	"minidic.h"
#else
#include	"dic4.h"
#endif
#endif


#endif	// __MULTIDIC_H
