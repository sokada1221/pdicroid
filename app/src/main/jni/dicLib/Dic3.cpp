/*======================================================================*/
/* クラス名称　：マルチ辞書アクセス										*/
/* バージョン　：Ver.1.00          										*/
/* ファイル名　：DIC3.CPP												*/
/* 作成日　　　：1993/11/19												*/
/* 作成者　　　：Copyright 1993 TaN										*/
/* 更新　　　　：														*/
/*======================================================================*/

// NEWDIC専用バージョン

#include	"tnlib.h"
#pragma	hdrstop
#include "pdconfig.h"
#include "LangProc.h"
#include "LangProcMan.h"
#include	"dic.h"
#include	"pdtypes.h"
#include	"dic3.h"
#include	"filestr.h"
#include "pdfilestr.h"
#include "perllib.h"
#include "jtype.h"

////////////////////////////////////////////////////////////////////////////
// TMultiAllSearchParam class
////////////////////////////////////////////////////////////////////////////
TMultiAllSearchParam::TMultiAllSearchParam()
	:mw(1)	// 1 is dummy
{
	num = 0;
	memset(Params, 0, sizeof(Params));
	lastdicAS = -1;
	KCodeTrans = &KCodeTranslateSetN;
	multiThread = false;
}

TMultiAllSearchParam::~TMultiAllSearchParam()
{
	AllClose();
}

void TMultiAllSearchParam::Open(MultiPdic &dic)
{
	num = dic.GetDicNum();
	FillParams();
	mw.clear(num);
	KCodeTrans = dic.GetKCodeTrans();
}

void TMultiAllSearchParam::AllClose()
{
	for (int i=0;i<MAX_MULTIDIC;i++){
		if (Params[i]){
			delete Params[i];
			Params[i] = NULL;
		}
	}
	num = 0;
	mw.clear(0);
	KCodeTrans = &KCodeTranslateSetN;
}

void TMultiAllSearchParam::FillParams()
{
	for (int i=0;i<num;i++){
		if (!Params[i]){
			Params[i] = new AllSearchParam;
		}
	}
}

void TMultiAllSearchParam::SetupSearch(bool dirflag)
{
	for (int i=0;i<num;i++){
		srchflag[i] = 1;
	}
	mw.clear(num);
	mw.dirflag = dirflag;
}

void TMultiAllSearchParam::ResetSearch()
{
	mw.dirflag = 0;
}

void TMultiAllSearchParam::Stop(bool wait)
{
	for (int i=0;i<num;i++){
		Params[i]->Stop(wait);
	}
}

#if !defined(SMALL)
// MD_MERGE_MASKに関するフラグのみセットする
int TMultiAllSearchParam::SetMergeMode( int _mode )
{
	int oldmode = mw.GetMode();
	mw.SetMode( (mw.GetMode() & ~MD_MERGE_MASK) | _mode );
	return oldmode;
}
#endif	// !SMALL

int TMultiAllSearchParam::GetFoundWord(tnstr &word, Japa *japa)
{
	_mwkstrdef(_word);
	int ret = lastdicAS = mw.get( _word, japa, srchflag );
	word = _word;
	return ret;
}

int TMultiAllSearchParam::GetNearestWord(const tchar *str, tnstr &word, Japa *japa)
{
	_mwkstrdef(_word);
	int ret = lastdicAS = mw.get( _word, japa, srchflag, _mwkstr(str) );
	word = _word;
	return ret;
}

////////////////////////////////////////////////////////////////////////////
// MultiPdic class
////////////////////////////////////////////////////////////////////////////
MultiPdic::MultiPdic()
	:sdic( MAX_MULTIDIC )
{
	errcode = 0;		// マルチ辞書関連のエラー番号
	errdic = 0;

#if defined(USE_DICORDER)
	order = SK_NORMAL;
#endif
#ifdef USE_SINGLEBYTE
	SingleByte = 0;
#endif
//	Flags = 0;

	LangProc = NULL;
}

MultiPdic::~MultiPdic()
{
	AllClose();
	if (LangProc)
		LangProc->Free();
}

// open/close ///////////////////////////////////////////////////
#if !defined(USE_UNIVDIC)
int MultiPdic::Open( int dn, const tchar *name, const tchar *indexname, const tchar *gconvname, const tchar *flinkpath, int openflag, const tchar *nettempname )
{
	if ( dn == -1 ){
		dn = sdic.get_num();
	}
	TUniversalDic *pdic = _open( name, indexname, gconvname, flinkpath, dn, openflag, nettempname );
	if ( !pdic ){
		return -1;
	}
	if (!LangProc){
		//TODO: 最適なlang.procを探すlogic
		LangProc = pdic->GetLangProc();
		LangProc->IncRef();
		KCodeTrans = LangProc->GetKCodeTrans();
	}
	sdic.insert( dn, pdic );
#if defined(USE_DICORDER)
	SetOrder( sdic[0].getHeader()->dicorder );
#endif
	SrchParam.Open(*this);
	return 0;
}

// Should be called after Open() successfully, and delete the returned object after Close().
TMultiAllSearchParam *MultiPdic::CreateSearchParam()
{
	if (sdic.get_num()==0)
		return NULL;
	TMultiAllSearchParam *param = new TMultiAllSearchParam();
	param->Open(*this);
	param->SetMode(SrchParam.GetMode());
	return param;
}

TUniversalDic *MultiPdic::_open( const tchar *pathname, const tchar *epwname, const tchar *gconvname, const tchar *flinkpath, int dicno, int openflag, const tchar *nettempname )
{
	//超過チェック
	errdic = -3;
	if ( sdic.get_num() >= MAX_MULTIDIC ){
		errcode = 10;
		return NULL;
	}

	errdic = dicno;
	Pdic *dic = CreatePdic( pathname, epwname, gconvname, flinkpath, openflag, nettempname, &errcode );
	if ( !dic ){
		if ( errcode == DICERR_OPEN_CREATE && !(openflag & DICFLAG_READONLY)){
			dic = new_open( pathname, dicno );
		}
		if ( dic == NULL )
			return NULL;
	}

#if 0
	//バージョンチェック
	if ( sdic.get_num() >= 1 ){
		if ( sdic[0].getVersion() != dic->getVersion() ){
			errcode = DICERR_DIFFVERSION;
			goto error;
		}
	}
#endif

#if 0
	// ソートの種類の異なる辞書のチェック
	if ( sdic.get_num() >= 1 ){
		if ( sdic[0].GetOrder( ) != dic->GetOrder( ) ){
			errcode = DICERR_INCOMPTYPE;
			goto error;
		}
	}
#endif

	// incompatible lang proc. //
	if ( sdic.get_num() >= 1 ){
		if ( !TLangProcMan::CompareId(sdic[0].GetLangProcId( ), dic->GetLangProcId( ) ) ){
#ifdef __PROTO
			if (dic->GetLangProcId()==0x00000000){
				// old type lang proc. is ok.
			} else
#endif
			{
				errcode = DICERR_INCOMPTYPE;
				goto error;
			}
		}
	}

	{
		tnstr irg = ChangeFileExt(pathname, _t("irg"));	// IrregDic file
		if (fexist(irg)){
			TLangProc *lp = dic->GetLangProc();
			lp->AddIrregFile(irg);
		}
	}
	
	return dic;
error:
	dic->Close();
	dic->Release( );
	return NULL;
}
#endif	// !USE_UNIVDIC

#ifdef MACCS
bool MultiPdic::CanClose( )
{
	for ( int i=0;i<GetDicNum();i++ ){
		if ( !sdic[i].CanClose() )
			return false;
	}
	return true;
}
#endif

void MultiPdic::AllClose()
{
	SrchParam.AllClose();
	while ( sdic.get_num() ){
		Close( 0 );
	}
	CloseIrregDic();
}
int MultiPdic::ODAClose()
{
	int n = 0;
	for (int i=0;i<GetDicNum();i++){
		if (sdic[i].ODAClose(true))
			n++;
	}
	return n;
}
//TODO: 辞書フラグで指定できるといい？
void MultiPdic::ODAReopen(int flags)
{
	for (int i=0;i<GetDicNum();i++){
		if (flags & (1<<i)){
			sdic[i].ODAReopen();
		}
	}
}
int MultiPdic::Close( int dn )
{
	sdic[dn].Close();
	sdic[dn].Release( );
	sdic.discard( dn );
	if (sdic.get_num()==0){
		if (LangProc){
			LangProc->Free();
			LangProc = NULL;
			KCodeTrans = NULL;
		}
	}
	return 0;
}

bool MultiPdic::ThreadUp()
{
	for (int i=0;i<GetDicNum();i++){
		if (!sdic[i].ThreadUp()){
			for (i--;i>=0;i--){
				sdic[i].ThreadDown();
			}
			return false;
		}
	}
	return true;
}
void MultiPdic::ThreadDown()
{
	for (int i=0;i<GetDicNum();i++){
		sdic[i].ThreadDown();
	}
}


// モード設定処理 ///////////////////////////////////////////////////////////

#if USE_DICFASTMODE
void MultiPdic::SetFastMode( bool bFast )
{
	for ( int i=0;i<sdic.get_num();i++ ){
		sdic[i].SetFastMode( bFast );
	}
}
#endif

// 情報取得処理 /////////////////////////////////////////////////////////////
int MultiPdic::GetErrorCode( )
{
	int dn = GetErrorDicNo();
	if ( dn < sdic.get_num() ){
		int r = sdic[GetErrorDicNo()].GetErrorCode();
		if ( r )
			return r;
	}
	return errcode;
}

int MultiPdic::SetMergeMode( int _mode )	//返り値は前のmode
{
	return SrchParam.SetMergeMode(_mode);
}

// 辞書番号指定 /////////////////////////////////////////////////////////////////

// all can be NULL, when it is, Pdic object uses its all param.
int MultiPdic::bsearch( const tchar * word, int dn, AllSearchParam *all )
{
	errdic = dn;
	return sdic[dn].BSearch( _mwkstr(word), false, all );
}

//指定辞書にwordがあるかどうかのチェック
//返り値：0:無し 1:あり -1:エラー
// 2009.6.6 wordがcompositかどうかで処理を分けていたが、compositに統一
// なぜ今まで分けていたのか不明（non compositの場合、bsearchでヒットするはず無いのだが？)
int MultiPdic::dsearch( const tchar * word, Japa *japa, int dn )
{
	tnstr cword = create_composit_word(word);

//	errdic = dn;
	int r = bsearch( cword, dn, SrchParam[dn] );
	if (r<=0){
		return r;
	}
	r = strequ( cword, _mwkstr(SrchParam[dn]->GetFoundWord()) );
	if (r){
		if ( japa ){
			sdic[dn].GetFoundJapa( *japa, SrchParam[dn] );
		}
		return 1;
	} else {
		return 0;
	}
}

#if !defined(USE_UNIVDIC)
#ifdef PDICW
//wordがある最初の辞書番号を返す
//返り値＝-1 エラー又は見つからない
// flags : 検索対象辞書条件(DICF_...)
int MultiPdic::dn_search( const tchar * word, int flags )
{
	for ( int i=0;i<sdic.get_num();i++ ){
		if ( !sdic[i].CheckCond( flags ) )
			continue;
		switch ( dsearch( word, NULL, i ) ){
			case -1:
//				errcode = sdic[i].getError();
				errdic = i;
				return -1;
			case 1:
				return i;
		}
	}
//	errcode = 0;
	return -1;
}
#endif

#if 0
#if defined(PDIC32)
//wordがある最初の辞書番号を返す
//返り値＝-1 エラー又は見つからない
int MultiPdic::dn_search( const tchar * word )
{
	for ( int i=0;i<sdic.get_num();i++ ){
		switch ( dsearch( word, NULL, i ) ){
			case -1:
//				errcode = sdic[i].getError();
				errdic = i;
				return -1;
			case 1:
				return i;
		}
	}
//	errcode = 0;
	return -1;
}
#endif
#endif	// 0

// diclistの中から一致する単語１つでもあれば1を返す
int MultiPdic::Find( const tchar * word, Japa *japa, diclist_t diclist )
{
	tnstr temp = create_composit_word(word);
#if MIXDIC || defined(KMIXDIC)
	_kwstrdef(sword,temp);
#else
	const _kchar *sword = temp;
#endif
	for ( int i=0;i<sdic.get_num();i++ ){
		if (!dl_check(diclist, i)) continue;
		if ( sdic[i].Find( sword, japa, SrchParam[i] ) > 0 )
			return 1;
	}
	return 0;
}
//TODO: 最適な検索方法を考えなければいけない
// strを検索
// -1 : error
// 0  : 見つからない
// 1  : 部分一致だが、見つかった単語の一致部分の次がスペース以外
// 2  : 部分一致(strが見つかった単語に含まれる)
// 3  : 完全一致(keyword部で比較するため、本当の完全一致ではない)
int MultiPdic::FindPart( const tchar *str, diclist_t diclist )
{
	if (!str[0])
		return 0;	// 2008.6.18

	const tchar *_str;
	tnstr str_temp;
	if (is_composit_word(str)){
		_str = str;
	} else {
		str_temp = create_kword(str);
		_str = str_temp;
	}
#if MIXDIC || defined(KMIXDIC)
	_kwstrdef(__str,_str);
#else
	const _kchar *__str = _str;
#endif
	int ret = 0;
	int len = _kcslen(__str);
	for ( int i=0;i<sdic.get_num();i++ ){
		if (!dl_check(diclist, i)) continue;
		int r = sdic[i].BSearch( __str, false, SrchParam[i] );
		if ( r == -1 ){
			errdic = i;
			return -1;
		} else
		if ( r == 1 ){
			const _kchar *p = SrchParam[i]->GetFoundWord();
			if (!_kcsncmp(__str, p, len)){
				_kchar c = p[len];
				if (!c || c=='\t')	//TODO: 2006.11.8 とりあえずkeyだけが一致すればOKにした
					return 3;	// 完全一致
				if (c==BOCU_SPACE || c==BOCU_COMMA || c==BOCU_SHARP)	// 2010.10.9 BOCU_SHARPを追加
					ret = 2;
				else
					if ( ret == 0 ) ret = 1;
			}
		}
	}
	return ret;
}
#endif	// !defined(USE_UNIVDIC)

#if 0
//	strに最も近い単語を探す
//→2008.1.3 正確にはstrに最も近い単語をそれぞれの辞書から探し、見つかった単語の中で一番前の単語を返す
//	なので、使い物にならないはず？
//TODO: 廃止する
//	word		: 検索文字列／検索ヒット文字列
//	japa		: 見つかった日本語訳
//	nDicNo		: 検索対象辞書（フラグ式）
//	返り値：重複単語数(マージモード)
int MultiPdic::Search( const tchar *str, tnstr &word, Japa *japa, diclist_t nDicList)
{
//	DBW("MultiPdic::Search %s %d", str, nDicList );
	SrchParam.SetupSearch(0);	// 後方検索時にこのmethodを呼ぶと期待しない結果となる
	int ndic = sdic.get_num();
#if MIXDIC || defined(KMIXDIC)
	_kstrdef(sword,str,_wSingleByte);
#else
	const _kchar *sword = str;
#endif
	Japa fjp;
	for ( int i=0;i<ndic;i++ ){
		if ( !dl_check(nDicList, i) ){
			SrchParam.srchflag[i] = -1;
			continue;
		}
		int r = sdic[i].BSearch( sword, false, SrchParam[i] );
		if ( r == -1 ){
			errdic = i;
			return -1;	// エラー
		} else if ( r == 1 ){
			if (japa){
				sdic[i].getfjapa(fjp, SrchParam[i]);
				SrchParam.mw.set( i, SrchParam[i]->GetFoundWord(), fjp);
			} else {
				SrchParam.mw.set( i, SrchParam[i]->GetFoundWord() );
			}
			SrchParam.srchflag[i] = 0;
		} else {
			SrchParam.srchflag[i] = 1;
		}
	}
	return SrchParam.GetFoundWord(word, japa);
}
#endif

//	strに最も近い単語を探す
//	word		: 検索文字列／検索ヒット文字列
//	japa		: 見つかった日本語訳
//	nDicNo		: 検索対象辞書（フラグ式）
//	返り値：重複単語数(マージモード)
int MultiPdic::Search2( const tchar *str, tnstr &word, Japa *japa, diclist_t nDicList)
{
//	DBW("MultiPdic::Search %s %d", str, nDicList );
	int ndic = sdic.get_num();
	SrchParam.mw.clear(ndic);	// 後方検索時にこのmethodを呼ぶと期待しない結果となる
#if MIXDIC || defined(KMIXDIC)
	_kwstrdef(sword,str);
#else
	const _kchar *sword = str;
#endif
	Japa fjp;
	for ( int i=0;i<ndic;i++ ){
		if ( !dl_check(nDicList, i) ){
			SrchParam.srchflag[i] = -1;
			continue;
		}
		int r = sdic[i].BSearch( sword, false, SrchParam[i] );
		if ( r == -1 ){
			errdic = i;
			return -1;	// エラー
		} else
		if ( r == 1 ){
			if (japa){
				sdic[i].getfjapa(fjp, SrchParam[i]);
				SrchParam.mw.set( i, SrchParam[i]->GetFoundWord(), fjp);
			} else {
				SrchParam.mw.set( i, SrchParam[i]->GetFoundWord() );
			}
			SrchParam.srchflag[i] = 0;
		} else {
			SrchParam.srchflag[i] = 1;
		}
	}
	return SrchParam.GetNearestWord(str, word, japa);
}

// ポップアップ検索用
// 見出語部の区切り文字以降は無視して探す
bool MultiPdic::SearchPart( const tchar *str, tnstr_vec &words, diclist_t nDicList )
{
#if 1
	if (EnumKeyWords(str, words, nDicList, true)){
		return true;
	}
	return false;
#else	//TODO: To be deleted
#if MIXDIC || defined(KMIXDIC)
	_kstrdef(__str,str,_wSingleByte);
#else
	const _kchar *__str = str;
#endif
	int len = _kcslen(__str);
	for ( int i=0;i<sdic.get_num();i++ ){
		if ( !dl_check(nDicList, i) ){
			continue;
		}
		sdic[i].SetAllSearch( __str, (SrchMode)(SRCH_HEAD|SRCH_WORD), NULL, SrchParam[i] );
		while ( 1 ){
			tchar c;
			_KChar _word;
			switch ( sdic[i].NextAllSearch( _word, NULL, SrchParam[i] ) ){
				case AS_FOUND:
					c = _word[len];
					if (c!='\0'){
						if (IsWordChar(c)){
							if (_istalpha( c )&&(c!='Z'&&c!='z'))
							{
#if 0
								// 一気に文字の終わりまでskipする
								// ただしcが'Z'or'z'の場合はしない
								_mchar *s = new _mchar[len+2];
								memcpy(s,_word,_MLENTOBYTE(len));
								s[len] = 'Z'|(c&0x20);	// A-Z->Z a-z=>z
								s[len+1] = '\0';
								sdic[i].jumpAS(s,false,false,SrchParam[i]);
								delete[] s;
#endif
							}
						} else
						if (c != ' ' ){
							// c!='\0' && !IsWordChar(c) && c!=' '
							// 一致！！
							word = _kstr(_word,_wSingleByte);
							return true;
						}
					}
					continue;
				case AS_CONTINUE:
					continue;
			}
			break;
		}
	}
	return false;
#endif
}
// keyに一致するkeyまたはwordを探し、その完全なwordを返す
// 大量にAS_FOUNDになる場合は途中で止める
#define	MaxFoundCount	500		// これ以上はabort
#define	MinMatchCount	10		// ただし、これ以上見つかった場合は最後まで続ける
bool MultiPdic::EnumKeyWords(const tchar *key, tnstr_vec &words, diclist_t nDicList, bool searchpart)
{
#if MIXDIC || defined(KMIXDIC)
	_kwstrdef(__str,key);
#else
	const _kchar *__str = str;
#endif

	int total_match_count = 0;
	tstring_map dupcheck;
	int len = _tcslen(key);
	for ( int i=0;i<sdic.get_num();i++ ){
		if ( !dl_check(nDicList, i) ){
			continue;
		}
		// 2011.12.27 found_count, match_countは全辞書トータルではなく、辞書毎に変えた（正しい？）
		// そうしないと、ある辞書でfound_count>MaxFoundCount && match_count==0 となり、
		// それ以降の辞書で match_count>0 となるようなケースでは、完全一致単語がないという結果に
		// なってしまう。
		int found_count = 0;
		int match_count = 0;
		sdic[i].SetAllSearch( __str, (SrchMode)(SRCH_HEAD|SRCH_WORD), NULL, SrchParam[i] );
		while ( 1 ){
			tchar c;
			tnstr _word;
			switch ( sdic[i].NextAllSearch_( _word, NULL, SrchParam[i] ) ){
				case AS_FOUND:
					found_count++;
					if (found_count>MaxFoundCount && match_count<MinMatchCount){
						//DBW("Abort : %d %d", found_count, match_count);
						break;	// abort search
					}
					c = _word[len];
					if (c=='\0' || c=='\t' || c=='#'){
						if (searchpart && c!='#') continue;
						if (dupcheck.count(_word.c_str())==0){
							words.add(_mwkstr(_word));
							dupcheck[_word.c_str()] = _t("");
							match_count++;
						}
					} else {
						// 不一致
					}
					continue;
				case AS_CONTINUE:
					continue;
			}
			break;
		}
		total_match_count += match_count;
	}
	return total_match_count>0;
}

// strはcomposit word
int MultiPdic::Read( const tchar *str, Japa *japa, diclist_t nDicList )
{
	__assert(is_composit_word(str));
	tnstr _word;
	int r = Search2( str, _word, japa, nDicList );
	if ( r <= 0 ) return r;
	return !_tcscmp( str, _word );
}

int MultiPdic::record( const tchar * word, const Japa &japa, int dn )
{
	errdic = dn;
	return sdic[dn].Record( word, japa );
}

int MultiPdic::update( const tchar * word, const Japa &japa, int dn )
{
//	errdic = dn;
	if (bsearch( word, dn, NULL ) <= 0){	// 登録なしの場合もエラー扱いにした 2007.11.5
		return -1;
	}
	return sdic[dn].Update( word, japa );
}

int MultiPdic::erase( const tchar * word, int dn )
{
	errdic = dn;
	return sdic[dn].Erase( word );
}

#if !defined(USE_UNIVDIC)
int MultiPdic::all_erase( const tchar * word )
{
	for ( int i=0;i<sdic.get_num();i++ ){
		switch ( dsearch( word, NULL, i ) ){
			case -1:
//				errcode = sdic[i].getError();
				errdic = i;
				return -1;
			case 1:
				sdic[i].erase( word );
				break;
		}
	}
	return 0;
}
#endif

int MultiPdic::rename( const tchar *oldword, const tchar *newword, int dn )
{
	errdic = dn;
	//return sdic[dn]._Rename( _mwkstr(oldword), _mwkstr(newword), 0 );
	return sdic[dn].rename(oldword, newword);
}

#if !defined(USE_UNIVDIC)
wa_t MultiPdic::get_attr( const tchar *word )
{
	wa_t attr = 0;
	for ( int i=0;i<sdic.get_num();i++ ){
		attr = Japa::MergeAttr( attr, sdic[i].get_attr( word ) );
	}
	return attr;
}

//返り値：セットできた単語数
int MultiPdic::set_all_attr( const tchar *word, int bit )
{
	int s = 0;
	for ( int i=0;i<sdic.get_num();i++ ){
		int r = sdic[i].set_attr( word, bit );
		if ( r > 0 )
			s += r;
	}
	return s;
}

//返り値：リセットできた単語数
int MultiPdic::reset_all_attr( const tchar *word, int bit )
{
	int s = 0;
	for ( int i=0;i<sdic.get_num();i++ ){
		int r = sdic[i].reset_attr( word, bit );
		if ( r > 0 ){
			s += r;
		}
	}
	return s;
}

//返り値：セットできた単語数
int MultiPdic::change_attr( const tchar *word, uchar attr, uchar mask, diclist_t dicflags )
{
	int s = 0;
	for ( int i=0;i<sdic.get_num();i++ ){
		if (!dl_check(dicflags, i)){
			continue;
		}
		int r = sdic[i].change_attr( word, attr, mask );
		if ( r > 0 ){
			s += r;
		}
	}
	return s;
}
#endif	// !USE_UNIVDIC

// 辞書へ変更ができない場合、ユーザー辞書へ？単語として登録
// addattr:
//	ユーザー辞書へ登録する場合、追加したい属性 (現状、JEDITとMEMORYのみ）
// return:
//	-2 - ユーザー辞書へ登録できない、登録できる条件ではない
//	-1 - error
//	0  - ユーザー辞書へ新規登録した
//	1以上 - 属性を変更できた
int MultiPdic::ChangeAttrEx(const tchar *word, uchar bit, bool f, wa_t addattr)
{
	int r;
	wa_t attr;
	if ( f ){
		r = set_all_attr0( word, bit );
		attr = addattr | bit;
	} else {
		r = reset_all_attr0( word, bit );
		attr = addattr & ~bit;
	}
	if (r){
		return r;	// success or error
	}
	if (sdic.get_num()<2){
		return -2;
	}

	if ( bit == WA_MEMORY && f){
		// 変更できなかった場合
		if ( CHECK_NETWORK((*this)) GetDicNumCond( DICF_READONLY ) ){
			// ユーザ辞書へコピー
			if ( (*this)[0].Find( word ) != 1 ){	// ユーザ辞書には未登録であること
				Japa japa;
				japa.japa.set( StrQ );
				japa.SetAttr(attr & ~(WA_EX|WA_LEVELMASK));
				(*this)[0].record( _mwkstr(word), japa );
				return 0;
			}
		}
		return -2;
	}
	return -2;
}

#if !defined(USE_UNIVDIC)
// wordが削除対象であるかどうかチェックし、該当した場合は削除
// -2 : 非該当
int MultiPdic::CheckEraseUser( const tchar * word )
{
#if MIXDIC || defined(KMIXDIC)
	_kwstrdef( __word, word );
#else
	#define	__word	word
#endif
	if ( sdic[0]._Find( (const _kchar*)__word ) != 1 ){
		return -2;
	}
	Japa japa;
	sdic[0].getfjapa( japa );
	if ( japa.IsQWord() && !japa.IsMemory() ){
		for ( int i=1;i<sdic.get_num();i++ ){
			if ( sdic[i].IsReadOnly() && sdic[i]._Find( (const _kchar*)__word ) == 1 ){
				Japa fj;
				sdic[i].getfjapa( fj );
				if ( (fj.GetAttr() & ~(WA_EX|WA_LEVELMASK|WA_NORMAL)) == (japa.GetAttr() & ~(WA_LEVELMASK|WA_NORMAL)) ){
					// 発音記号、用例などが登録されていないことはattrでわかる！！
					// ユーザ辞書から削除！！
					return sdic[0].erase( word );
				}
			}
		}
	}
	return -2;
}
#endif	// !USE_UNIVDIC

// for network
int MultiPdic::update0( const tchar * word, const Japa &japa, int dn )
{
//	errdic = dn;
	if (bsearch( word, dn, NULL ) <= 0){	// 登録なしの場合もエラー扱いにした 2007.11.5
		return -1;
	}
	if ( CHECK_NETWORK2 (dn == 0) && japa.IsQWord() ){
		int r = CheckEraseUser( word );
		if ( r != -2 )
			return r;
	}
	return sdic[dn].Update( word, japa );
}

#if !defined(USE_UNIVDIC)
// for network
int MultiPdic::set_all_attr0( const tchar *word, int bit )
{
	int s = set_all_attr( word, bit );
	if ( CHECK_NETWORK2 (s > 0) ){
		CheckEraseUser( word );
	}
	return s;
}

//返り値：リセットできた単語数
int MultiPdic::reset_all_attr0( const tchar *word, int bit )
{
	int s = reset_all_attr( word, bit );
	if ( CHECK_NETWORK2 (s > 0) ){
		CheckEraseUser( word );
	}
	return s;
}

int MultiPdic::change_all_attr0( const tchar *word, uchar attr, uchar mask )
{
	int s = change_attr( word, attr, mask );
	if ( CHECK_NETWORK2 (s > 0) ){
		CheckEraseUser( word );
	}
	return s;
}

int MultiPdic::set_attr0( const tchar *word, int bit, int dn )
{
	int r = sdic[dn].set_attr( word, bit );
	if ( CHECK_NETWORK2 (r == 1) )
		CheckEraseUser( word );
	return r;
}

int MultiPdic::reset_attr0( const tchar *word, int bit, int dn )
{
	int r = sdic[dn].reset_attr( word, bit );
	if ( CHECK_NETWORK2 (r == 1) )
		CheckEraseUser( word );
	return r;
}

int MultiPdic::add_level0( int dn, const tchar *word, int level, int minlevel, int maxlevel )
{
	int r = sdic[dn].add_level( word, level, minlevel, maxlevel );
	if ( CHECK_NETWORK2 (r == 1) )
		CheckEraseUser( word );
	return r;
}
#endif

//	nsearchをnextASに融合バージョン
void MultiPdic::initNsearch( const tchar * word, TMultiAllSearchParam *all )
{
	setAS( word, (SrchMode)(SRCH_HEAD | SRCH_WORD), NULL, all );
}

#if NEWINCSRCH
// incremental search用で頭出しをするが、
// 後方検索も途中から可能である
void MultiPdic::initNsearch2( const tchar * word, TMultiAllSearchParam *all )
{
	setAS( word, (SrchMode)(SRCH_ALL|SRCH_FAST), NULL, all );
//	jumpAS( word, false, true, all );	// 前方
}
// wordより前の単語は存在するか？
bool MultiPdic::CheckPrevWord( const tchar *word, tnstr *fword )
{
	setAS( word, SRCH_ALL, NULL );
	jumpAS( word, true, false );
	Japa japa;
	tnstr _word;
	for(;;){
		//TODO: japaは要らない - 効率が悪い
		switch ( MultiPdic::prevAS( _word, &japa, -1 ) ){	// 全辞書から
			case AS_CONTINUE:
				continue;
			case AS_FOUND:
				if ( fword ){
					fword->set( _word );
				}
				return true;
		}
		break;
	}
	return false;
}
// wordより後ろの単語は存在するか？
bool MultiPdic::CheckNextWord( const tchar *word, tnstr *fword )
{
	setAS( word, SRCH_ALL, NULL );
	jumpAS( word, false, false );
	Japa japa;
	tnstr _word;
	for(;;){
		//*++ japaは要らない - 効率が悪い
		switch ( MultiPdic::nextAS( _word, &japa, -1 ) ){	// 全辞書から
			case AS_CONTINUE:
				continue;
			case AS_FOUND:
				if ( fword ){
					fword->set( _word );
				}
				return true;
		}
		break;
	}
	return false;
}
#endif

#if 0
//返り値：-1: 終了
//        -2: ディスクエラー
//         0: 以上(mode=MD_PARAのとき：辞書番号 mode=MD_MERGEのとき：重複単語数
int MultiPdic::nsearch( tnstr &fword, Japa &fjapa, int dicno )
{
	if ( dicno != -1 ){
		while ( 1 ){
			switch ( sdic[dicno].nextAllSearch( fword, &fjapa ) ){
				case AS_END:
					return -1;
				case AS_FOUND:
#if 0
					if ( cmpnword( fword, sdic[dicno].GetAllWord(), sdic[dicno].GetAllLen() ) > 0 ){
						return -1;
					}
#endif
					return 1;
				case AS_ERROR:
					return -2;
			}
		}
	}

	fjapa.clear();
	tnstr wd;
	Japa jp;

	if ( multiword.Mode & MD_PARA ){
		int r = multiword.get_fromarray( fword, fjapa, srchflag );
		if ( r != -1 ){
			return r;
		}
	}
	int eofflag = 0;
	for ( int i=0;i<sdic.get_num();i++ ){
		if ( srchflag[i] == 1 ){
			while ( 1 ){
				switch ( sdic[i].nextAllSearch( wd, &jp ) ){
					case AS_FOUND:
#if 0
						if ( cmpnword( wd, sdic[i].GetAllWord(), sdic[i].GetAllLen() ) > 0 ){
							srchflag[i] = -1;	// end
							break;
						}
#endif
						multiword.set( i, wd, jp );
						srchflag[i] = 0;
						break;
					case AS_ERROR:	//ディスクエラー
	//					errcode = sdic[i].getError();
						errdic = i;
						return -2;
					case AS_END:		//該当単語これ以上無し
						srchflag[i] = -1;
						break;
					case AS_CONTINUE:
						continue;
				}
				break;
			}
		}
		if ( srchflag[i] == -1 ){
			eofflag++;
		}
	}
	if ( eofflag == sdic.get_num() ){
		return -1;
	}
	return multiword.get( fword, fjapa, srchflag );
}
#endif

// マルチ辞書検索処理 ///////////////////////////////////////////////////////

int MultiPdic::setAS( const tchar * word, SrchMode mode, GENERICREXP *regp, TMultiAllSearchParam *_all )
{
	TMultiAllSearchParam &all = _all ? *_all : SrchParam;
//	DBW("MultiPdic::setAS %s %d", word, mode );
	for ( int i=0;i<sdic.get_num();i++ ){
		GENERICREXP *nreg;
#ifdef USE_REGEXP
		if (regp && all.multiThread){
			nreg = regp->Duplicate();
			if (!nreg){
				return -1;
			}
			all[i]->re = nreg;
			all[i]->re_owner = true;
			nreg = NULL;
		} else {
			nreg = regp;
		}
#else
		nreg = NULL;
#endif
		sdic[i].SetAllSearch( word, mode, nreg, all[i] );
	}
	all.SetupSearch();
	return 0;
}

#if 0
void MultiPdic::SetDirection( bool bForward )
{
	SrchParam.multiword->dirflag = bForward;
}
#endif
#if 0
void MultiPdic::resetAS( )
{
	SrchParam.ResetSearch();
}
#endif

// AS_CONTINUEがないバージョン
int MultiPdic::nextAS2( tnstr &word, Japa *japa, diclist_t diclist )
{
	int r;
	while ( 1 ){
		r = nextAS( word, japa, diclist );
		if ( r != AS_CONTINUE )
			break;
	}
	return r;
}

int MultiPdic::nextAS( tnstr &word, Japa *japa, diclist_t diclist, TMultiAllSearchParam *_all )
{
	TMultiAllSearchParam &all = _all ? *_all : SrchParam;

	if (all.multiThread){
		return nextASMT(word, japa, diclist, &all);
	}

	if (japa) japa->clear();

	_KChar wd;
	Japa jp;

	int eofflag = 0;
	for ( int i=0;i<sdic.get_num();i++ ){
		if ( !dl_check(diclist, i) ){
			all.srchflag[i] = -1;
			eofflag++;
			continue;
		}
		if ( all.srchflag[i] == 1 ){
			switch ( sdic[i].NextAllSearch_k( wd, &jp, all[i] ) ){
				case AS_CONTINUE:
					return AS_CONTINUE;
				case AS_FOUND:
					all.mw.set( i, wd, jp );
					all.srchflag[i] = 0;
					break;
				case AS_ERROR:
					errdic = i;
//					errcode = sdic[i].getError();
					return AS_ERROR;
				case AS_END:
					all.srchflag[i] = -1;
					break;
			}
		}
		if ( all.srchflag[i] == -1 ){
			eofflag++;
		}
	}
	if ( eofflag == sdic.get_num() ){
		return AS_END;
	}
	all.GetFoundWord(word, japa);
	return AS_FOUND;
}

int MultiPdic::nextASMT( tnstr &word, Japa *japa, diclist_t diclist, TMultiAllSearchParam *_all )
{
	return nextASMT(word, japa, diclist, _all, true);
}

int MultiPdic::nextASMT( tnstr &word, Japa *japa, diclist_t diclist, TMultiAllSearchParam *_all, bool forward )
{
	TMultiAllSearchParam &all = _all ? *_all : SrchParam;

	if (japa) japa->clear();

	_KChar wd;
	Japa jp;

	int eofflag = 0;
	//bool found = false;
	for ( int i=0;i<sdic.get_num();i++ ){
		if ( !dl_check(diclist, i) ){
			all.srchflag[i] = -1;
			eofflag++;
			continue;
		}
		if ( all.srchflag[i] == 1 ){
			int ret = sdic[i].CommonAllSearchMT( wd, &jp, all[i], forward );
			switch (ret){
				case AS_RUNNING:
					break;
				case AS_CONTINUE:
					//return AS_CONTINUE;
					//WaitForSingleObject(all[i]->thread_sem, 0);
					sdic[i].SearchMTNext();
					break;
				case AS_FOUND:
					//DBW("AS_FOUND:%d", i);
					all.mw.set( i, wd, jp );
					//all.srchflag[i] = 0;
					//WaitForSingleObject(all[i]->thread_sem, 0);
					//sdic[i].SearchMTNext();
					//found = true;
					goto jfound;
				case AS_ERROR:
					errdic = i;
//					errcode = sdic[i].getError();
					return AS_ERROR;	//TODO: stop処理が必要
				case AS_END:
					//DBW("AS_END:%d", i);
					all.srchflag[i] = -1;
					break;
			}
		}
		if ( all.srchflag[i] == -1 ){
			eofflag++;
		}
	}
	if ( eofflag == sdic.get_num() ){
		return AS_END;
	}

//	if (!found)
		return AS_CONTINUE;

jfound:;	
#if 0
	const int timeout = 100;
	if (WaitForSingleObject(all.thread_sem, timeout)!=WAIT_OBJECT_0){
		return AS_CONTINUE;
	}
#endif
	//all.GetFoundWord(word, japa);
	_mwkstrdef(_word);
	_word.set(wd);
	word = _word;
	if (japa)
		Read(word, japa, diclist);
	return AS_FOUND;
}

int MultiPdic::prevAS( tnstr &word, Japa *japa, diclist_t diclist, TMultiAllSearchParam *_all )
{
	TMultiAllSearchParam &all = _all ? *_all : SrchParam;

	if (all.multiThread){
		return prevASMT(word, japa, diclist, &all);
	}

	if (japa) japa->clear();

	_KChar wd;
	Japa jp;

	int eofflag = 0;
	for ( int i=0;i<sdic.get_num();i++ ){
		if ( !dl_check(diclist, i) ){
			all.srchflag[i] = -1;
			eofflag++;
			continue;
		}
		if ( all.srchflag[i] == 1 ){
			switch ( sdic[i].PrevAllSearch_k( wd, &jp, all[i] ) ){
				case AS_CONTINUE:
					return AS_CONTINUE;
				case AS_FOUND:
					all.mw.set( i, wd, jp );
					all.srchflag[i] = 0;
					break;
				case AS_ERROR:
					errdic = i;
//					errcode = sdic[i].getError();
					return AS_ERROR;
				case AS_END:
					all.srchflag[i] = -1;
					break;
			}
		}
		if ( all.srchflag[i] == -1 ){
			eofflag++;
		}
	}
	if ( eofflag == sdic.get_num() ){
		return AS_END;
	}
	all.GetFoundWord(word, japa);
	return AS_FOUND;
}

int MultiPdic::prevASMT( tnstr &word, Japa *japa, diclist_t diclist, TMultiAllSearchParam *all )
{
	return nextASMT(word, japa, diclist, all, false);
}

#if USE_MARK
void MultiPdic::markAS( MultiMarkInfo& mmark )
{
	mmark.dirflag = multiword->dirflag;
	for ( int i=0;i<sdic.get_num();i++ ){
		sdic[i].markAS( mmark.markinfo[i] );
		mmark.srchflag[i] = srchflag[i];
	}
}

int MultiPdic::jumpAS( MultiMarkInfo &mmark )
{
	for ( int i=0;i<sdic.get_num();i++ ){
		if ( sdic[i].jumpAS( mmark.markinfo[i] ) == -1 ){
			return -1;
		}
	}
//	multiword->dirflag = mmark.dirflag;
	multiword->clear();
	for ( i=0;i<sdic.get_num();i++ ){
		if ( multiword->dirflag == mmark.dirflag ){
			srchflag[i] = mmark.srchflag[i];
		} else {
			srchflag[i] = 1;
		}
		tnstr word;
		Japa japa;

		sdic[i].curAllSearch( word, japa, multiword->dirflag );
		multiword->set( i, word, japa );
	}
	return 0;
}
#else	// !USE_MARK
	// マークを使わない再検索方法のしかた
	// jumpAS()では一番最後に見つかった位置にlocationを置くように処理する。
	// つまり、すべての単語はすでに読みだし済みとする。srchflag = 1
	// ※欠点：ヒットの確率の低い辞書場合、再び空読みが始まってしまう。
	//   >> この関数の呼出側で最適化している場合は問題ないが・・・。
int MultiPdic::jumpAS( const tchar *word, bool fBack, bool fSameBack, TMultiAllSearchParam *_all )
{
//	DBW("MultiPdic::jumpAS %s %d %d", word, fBack, fSameBack );
	TMultiAllSearchParam &all = _all ? *_all : SrchParam;
	for ( int i=0;i<GetDicNum();i++ ){
		if ( sdic[i].JumpAS_( word, fBack, fSameBack, all[i] ) == -1 ){
			return -1;
		}
	}
	all.SetupSearch(fBack);
	return 0;
}
#endif

// LangProc depend search //
int MultiPdic::SearchLongestWord( const tchar *words, const tchar *prevwords, int curpos, int option, class MatchArray *HitWords )
{
	return LangProc->SearchLongestWord(this, words, prevwords, curpos, option, HitWords);
}

int MultiPdic::SearchLongestWordExt(const tchar *words, const tchar *prevwords, class MatchArray *HitWords, int &thread_key, FNLPSLWExtCallback callback, uint_ptr user)
{
	return LangProc->SearchLongestWordExt(this, words, prevwords, HitWords, thread_key, callback, user);
}

int MultiPdic::SearchLongestWordCmd(int cmd, int thread_key)
{
	return LangProc->SearchLongestWordExtCmd(cmd, thread_key);
}



#ifdef PDICW
#ifdef USE_OLE
void MultiPdic::FreeObjects( )
{
	for ( int i=0;i<GetDicNum();i++ ){
		sdic[i].FreeObjects( );
	}
}
#endif	// USE_OLE

#if defined(USE_JLINK)
#if !defined(USE_UNIVDIC)
bool MultiPdic::SetLinkAttr( int flag, bool f )
{
	bool r = false;
	for ( int i=0;i<GetDicNum();i++ ){
		r = sdic[i].SetLinkAttr( flag, f );
	}
	return r;
}
#endif	// !USE_UNIVDIC
#endif	// USE_JLINK

#if !defined(USE_UNIVDIC)
// flagsの条件に一致する辞書の数を得る
int MultiPdic::GetDicNumCond( int flags )
{
	int n = 0;
	for ( int i=0;i<GetDicNum();i++ ){
		n += sdic[i].CheckCond( flags );
	}
	return n;
}

// flagsの条件に一致する辞書番号をフラグ化したものをかえす
// すべての辞書である場合は-1を返す！
int MultiPdic::GetDicCond( int flags )
{
	int n = -1;
	for ( int i=0;i<GetDicNum();i++ ){
		if ( !sdic[i].CheckCond( flags ) ){
			n &= ~(1<<i);
		}
	}
	return n;
}
#ifndef SMALL
// flagsの条件にOR一致する最初の辞書番号を返す
// もしない場合は-1を返す
int MultiPdic::GetFirstCond( int flags )
{
	for ( int i=0;i<GetDicNum();i++ ){
		if ( sdic[i].CheckCond( flags ) )
			return i;
	}
	return -1;
}
#endif
#endif	// !USE_UNIVDIC
#if defined(USE_JLINK)
int MultiPdic::SearchDicName( const tchar *name )
{
	for ( int i=0;i<GetDicNum();i++ ){
		if ( IsSameFile( GetDicName( i ), name ) ){
			return i;
		}
	}
	return -1;	// 一致する辞書が見つからない
}
#endif
#endif // PDICW

// 不規則変化形辞書 //

bool MultiPdic::OpenIrregDic()
{
	return LangProc->OpenIrreg();
}

void MultiPdic::CloseIrregDic()
{
	if (!LangProc)
		return;
	LangProc->CloseIrreg();
}

// 検索処理
// SearchIrregular()を行う前にOpenIrregular()を呼び出すこと！！
bool MultiPdic::SearchIrreg( const tchar *word, tnstr &trsword )
{
	__assert(LangProc!=NULL);
	__assert(LangProc->IsIrregOpened());
	return LangProc->SearchIrreg(word, trsword);
}

#if !defined(USE_UNIVDIC)
#ifdef USE_JLINK
#include "assexec.h"
// filenameが存在するパスを全ての辞書のパスから探す
// 存在するパスの辞書番号を返す
// commonpath can be null.
// CommandPath/Currentは無し
int MultiPdic::SearchFileLinkPath( const tchar *filename, const tchar *commonpath, tnstr &fullpath )
{
#if 0
	if (IsFullPath(filename)){
		// 完全パスの場合
		return -1;
	}
#endif
	tnstr path;
	for ( int i=0;i<GetDicNum();i++ ){
		tnstr dicpath = _GetFilePath( sdic[i].GetFullFileName() );
		tnstr dicflink = sdic[i].GetFileLinkPath();
		if (FindFileLinkPath(fullpath, filename, dicpath, dicflink, commonpath, false)){
			return i;
		}
	}
	return -1;
}
#endif	// USE_JLINK
#endif	// !USE_UNIVDIC

#ifdef USE_UNIVDIC

int MultiPdic::Bind( TUniversalDic *dic, int dn )
{
	//超過チェック
	errdic = -3;
	if ( sdic.get_num() >= MAX_MULTIDIC ){
		errcode = 10;
		return 0;
	}
	if ( dn == -1 ){
		dn = sdic.get_num();
	}
	sdic.insert( dn, dic );
//	make_multiword();	//DELETE
#if 0	// TBD
#if !defined(WINCE)
	// ソートの種類の異なる辞書のチェック
	if ( sdic.get_num() >= 1 ){
		if ( sdic[0].GetOrder( ) != dic->GetOrder( ) ){
			errcode = DICERR_INCOMPTYPE;
			goto error;
		}
	}
#endif

#if defined(USE_DICORDER)
	SetOrder( sdic[0].getHeader()->dicorder );
#endif
#endif
	return 0;
}
#endif	// USE_UNIVDIC

#if INETDIC
#include "inetdic.h"

bool MultiPdic::IsNetworkUsed()
{
	for (int i=0;i<sdic.get_num();i++){
		if (sdic[i].GetNetworkMode()!=NM_NONE){
			return true;
		}
	}
	return false;
}
#endif

// return value:
//	the number of uploaded words
//	-1 is never returned.
int MultiPdic::ReqUpload()
{
#if NETDIC
	int ret = 0;
	for (int i=0;i<sdic.get_num();i++){
		InetDic *idic = sdic[i].GetInetDic();
		if (idic){
			int r = idic->ReqUpload();
			if (r>0)
				ret += r;
		}
	}
	return ret;
#else
	return 0;
#endif
}
void MultiPdic::ReqDownload(int max_count)
{
#if NETDIC
	for (int i=0;i<sdic.get_num();i++){
		InetDic *idic = sdic[i].GetInetDic();
		if (idic){
			idic->ReqDownload(max_count);
		}
	}
#endif
}
bool MultiPdic::IsNetDownloadAllowed()
{
#if NETDIC
	for (int i=0;i<sdic.get_num();i++){
		InetDic *idic = sdic[i].GetInetDic();
		if (idic){
			if (idic->DownloadAllowed){
				return true;
			}
		}
	}
	return false;
#else
	return false;
#endif
}
void MultiPdic::SetNetDownloadAllowed(bool allowed)
{
#if NETDIC
	for (int i=0;i<sdic.get_num();i++){
		InetDic *idic = sdic[i].GetInetDic();
		if (idic){
			idic->DownloadAllowed = allowed;
		}
	}
#endif
}

