#ifndef __multiwd_h
#define	__multiwd_h

#if MIXDIC || defined(KMIXDIC)
#define	USE_MWKCHAR
#define	_mwkstrdef(name)	__kstr name(GetKCodeTrans())
#define	_mwkstr(var)		__kstr(var, GetKCodeTrans())
#define	_mwkstr_t			__kstr
#define	_mwkchar	_kchar
#else
#define	_mwkstrdef(name)	tnstr name
#define	_mwkstr				tnstr
#define	_mwkstr_t			tnstr
#define	_mwkchar			tchar
#endif

class __kstr_vec : public FlexObjectArray<TCharSingle> {
typedef FlexObjectArray<TCharSingle> super;
public:
	__kstr_vec(int arraynum)
		:super(arraynum)
	{
	}
};

class MultiWord {
protected:
#ifdef USE_MWKCHAR
	__kstr_vec warray;
#else
	tnstr_vec warray;
#endif
	FlexObjectArray<Japa> jarray;
	int samect;			//Mode=MD_PARAのとき、同じ単語の残りの数
	int minstr;			//最若／最古単語
//	class MultiPdic &dic;
	int Mode;			// merge mode
public:
	int dirflag;		//0:前方検索 1:後方検索
public:
	MultiWord(int _arraynum);
	~MultiWord();
//	MultiPdic &get_dic() const
//		{ return dic; }
	void SetMode( int mode )
		{ Mode = mode; }
	int GetMode() const
		{ return Mode; }
	void set( int i, const _mwkchar *word, Japa &japa );
	void set( int i, const _mwkchar *word )	// Japaを使用しない場合
		{ warray[i] = word; }
#ifdef USE_SINGLEBYTE
#ifdef USE_MWKCHAR
	int get( _mwkstr_t &word, Japa *japa, int *table, int SingleByte );
#else
	int get( tnstr &word, Japa *japa, int *table, int SingleByte );
#endif
#else
#ifdef USE_MWKCHAR
	int get( _mwkstr_t &word, Japa *japa, int *table, const _mwkchar *str=NULL );
#else
	int get( tnstr &word, Japa *japa, int *table, const _mwkchar *str=NULL );
#endif
#endif
	int get_nearest(const _mwkchar *str, tnstr &word, Japa *japa, int *srchflag);
	int get_fromarray( _mwkstr_t &word, Japa *japa, int *table );
	int get_sameword( int *table, int &minstr, const _mwkchar *str=NULL );
	void clear( int dicnum );
#ifdef USE_MWKCHAR
	int cmpword( const _mwkchar *str1, const _mwkchar *str2 )
		{ return strcmp( str1, str2 ); }
#else
	int cmpword( const tchar *str1, const tchar *str2 )
		{ return _tcscmp( str1, str2 ); }
#endif
};

#endif	/* __multiwd_h */

