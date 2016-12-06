#ifndef __PDDEFS_H
#define	__PDDEFS_H

#include "defs.h"

#define	SQM_JAPA	0x01
#define	SQM_EXP		0x02
#define	SQM_LEVEL	0x04
#define	SQM_PRON	0x08
#define	SQM_WORD	0x10
#define	SQM_TITLE	0x20
#define	SQM_FILENAME 0x40
#define	SQM_COMMON	0x80		// 共通フォントの場合(SQUFONT.CPPで使用)
#define	SQM_EPWING	0x100
#define	SQM_ATTR	0x204		// Memory,Level
#define	SQM_OBJECT	0x400		// PSF_xxxとの互換性のため用意
#define	SQM_DICNAME	0x800
#define	SQM_ALL		0xffff

/* MixDic */
#if defined(UNICODE)
#include "mstr.h"
#include "mustr.h"
// _mstr : Unicode <-> Dictionary Code(ANSI or UTF-8 or BOCU)
#if MIXDIC
#ifdef NEED_SINGLEBYTE
#define	_mstr		__mstr
#else	// !NEED_SINGLEBYTE
#ifdef DIC_BOCU
#define	_mstr(x,s)	(x)
#else
#define	_mstr(x,s)	__mstr(x)
#endif	// !DIC_BOCU
#endif	// !NEED_SINGLEBYTE
#else	// !MIXDIC
#define	_mstr(x,s)	(x)
#endif	// !MIXDIC
// _mustr : Unicode <-> ANSI
#ifdef DIC_UTF16
#define	_mustr(x)	_mstr(x,false)
#else
#define	_mustr(x)	__mustr(x)
#endif
#else	// !UNICODE
#define	_mstr(x,s)	(x)
#define	_mustr(x)	(x)
#endif	// !UNICODE

// KMixDic //
#ifdef KMIXDIC
#define	_kwstr(x,trans)	__kstr(x,trans)		// _kstr for keyword
#define	_kstrn(x)		__kstr(x, &KCodeTranslateSetN)
//#define	_ktstr(x,s)	(x)
#else	// !KMIXDIC
#define	_kwstr			_mstr
#define	_kstrn			_mstr
//#define	_ktstr		_mstr
#endif	// !KMIXDIC

// BOCU1 //
// _kmstr : bocu <=> wchar_t 変換 except keyword
#ifdef DIC_BOCU
#define	_kmstr		_kstrn
#else
#define	_kmstr		_mstr
#endif

// dummy definitions for VCL //
namespace Controls {
class TWinControl;
}

namespace Tntstdctrls {
class TTntEdit;
}

namespace Tntcomctrls {
class TTntRichEdit;
}

#ifndef UNIX
using namespace Controls;
using namespace Tntstdctrls;
using namespace Tntcomctrls;
#endif

#endif	/* __PDDEFS_H */

