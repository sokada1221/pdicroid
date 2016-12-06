#ifndef __LangProcSimple_h
#define	__LangProcSimple_h

#include "LangProcBase.h"

// 文字コード変換なし(別途.cttファイル不要)

//
// Simple Language Processor
//
class TLangProcSimple : public TLangProcBase {
typedef TLangProcBase super;
public:
	enum { LangProcId = 0x00020000 };
public:
	TLangProcSimple()
		{}
	__override bool Open()
		{ return true; }
	__override tnstr CompositeWord(const tchar *str)
		{ return tnstr(str); }
	__override tnstr Normalize(const tchar *str)
		{ return NULL; }	// no keyword!!
		//{ return _t(" "); }	// NULLにすると cword+"\t"+cwordになってしまうため
	//__override bool OpenIrreg();
	__override bool Compare( struct COMPARE_STRUCT &cs, const int flags )
		{ return false; }	// not supported.
	__override int Search( COMPARE_STRUCT &cs, const tchar *words, tchar *str, MatchArray *HitWords )
		{ return 0; }
protected:
	int FindLoop(COMPARE_STRUCT &cs)
		{ return 0; }	// not supported.
public:
	__override int SearchLongestWord( class MultiPdic *dic, const tchar *words, const tchar *prevwords, int curpos, int option, class MatchArray *HitWords )
		{ return 0; }	// not supported.
};

class TLangProcSimpleOld : public TLangProcSimple {
typedef TLangProcSimple super;
public:
	enum { LangProcId = 0x00020001 };
};

#endif	/* __LangProcSimple_h */

