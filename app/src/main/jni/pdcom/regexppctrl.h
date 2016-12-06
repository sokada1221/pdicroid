#ifndef __REGEXPPCTRL_H
#define	__REGEXPPCTRL_H

#include "../regexppdll/regexpp.h"

#define	REGEXPP_THREADSAFE	0	// thread safe for hInst/Ref/... static variables.
//Note: 2012.9.19
//	排他制御が本当に必要なのかどうかわからないが、
//	static変数の排他制御を入れることで、関連語検索のAND検索でエラーがほとんど出なくなる
//	(100%ではないのでまだ原因があるかも？)
//	DLL APIもLock/Unlockしているが本当に必要なのかどうか不明。(DLL側要調査)
//	あっても実害は少ないと思うので。（検索速度落ちる？）

class TRegexpp : public GenericRexp {
protected:
	static HINSTANCE hInst;		// JREインスタンスハンドル
	static int Ref;
	static FNREGCOMP regcomp;
	static FNREGCOMPEX regcomp_ex;
	static FNREGEXEC regexec;
	static FNREGERROR regerror;
	static FNREGFREE regfree;
	static FNREGVERSION regversion;
	static FNREGMATCH regmatch;
	static FNREGMATCHSTRING regmatch_string;
	//static FNREGREPLACE regreplace;
	bool Opened;
	int Handle;	// regexpp handle
	int Start;
	int End;
	int ErrorCode;
public:
	TRegexpp();
	~TRegexpp();
	virtual BOOL Open( );
	virtual void Close( );
	virtual BOOL CanUse( )
		{ return hInst != NULL; }

protected:
	virtual BOOL CompileImpl( const tchar *patern );
	virtual BOOL CompareImpl( const tchar *str );
	virtual BOOL GetMatchImpl( int &len, int &loc );	// len : マッチ長 loc : マッチ位置
	virtual GenericRexp *Create()
		{ return new TRegexpp(); }
public:
	virtual int GetErrorCode( );
	// specific methods //
	// ()対応 compare
	virtual bool CompareEx(const tchar *pattern, const tchar *str, tnstr_vec &strs);	// thread safeかも?

#if 0
	// handle operation //
	virtual int GetHandle() const { return Handle; }
	virtual void SetHandle(int h){ Handle = h; }
	virtual void CloseHandle(int h);
	virtual int CompileEx(const tchar *pattern);
	virtual bool CompareEx(int h, const tchar *str, tnstr_vec &strs);
#endif

	WORD GetVersion( );

#if REGEXPP_THREADSAFE
	CRITICAL_SECTION mutex;
	void Lock()
		{ EnterCriticalSection(&mutex); }
	void Unlock()
		{ LeaveCriticalSection(&mutex); }
#else
	inline void Lock(){}
	inline void Unlock(){}
#endif
};

#endif	// __REGEXPPCTRL_H

