#ifndef	__JMERGE_H
#define	__JMERGE_H

#ifndef __ZEN2HAN_H
#include "zen2han.h"
#endif

/* single byte mode の処理 */
#ifdef USE_SINGLEBYTE
#define	__ARG_fs		, bool fs	/* 関数定義 */
#define	__fs			fs			/* 変数名 */
#define	__PASS_fs(fs)	, fs		/* 関数へ渡す */
#else	// !USE_SINGLEBYTE
#define	__ARG_fs
#define	__fs
#define	__fs_comma
#define	__PASS_fs(fs)
#endif	// !USE_SINGLEBYTE


// 括弧単語の位置と長さ
struct BrLoc {
	int loc;		// 括弧単語の先頭オフセット（括弧を含まない）
	int len;		// 括弧単語の長さ（括弧を含まない）
	BrLoc( int _loc, int _len )
		{ loc=_loc; len=_len; }
	BrLoc(){}
};


struct WordAttr {
	int type;
	ushort delim;
	tchar *word;
	tchar *full;
	int topr;		// 先頭の右側の括弧単語のbrlocのインデックス（これで左側か、右側かを判別）
	void clear( );
	FlexObjectArray<BrLoc> brloc;
	WordAttr();
	~WordAttr();
};

typedef void (*LPFNWORDMERGE)(tchar *outbuf, const tchar *full1, const tchar *full2 );

class WordArray : public FlexObjectArray<WordAttr> {
	private:
		int add( WordAttr *wa )
			{ return FlexObjectArray<WordAttr>::add( wa ); }
	public:
		int Add( WordAttr *wa __ARG_fs );
		int Add2( WordAttr *wa );	// 用例用追加処理
};

// henkan(jmerge)
#define	HK_ZEN2HAN		0x0001	// zen2han
#define	HK_CONVDELIM	0x0010	// ◆●を,へ変換

// henkan(Japa::Merge)
#define	HK_CONVWAVE		0x0200	// 【音声】をファイルリンクに変換
#define	HK_CONVLEVEL	0x0400
#define	HK_CONVPRON		0x0800
#define	HK_REMOVEATMARK	0x1000	// 汎用性があるので、いずれ削除

int jmerge( tchar *dstr, const tchar *sstr, int ljapa, int henkan __ARG_fs );
int expmerge( tchar *dstr, const tchar *sstr, int ljapa, int henkan, int delimtype=DLM_EX );
tchar *isdelm(const tchar *p);
int isnami(ushort c);
int isganonio(ushort c);
void preMerge(tchar *dest, const tchar *src, int henkan);
void preMergeWord(tchar *dest, const tchar *src);
const tchar *wordcpy( WordAttr &wa, const tchar *src __ARG_fs );

extern const tchar STR_DLMEX[3+1];	// 例文用区切り文字（これ以降はマージ対象外）

#endif	// __JMERGE_H

