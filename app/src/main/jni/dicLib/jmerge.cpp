// OLDDIC	O
// NEWDIC	O
// NEWDIC2	O
// DOS		O
// WIN16	O
// WIN32	O
#include	"tnlib.h"
#pragma	hdrstop
#include	"pdcom.h"
#include	"dicdef.h"
#include	"jmerge.h"
#include	"zen2han.h"
#include	"jtype.h"
#include	"faststr.h"
//#include	"debug.h"

#ifdef _UNICODE
#define	_tmbstrstr2(s1,s2,s2len)	_tcsstr(s1,s2)
#else
#define	_tmbstrstr2(s1,s2,s2len)	jfstrstr(s1,s2,s2len)
#endif

// pointerからの文字の取り出し、pointerへの文字の格納はこのmacroを使用する //
#ifdef _UNICODE
#  define	LD_FCHAR(c,p)	LD_CHAR(c, p)
#  define	ST_FCHAR(c,p)	ST_CHAR(c, p)
#ifdef USE_SINGLEBYTE
#  define	__WORD_MERGE(outbuf,word)	_wordmerge(outbuf, word, fs)
#else
#  define	__WORD_MERGE(outbuf,word)	_wordmerge(outbuf, word)
#endif
#else	// !_UNICODE
#ifdef USE_SINGLEBYTE
#  define	LD_FCHAR(c,p)	LD_MCHAR( c, p, fs )
#  define	ST_FCHAR(c,p)	ST_MCHAR( c, p, fs )
#  define	__WORD_MERGE(outbuf,word)	_wordmerge( outbuf, word, fs )
#else
#  define	LD_FCHAR(c,p)	LD_CHAR(c,p)
#  define	ST_FCHAR(c,p)	ST_CHAR(c,p)
#  define	__WORD_MERGE(outbuf,word)	_wordmerge(outbuf,word)
#endif
#endif	// !_UNICODE

const tchar STR_DLMEX[] = _T(" / ");	// 例文用区切り文字（これ以降はマージ対象外）
#define	LEN_STRDLMEX	( tsizeof( STR_DLMEX ) - 1 )
extern const tchar *StrQ;

WordAttr::WordAttr( )
{
	type = 0;
	delim = 0;
	word = 0;
	full = 0;
}

WordAttr::~WordAttr( )
{
	clear();
}

void WordAttr::clear()
{
	type = 0;
	delim = 0;
	if ( word ){
		delete[] word;
		word = 0;
	}
	if ( full ){
		delete[] full;
		full = 0;
	}
	brloc.clear();
}

int jmerge( WordArray &warray, const tchar *sstr __ARG_fs  );
void wordmerge( tchar *outbuf, const tchar *full1, const tchar *full2 __ARG_fs );
tchar *_wordmerge( tchar *outbuf, const tchar *word __ARG_fs );
int assemble( tchar *dstr, WordArray &warray, int maxlen );
int srch_brword( WordAttr &wa1, const tchar *word );

int jmerge( tchar *dstr, const tchar *sstr, int maxlen, int henkan __ARG_fs )
{
//マージの機能：小文字化、区切り文字の統一、同じ単語の削除
//              ()内にある区切り文字は無視
//戻り値：
//		0 正常
//		1 日本語訳が長すぎてマージ出来ない日本語訳があった
//		-1 メモリ不足

	WordArray wa;

#ifdef	DOS
	if ( coreleft() < ( ( _tcslen( dstr ) + _tcslen( sstr ) ) << 2 ) ){
		return -1;	// メモリ不足（こんなチェックでいいのだろうか？）
	}
#endif

	int qflag = 0;	// ?のみの日本語訳
	//sstrが?単語のとき
	if ( (*sstr == '?' && *(sstr+1) == '\0') ||
		 (*dstr == '?' && *(dstr+1) == '\0' ) ){
		qflag = 1;
	}

#ifdef USE_SINGLEBYTE
	if ( fs )
		henkan = 0;
#endif

	if ( henkan & (HK_CONVDELIM|HK_ZEN2HAN)){
		preMerge( dstr, dstr, henkan  );
	}
	jmerge( wa, dstr __PASS_fs(fs) );
	if ( henkan & (HK_CONVDELIM|HK_ZEN2HAN) ){
		preMerge( dstr, sstr, henkan );
		jmerge( wa, dstr __PASS_fs(fs) );
	} else {
		jmerge( wa, sstr __PASS_fs(fs) );
	}

	int r = assemble( dstr, wa, maxlen );
	if ( !*dstr && qflag ){
		_tcscpy( dstr, StrQ );
	}
	return r;
}

// warray += sstr
// preMergeは無し
int jmerge( WordArray &warray, const tchar *sstr __ARG_fs )
{
//マージの機能：同じ単語の削除
//              ()内にある区切り文字は無視
//戻り値：日本語訳が長すぎてマージ出来ない日本語訳があった

	if ( (*sstr == '?' && *(sstr+1) == '\0')
		|| *sstr == '\0' ){
		return 0;
	}

	const tchar *sp = sstr;
	while ( 1 ){

		WordAttr *wa = (WordAttr *)new WordAttr;

		//１単語の切り出し
		if ( ( sp = wordcpy( *wa, sp __PASS_fs(fs) )) == NULL ){
			delete wa;
			break;	// 終わり
		}

		warray.Add( wa __PASS_fs(fs) );

	}
	return 0;
}

int WordArray::Add( WordAttr *wa __ARG_fs )
{
	const tchar *p = wa->word;
		// 括弧内単語の比較を行う最低条件
	int cond = ( ( wa->delim != DLM_EX )
			&& ( !wa->full || ( wa->word && !_tcscmp( wa->word, wa->full ) ) ) );
			// 1994.6.18 wa->wordがNULLになるときがあった。
			//	==>wa->wordのNULLチェックを行っていなかったため、例外発生
			//	原因は、jmerge( WordArray &warray, ...) のwordcpy()呼び出しのときに、
			//	wordcpy()関数の最後の方にある、wa.word = buf;代入時に、
			//	bufがNULLになることがあるため。(existがTRUEのとき）
			//	一応チェックするようにしたが、これが一番効率良いかどうかは解らない
	for ( int i=0;i<get_num();i++ ){
		WordAttr &twa = (*this)[i];
		if ( twa.full && wa->full ){	// 完全に一致していた場合(1997.3.15)
			if ( !_tcscmp( twa.full, wa->full ) ){
				delete wa;
				return 0;
			}
		}
		if ( p && twa.word && !_tcscmp( twa.word, p ) ){		// 1994.6.18 ここでも、p,twa.wordのNULLチェックをしていなかった
			/*----- 同じ単語の場合 -----*/
			// 単語マージを行い、登録済みのWordAttrを書き換える
			// 用例の場合、括弧だけの単語も対象
			const tchar *wp = wa->full;
			if ( !wp ){
				wp = wa->word;
			}
			// 1995.9.14 fullの場合のチェックを追加！
			// これで本当に良いかどうかわからない（wa->fullの場合に習った）
			const tchar *twp = twa.full;
			if ( !twp ){
				twp = twa.word;
			}
			tchar *mergebuf = new tchar[ _tcslen( twp ) + _tcslen( wp ) + 15 + 1 ];	// 15はmargin
			wordmerge( mergebuf, twp, wp __PASS_fs(fs) );
//			mergebuf = (tchar*)::realloc( mergebuf, _tcslen( mergebuf ) + 1 );
			if ( twa.full )
				delete[] twa.full;
			twa.full = mergebuf;
			delete wa;
			return 0;
		} else if ( cond ){
			// 追加する単語に括弧が無く（word==fullで判断）、追加するwordが追加されるfullの括弧内に同じ単語がある場合
			// -> その単語は登録されない
			// 括弧だけの単語も同様
			// 用例は無視
			if ( srch_brword( twa, wa->word ) ){
				delete wa;
				return 0;
			}
		}
	}
	FlexObjectArray<WordAttr>::add( wa );
	return 0;
}

// 用例用マージ(同じ単語があるかどうかだけのチェック)
int WordArray::Add2( WordAttr *wa )
{
	const tchar *p = wa->word;
	if ( !p ){
		delete wa;
		return 0;
	}
	for ( int i=0;i<get_num();i++ ){
		WordAttr &twa = (*this)[i];
		if ( p && twa.word && !_tcscmp( twa.word, p ) ){
			/*----- 同じ単語の場合 -----*/
			delete wa;
			return 0;
		}
	}
	FlexObjectArray<WordAttr>::add( wa );
	return 0;
}

// 返り値：0 正常
//         1 maxlenを越えた
int assemble( tchar *dstr, WordArray &warray, int maxlen )
{
	*dstr = '\0';
	int tlen = 0;
	for ( int j=0;j<2;j++ ){
		// j=0 : 通常の単語
		// j=1 : 用例
		for ( int i=0;i<warray.get_num();i++ ){
			ushort delim = warray[i].delim;
			if ( j == 0 ){
				if ( delim == DLM_EX )
					continue;
			} else {
				if ( delim != DLM_EX )
					continue;
			}
			if ( !i ){
				if ( !j )
					delim = 0;
			} else {
				if ( !delim ){
					delim = ',';
				}
			}
			const tchar *word = warray[i].full;
			if ( !word || word[0] == '\0' ){
				word = warray[i].word;
			}
			int len = _tcslen( word );
			// デリミタの長さを求める
			int dlen = 0;
			if ( delim > 0 ){
				if ( delim == DLM_EX ){
					if ( tlen )	// 1996.9.16
						dlen = LEN_STRDLMEX;
#ifndef _UNICODE
				} else if ( delim >= 0x100 ){
					dlen = 2;
#endif
				} else {
					dlen = 1;
				}
			}
			if ( len + tlen + dlen > maxlen ){
				return 1;
			}
			tchar *p = dstr + tlen;
			if ( delim > 0 ){
				if ( delim == DLM_EX ){
					if ( tlen ){	// 1996.9.16
						_tcscpy( p, STR_DLMEX );
						p += dlen;
					}
#ifndef _UNICODE
				} else if ( delim >= 0x100 ){
					ST_CHAR(delim, p);
#endif
				} else {
					*p++ = delim;
				}
			}
			_tcscpy( p, word );
			tlen += dlen + len;
		}
	}
	return 0;
}



void preMerge(tchar *dest, const tchar *src, int henkan)
{
//		区切り文字の統一、短縮
//		全角ー＞半角変換
	unsigned short c;
	int del = 0;		//1の時に通常の文字がきたときは、デリミタを挿入

	for(;;) {
		LD_CHAR(c, src);
		if ( !((tuchar)c) )
			break;
		if ( henkan & HK_ZEN2HAN ){
			if (is_nami(c)){
				c = CODE_NAMI1;		// ～
			} else
			if (isdelim(c)){
				del = 1;
				continue;
			}
			if (del){
				*dest++ = ',';
				del = 0;
			}
			c = zen2han(c);
		}
		if ( henkan & HK_CONVDELIM ){
			if ( c == CODE_BLACK_DIAMOND || c == CODE_BLACK_CIRCLE ){
				// ◆●の場合
				c = ',';
			}
		}
		ST_CHAR(c, dest);
	}
	*dest = '\0';
}

// 見出語のためのpreMerge(現在のところFENG V5形式からの変換のみ対応)
void preMergeWord(tchar *dest, const tchar *src)
{
//		区切り文字の統一、短縮
//		全角ー＞半角変換
	unsigned short c;
	int del = 0;		//1の時に通常の文字がきたときは、デリミタを挿入

	for(;;) {
		LD_CHAR(c, src);
		if ( !((tuchar)c) )
			break;
		if (is_nami(c)){
			c = '~';		// ここだけが違う！！
		} else if (isdelim(c)){
			del = 1;
			continue;
		}
		if (del){
			*dest++ = ',';
			del = 0;
		}
		c = zen2han(c);
		ST_CHAR(c, dest);
	}
	*dest = '\0';
}

// 返り値：NULL 終了
//		   NULL以外：次のポインタ
//
// 「用例" / "」、「括弧だけの単語」は、wordへ、fullはNULL
const tchar *wordcpy( WordAttr &wa, const tchar *src __ARG_fs )
{
loop:
	wa.clear();

	if ( !*src )
		return NULL;

	// 半角スペースを読み飛ばす
	while ( *src == ' ' ){
		if ( !_tcsncmp( src, STR_DLMEX, LEN_STRDLMEX ) ){
			// 用例である場合
			wa.delim = DLM_EX;
			wa.word = newstr( src + LEN_STRDLMEX );
			while ( *src ) src++;
			return src;
		}
		src++;
	}

	if ( !*src )
		return NULL;

	int srclen = _tcslen(src)+1;
	tchar *buf = new tchar[ srclen ];
	tchar *dp = buf;
	ushort c;
	int kn = 0;
	int flag = 0;
	int exist = 0;	// 括弧だけの単語のために

	const tchar *sp = src;

	wa.delim = DLM_NONE;
	wa.type = 0;

	int loc;
	int topr = 0;
	while ( 1 ){
		const tchar *_sp = sp;
		LD_FCHAR(c, sp);
		if ( !((tuchar)c) ){
			sp--;
			break;
		}
		if ( !flag ){
			if ( isdelim2( c ) ){
				wa.delim = c;	// デリミタ保存
#if 0
				while ( *sp == ' ' )	// 半角スペースは読み飛ばす
					sp++;
#endif
				src = sp;		// 先頭を移動
				continue;
			} else if ( is_nami(c) ){
				if ( !*sp ){
					return NULL;
				}
				continue;
			}
		}
		flag = 1;
		if (islbr(c)){
			if ( !kn ){
				loc = STR_DIFF( sp, src );	// 先頭左括弧の位置をマーク
			}
			exist++; kn++; continue;
		}
		if ( kn ){
			if ( isrbr( c ) ){
				kn--;
				if ( !kn ){
					wa.brloc.add( (BrLoc *)new BrLoc( loc, STR_DIFF( _sp, src ) - loc ) );	// 括弧単語の位置・長さを登録
				}
				continue;
			}
		} else {
			if (isdelim2(c) ){
				sp = _sp;
				break;
			}
			if ( c == ' ' ){
				if ( !_tcsncmp( sp-1, STR_DLMEX, LEN_STRDLMEX ) ){
					sp = _sp;
					break;
				}
			}
			ST_FCHAR(c, dp);
			topr = wa.brloc.get_num();
		}
	}
	*dp = '\0';
	wa.topr = topr;

	if ( buf[0] == '\0' ){
		delete[] buf;
		buf = 0;
		if ( !exist ){
			if ( *src ){
				goto loop;
			} else {
				return NULL;
			}
		}
	} else {
		// 無駄な領域を削除するため
//		buf = (tchar*)::_realloc( buf, _tcslen( buf ) + 1 );
		int l = _tcslen(buf)+1;
		if ( l + 16 < srclen ){
			tchar *buf2 = new tchar[ l ];
			memcpy( buf2, buf, LENTOBYTE(l) );
			delete[] buf;
			buf = buf2;
		}
	}
	wa.word = buf;

	// 括弧を含めた単語のコピー
	wa.full = newstr( src, STR_DIFF( sp, src ) );

	return sp;
}



// 括弧を含めた単語のマージ
void wordmerge( tchar *outbuf, const tchar *full1, const tchar *full2 __ARG_fs )
{
	const tchar *p;
	ushort us;

	outbuf[0] = '\0';
	tchar *outp = outbuf;
	const tchar *pw1 = full1;
	if ( pw1 ){
		while ( 1 ){
			p = pw1;
			LD_FCHAR( us, p );
			if ( !((tuchar)us) ){		// S-JIS 2バイト目NULL対策 : 欠点S-JIS1バイト目が落ちる！！
				p--;
				break;
			}
			if ( !islbr( us ) && !isrbr( us ) ){
				while ( 1 ){
					p = pw1;
					LD_FCHAR( us, pw1 );
					if ( !((tuchar)us) ){
						pw1--;
						break;
					}
					if ( islbr( us ) ){
						pw1 = p;
						break;
					}
				}
				break;
			}
			pw1 = __WORD_MERGE( outbuf, pw1 );
		}
	}

	const tchar *pw2 = full2;
	while ( 1 ){
		p = pw2;
		LD_FCHAR( us, p );
		if ( !((tuchar)us) ){
			p--;
			break;
		}
		if ( !islbr( us ) && !isrbr( us ) ){
			outp = outbuf + _tcslen( outbuf );
			while ( 1 ){
				p = pw2;
				LD_FCHAR( us, pw2 );
				if ( !((tuchar)us) ){
					pw2--;
					break;
				}
				if ( islbr( us ) ){
					pw2 = p;
					break;
				}
				ST_FCHAR( us, outp );
			}
			*outp = '\0';
			break;
		}
		pw2 = __WORD_MERGE( outbuf, pw2 );
	}

#if 0
	outp = outbuf + _tcslen( outbuf );

	int l1 = _tcslen( word1 );
	int l2 = _tcslen( word2 );
	if ( l1 >= l2 ){
		_tcscpy( outp, word1 );
		outp += l1;
	} else {
		_tcscpy( outp, word2 );
		outp += l2;
	}
#endif

	if ( pw1 ){
		while ( *pw1 ){
			pw1 = __WORD_MERGE( outp, pw1 );
		}
	}

	while ( *pw2 ){
		pw2 = __WORD_MERGE( outp, pw2 );
	}
}
// wordから括弧単語を抜き出し、outbufにあれば無視、無ければ追加する
// 1996.12.4 : wordの先頭にいきなり右括弧があるとbuf[0]='\0'となり、
//             jfstrstrがそれに対応していなくなったため、jfstrstr内で無限ループ
// 返り値：word の次の項へ
tchar *_wordmerge( tchar *outbuf, const tchar *word __ARG_fs )
{
	tchar *buf = new tchar[ _tcslen( word ) + 1 ];
	tchar *p = buf;
	const tchar *sp = word;
	ushort us;
	int kn = 0;

	// 括弧単語の切り出し
	while ( 1 ){
		LD_FCHAR( us, sp );
		if ( !((tuchar)us) ){
			sp--;
			break;
		}
		if ( isrbr( us ) ){
			if ( --kn <= 0 )
				break;
		} else if ( islbr( us ) ){
			if ( ++kn <= 1 )
				continue;
		}
		ST_FCHAR( us, p );
	}
	*p = '\0';
	if ( buf[0] && !_tmbstrstr2( outbuf, buf, _tcslen( buf ) ) )	// 一寸問題あり？
	{
		// 見つからない場合追加
		sp = word;
		p = outbuf + _tcslen( outbuf );
		kn = 0;
		while ( 1 ){
			LD_FCHAR( us, sp );
			if ( !((tuchar)us) ){
				sp--;
				break;
			}
			ST_FCHAR( us, p );
			if ( isrbr( us ) ){
				if ( --kn <= 0 )
					break;
			} else if ( islbr( us ) ){
				kn++;
			}
		}
		*p = '\0';
	}
	delete[] buf;
	return (tchar*)sp;
}

// wordが、wa1内の括弧単語にあるかどうか調べる
int srch_brword( WordAttr &wa1, const tchar *word )
{
	int l = _tcslen( word );
	for ( int i=0;i<wa1.brloc.get_num();i++ ){
		if ( !_tcsncmp( wa1.full + wa1.brloc[i].loc, word, l ) ){
			return 1;
		}
	}
	return 0;
}

// 用例マージ /////////////////////////////////////
// シングルバイト文字のみ対応(区切り文字しか見ないので、用例の場合は大丈夫)

int expmerge( WordArray &warray, const tchar *sstr, int delimtype  );

int expmerge( tchar *dstr, const tchar *sstr, int maxlen, int henkan, int delimtype )
{
//マージの機能：小文字化、区切り文字の統一、同じ単語の削除
//              ()内にある区切り文字は無視
//戻り値：
//		0 正常
//		1 日本語訳が長すぎてマージ出来ない日本語訳があった
//		-1 メモリ不足

	WordArray wa;

#ifdef	DOS
	if ( coreleft() < ( ( _tcslen( dstr ) + _tcslen( sstr ) ) << 2 ) ){
		return -1;	// メモリ不足（こんなチェックでいいのだろうか？）
	}
#endif

	if ( henkan ){
		preMerge( dstr, dstr, henkan );
	}
	expmerge( wa, dstr, delimtype );
	if ( henkan ){
		preMerge( dstr, sstr, henkan );
		expmerge( wa, dstr, delimtype );
	} else {
		expmerge( wa, sstr, delimtype );
	}

	return assemble( dstr, wa, maxlen );
}

const tchar *expwordcpy( WordAttr &wa, const tchar *src, int delimtype )
{
	wa.clear();

	const tchar *sp = src;

	const tchar *delim = (delimtype == DLM_EX ? STR_DLMEX : _T(","));
	int delimlen = _tcslen( delim );

	while ( *sp ){
		if ( _tcsncmp( sp, delim, delimlen ) == 0 ){
			// 区切り文字
			wa.delim = (tuchar)delimtype;
			wa.word = newstr( src, STR_DIFF( sp, src ) );
			return sp + delimlen;
		}
		sp++;
	}
	if ( STR_DIFF( sp, src ) ){
		wa.delim = (tuchar)delimtype;
		wa.word = newstr( src );
		return sp;
	} else {
		return NULL;
	}
}

//マージの機能：同じ単語(" / "で区切られた)の削除
//戻り値：日本語訳が長すぎてマージ出来ない日本語訳があった
int expmerge( WordArray &warray, const tchar *sstr, int delimtype  )
{
	if ( *sstr == '\0' ){
		return 0;
	}

	const tchar *sp = sstr;
	while ( 1 ){

		WordAttr *wa = new WordAttr;

		//１単語の切り出し
		if ( ( sp = expwordcpy( *wa, sp, delimtype )) == NULL ){
			delete wa;
			break;	// 終わり
		}

		warray.Add2( wa );

	}
	return 0;
}

