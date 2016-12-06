#ifndef	__GREGEXP_H
#define	__GREGEXP_H
#define	RE_KANJI	0x01
#define	RE_VZMETA	0x02
#define	RE_VWMETA	0x04
#define	RE_WFLAG	0x08
#define	RE_IGN		0x10
#define	RE_ZENHAN	0x20
#define	RE_REGEXP	0x40
#define	RE_SCHVWX	0x80
#define	RE_KANA		0x100

class RegExp {
	protected:
		int rexpf;			//検索フラグ
		static int xopt;			//保存用
		static int vopt;			//保存用
		static int fVWX;
	public:
		const char *findptr;		//見つかったときの文字列のポインタ
	public:
		RegExp();
		virtual ~RegExp();
		int Compile( const char *exp );
		int Compare( const char *str );
		int Compare( const char *exp, const char *str );
		void SetFlags( int mode );
		static int GetXOption( void );
		static int GetVOption( void );
		int SetXOption( int v );
		int SetVOption( int v );
		static int CanUse( void );
	protected:
		static int StayCheck( void );
};

#endif	// __GREGEXP_H
