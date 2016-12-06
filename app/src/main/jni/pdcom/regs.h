#ifndef	__REGS_H
#define	__REGS_H

// 正規表現検索用オブジェクトを統一的に扱うためのクラス

#include "pdconfig.h"

#ifdef USE_JRE
#include	"jrectrl.h"
#endif
#ifdef USE_REGEXPP
#include "regexppctrl.h"
#endif
#ifdef USE_BREGONIG
#include "bregonigctrl.h"
#endif

// SetupRegsのwhichで使用
#define	REGS_JRE	0
//#define	REGS_REXP	1
//#define	REGS_VWXW	2

#define	REGTYPE_DEFAULT	(0)

class Regs {
public:
#ifdef USE_JRE
	JREObject *jre;
#endif
#ifdef USE_REGEXPP
	class TRegexpp *regexpp;
#endif
#ifdef USE_BREGONIG
	class TBregonig *bregonig;
#endif
#if 0
protected:
	// paramters for duplication //
	int regType;
	bool useRegular;
	bool useFuzzy;
	bool ignoreCase;
	bool wordAlign;
#endif
public:
	Regs();
	virtual ~Regs();
	void Open( );
	void Close( );
	void Discard( );
	bool IsOpened( );
	void Setup( UINT regtype, bool bUseRegular, bool bFuzzy, bool bIgnore, bool bWord );
	void SetPattern(const tchar *pattern)
		{ GetRexp()->SetPattern(pattern); }
	const tchar *GetPattern()
		{ return GetRexp()->GetPattern(); }
	GenericRexp *GetRexp( );
	Regs *Duplicate();

	// 複数条件
	Regs *next;
	int connection;	// 0:and 1:or
	bool _not;		// 否定
};

bool regb(const tchar *str, const tchar *pat);
tnstr_vec *reg(const tchar *str, const tchar *pat, bool case_ignore=false);

#endif	// __REGS_H

