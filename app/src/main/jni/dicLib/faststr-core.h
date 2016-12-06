
#undef FS_WIDE
#if _fsize == 2
#define	FS_WIDE
#endif


struct FS_NAME(FindStr) {
	int n2;
	_fuchar *s2;
	_fuchar *e2;
	uchar skip[MAX_FINDSTR];	// !FS_WIDEÇ≈ÇÕÅApaternÇÕç≈ëÂ256ï∂éöÇ‹Ç≈
#if !defined(FS_WIDE)
	uchar *size;
	uchar *size_e;
	uchar skip_single[MAX_FINDSTR];	// single byteóp
	uchar *s2_single;
#endif
	FS_NAME(FindStr)( const _fchar *patern, bool fIgnoreCase );
	~FS_NAME(FindStr)();
};

_fchar *fstristr( const _fchar *str1, int n1, FS_NAME(FindStr) &fs );
_fchar *fstrstr( const _fchar *str1, int n1, FS_NAME(FindStr) &fs );
_fchar *fstrstr( const _fchar *str1, const _fchar *s2, int n2 );

#if defined(FS_WIDE)
#define	jfstrstr	fstrstr
#define	jfstristr	fstristr
//#define	jfstrchr	_mcschr
//#define	jfstricmp	_mcsicmp
#else
_fchar *jfstrstr( const _fchar *str1, int n1, FS_NAME(FindStr) &fs );
_fchar *jfstrstr( const _fchar *str1, const _fchar *s2, int n2 );
_fchar *jfstristr( const _fchar *str1, int n1, FS_NAME(FindStr) &fs );
_fchar *jfstrchr( const _fchar *s1, _fuchar c );
int jfstricmp( const _fchar *s1, const _fchar *s2 );
#endif

_fchar *jfstrrchr(const _fchar *s, unsigned short c);

#ifndef __DEFINED_JCHARCMP
#define	__DEFINED_JCHARCMP
extern "C" {
int jcharcmp( const _fchar *str1, const _fchar *str2 );	// in faststr1.asm
};
#endif	// __DEFINED_JCHARCMP

