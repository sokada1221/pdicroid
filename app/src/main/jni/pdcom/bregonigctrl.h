//---------------------------------------------------------------------------

#ifndef bregonigctrlH
#define bregonigctrlH
//---------------------------------------------------------------------------

#include "bregonig/bregexp.h"

#define	BREGONIG_THREADSAFE		0

// class TBregonig is thread unsafe.
//	検索のinstanceをこのクラス固有の"Handle"に保存しているため。

typedef int (*FNBMatch)(TCHAR *str, TCHAR *target, TCHAR *targetendp, BREGEXP **rxp, TCHAR *msg);
typedef int (*FNBSubst)(TCHAR *str, TCHAR *target, TCHAR *targetendp, BREGEXP **rxp, TCHAR *msg);
typedef void (*FNBRegfree)(BREGEXP *rx);
typedef TCHAR *(*FNBRegexpVersion)(void);

typedef int (*FNBoMatch)(const TCHAR *patternp, const TCHAR *optionp, const TCHAR *strstartp, const TCHAR *targetstartp, const TCHAR *targetendp, BOOL one_shot, BREGEXP **rxp, TCHAR *msg);
typedef int (*FNBoSubst)(const TCHAR *patternp, const TCHAR *substp, const TCHAR *optionp, const TCHAR *strstartp, const TCHAR *targetstartp, const TCHAR *targetendp, BCallBack callback, BREGEXP **rxp, TCHAR *msg);

class TBregonig : public GenericRexp {
protected:
	static HINSTANCE hInst;		// DLL instance handle
	static int Ref;
	static FNBMatch BMatch;
	static FNBSubst BSubst;
	static FNBRegfree BRegfree;
	static FNBRegexpVersion BRegexpVersion;
	static FNBoMatch BoMatch;
	static FNBoSubst BoSubst;
#if 0
	static FNREGCOMPEX regcomp_ex;
	static FNREGMATCH regmatch;
	static FNREGMATCHSTRING regmatch_string;
	//static FNREGREPLACE regreplace;
#endif
	bool Opened;
	BREGEXP *Handle;
	int Start;
	int End;
	int ErrorCode;
public:
	TBregonig();
	~TBregonig();
	virtual BOOL Open( );
	virtual void Close( );
	virtual BOOL CanUse( )
		{ return hInst != NULL; }

protected:
	virtual BOOL CompileImpl( const tchar *patern );
	virtual BOOL CompareImpl( const tchar *str );
	virtual BOOL GetMatchImpl( int &len, int &loc );	// len : マッチ長 loc : マッチ位置
	virtual GenericRexp *Create()
		{ return new TBregonig(); }
public:
	virtual int GetErrorCode( );
	WORD GetVersion( );

	// specific methods //
	// ()対応 compare
	virtual bool CompareEx(const tchar *pattern, const tchar *str, tnstr_vec &strs);
#if BREGONIG_THREADSAFE
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

#endif
