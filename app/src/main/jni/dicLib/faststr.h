#ifndef	__FASTSTR_H
#define	__FASTSTR_H

#include "dicdef.h"
#include "dicmix.h"

#define	_fsize			_msize
//#define	MAX_FINDSTR	256
#define	MAX_FINDSTR		0x10000
#define	FS_NAME(name)	name
#define	_fchar			_mchar
#define	_fuchar			_muchar
#define	_fcslen			_mcslen
#define	_fcschr			_mcschr
#define	_fnext			_mnext
#define	_flower			_mlower
#define	_fdiff			_mdiff
#define	_flentobyte		_MLENTOBYTE

#define	__NO_STRICMP
#include "faststr-core.h"
#ifdef __INCLUDE_FASTSTR
#include "faststr-core.cpp"
#endif

#ifdef DIC_BOCU
#undef	_fsize
#undef	FS_NAME
#undef	_fchar
#undef	_fuchar
#undef	_fcslen
#undef	_fcschr
#undef	_fnext
#undef	_flower
#undef	_fdiff
#undef	_flentobyte

#define	_fsize			_mtsize
#define	FS_NAME(name)	_mt##name
#define	_fchar			_mtchar
#define	_fuchar			_mtchar
#define	_fcslen			_mtcslen
#define	_fcschr			_mtcschr
#define	_fnext			_mtnext
#define	_flower			_mtlower
#define	_fdiff			_mtdiff
#define	_flentobyte		_MTLENTOBYTE

#include "faststr-core.h"
#ifdef __INCLUDE_FASTSTR
#include "faststr-core.cpp"
#endif

#define	_ktFindStr		_mtFindStr

#else	// !DIC_BOCU
#define	_mtFindStr		FindStr
#define	_ktFindStr		FindStr
#endif	// !DIC_BOCU

#ifdef MIXMJ
#undef	_fsize
#undef	FS_NAME
#undef	_fchar
#undef	_fuchar
#undef	_fcslen
#undef	_fcschr
#undef	_fnext
#undef	_flower
#undef	_fdiff
#undef	_flentobyte

#include "japa.h"
#define	_fsize			_jsize
#define	FS_NAME(name)	_j##name
#define	_fchar			_jchar
#define	_fuchar			_jchar
#define	_fcslen			_jcslen
#define	_fcschr			_jcschr
#define	_fnext			_jnext
#define	_flower			_jlower
#define	_fdiff			_jdiff
#define	_flentobyte		_JLENTOBYTE

#define	__NO_STRICMP
//#define	__NO_STRRCHR
#include "faststr-core.h"
#ifdef __INCLUDE_FASTSTR
#include "faststr-core.cpp"
#endif
#else	// !MIXMJ
#define	_jFindStr		_mtFindStr
#endif	// !MIXMJ

#if MIXDIC
#if 0
inline wchar_t *jfstrchr( const wchar_t *str, unsigned short chr )
	{ return _tcschr( str, chr ); }
wchar_t *jfstrrchr(const wchar_t*s, wchar_t c);
inline int jfstricmp( const wchar_t *s1, const wchar_t *s2 )
	{ return CompareString( GetSystemDefaultLCID(), NORM_IGNORECASE, s1, -1, s2, -1 ) - 2; }
#endif
#ifdef _MSC_VER
int fstrnicmp( const char *s1, const char *s2, int len );
#define	strnicmp	fstrnicmp
int jfstrnicmp( const char *s1, const char *s2, int len );
#endif	// _MSC_VER
#endif	// MIXDIC

// TCHAR‚Æ‚È‚éFindStr‚ðŽw’è‚·‚é
#if defined(DIC_BOCU)
#define	_tFindStr	_mtFindStr
#else
#define	_tFindStr	_jFindStr
#endif

#endif	// __FASTSTR_H



