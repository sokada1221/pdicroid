#ifndef __BiPool_h
#define	__BiPool_h

class TBiPool {
protected:
	tnstr_vec fw;
public:
	TBiPool( int preallocSize );
	virtual ~TBiPool(){}
	int get_num()
		{ return fw.get_num(); }
	virtual int BSearch( const tchar *str );
//	int BSearchExact( const tchar *str );
	int Find( const tchar *str );
	const tchar *word(int index)
		{ return fw[index]; }
};

class TBiPoolJ : public TBiPool {
typedef TBiPool super;
protected:
	tnstr_vec fj;
public:
	TBiPoolJ( int preallocSize );
	void Add( const tchar *word, const tchar *japa );
	void AddSort( const tchar *word, const tchar *japa );
	const tchar *japa(int index)
		{ return fj[index]; }
};

#endif	// __BiPool_h
