#include "pdclass.h"
#pragma hdrstop
#include "pddefs.h"
#include "pdconfig.h"
#include "pdprof.h"
#include "squfont.h"
#include "id.h"

#include "vclcom.h"

/*------------------------------------------*/
/*		Global Variables					*/
/*------------------------------------------*/
TFontAttr nfont;

#if SQUFONT
TFontAttr wfont;
#ifdef USE_DT2
TFontAttr wfontASC;
TFontAttr nfontASC;
#endif
#ifdef NEWDIC2
TFontAttr pfont;
#endif
#endif

COLORREF LinkColors[ N_SECTION+1+1 ];

#if SQUFONT
TFontAttr jfont;
TFontAttr efont;
TFontAttr gfont;
#endif

TNFONT *hnFont = NULL;
#if SQUFONT
TNFONT *hwFont = NULL;
TNFONT *hpFont = NULL;
TNFONT *hjFont = NULL;
TNFONT *heFont = NULL;
TNFONT *hgFont = NULL;
#endif

#if MIXDIC && SQUFONT
bool wSingleByte;
bool jSingleByte;
bool eSingleByte;
bool pSingleByte;
#endif

int nFontsOpened = 0;

int fNormalSize = 0;	// 入力時にノーマルサイズにする項目

int useuniqfont = SNF_PRON;	// Flags of SNF_xxx bit.

namespace squfont {
int HtmlEnabled = SQM_ALL & ~(SQM_PRON);
bool CanDispDicName = false;
};

TFontAttr *GetFontAttrFromSN(int sn)
{
	switch (sn){
		case SN_WORD:
			return useuniqfont & SNF_WORD ? &wfont : &nfont;
		case SN_JAPA:
			return useuniqfont & SNF_JAPA ? &jfont : &nfont;
		case SN_EXP1:
			return useuniqfont & SNF_EXP1? &efont : &nfont;
		case SN_PRON:
			return useuniqfont & SNF_PRON? &pfont : &nfont;
		default:
			__assert__;
			return NULL;
	}
}

void _CreateTextFonts( HDC hdc )
{
	if ( nFontsOpened++ ){
#ifdef _DEBUG
		if (hdc!=hnFont->hdc){
			DBW("hdc=%08X hnFont=%08X", hdc, hnFont->hdc);
			__assert__;
			nFontsOpened = 1;	// recovery
		} else {
			return;
		}
#else
		return;
#endif
	}
#if SQUFONT
	hnFont = new TNFONT( hdc, nfont, LinkColors[0] );
	hwFont = useuniqfont & SNF_WORD ? new TNFONT( hdc, wfont, LinkColors[2] ) : hnFont;
	hpFont = useuniqfont & SNF_PRON ? new TNFONT( hdc, pfont, LinkColors[3] ) : hnFont;
	hjFont = useuniqfont & SNF_JAPA ? new TNFONT( hdc, jfont, LinkColors[5] ) : hnFont;
	heFont = useuniqfont & SNF_EXP1 ? new TNFONT( hdc, efont, LinkColors[6] ) : hnFont;
	hgFont = useuniqfont & SNF_EPWING ? new TNFONT( hdc, gfont, LinkColors[7] ) : hnFont;
//#if MIXDIC
#if 0
	wSingleByte = hwFont->fSingleByte;
	jSingleByte = hjFont->fSingleByte;
	eSingleByte = heFont->fSingleByte;
	pSingleByte = hpFont->fSingleByte;
#endif
#else
	hnFont = new TNFONT( hdc, NULL, LinkColors[0] );
#endif
}

void _DeleteTextFonts( )
{
	if ( nFontsOpened <= 0 )
		return;
	if ( --nFontsOpened )
		return;
	delete hnFont;
	hnFont = NULL;
#if SQUFONT
	if ( useuniqfont & SNF_WORD ){
		delete hwFont;
		hwFont = NULL;
	}
	if ( useuniqfont & SNF_PRON ){
		delete hpFont;
		hpFont = NULL;
	}
	if ( useuniqfont & SNF_JAPA ){
		delete( hjFont );
		hjFont = NULL;
	}
	if ( useuniqfont & SNF_EXP1 ){
		delete( heFont );
		heFont = NULL;
	}
	if ( useuniqfont & SNF_EPWING ){
		delete( hgFont );
		hgFont = NULL;
	}
#endif
}

#if 0
HDC _GetCurrentHDC( )
{
	if ( nFontsOpened <= 0 )
		return NULL;
	return hnFont->hdc;
}
#endif

void _SetCurrentHDC( HDC hdc )
{
	if ( nFontsOpened <= 0 )
		return;
	if ( hnFont )
		hnFont->hdc = hdc;
#if SQUFONT
	if ( hwFont )
		hwFont->hdc = hdc;
	if ( hpFont )
		hpFont->hdc = hdc;
	if ( hjFont )
		hjFont->hdc = hdc;
	if ( heFont )
		heFont->hdc = hdc;
	if ( hgFont )
		hgFont->hdc = hdc;
#endif
}

FontHandles::FontHandles()
{
	hnFont = NULL;
}

FontHandles::~FontHandles()
{
	if ( hnFont )
		Delete();
}

#if SQUFONT
static
HFONT CreateAFont( TFontAttr &font, int flag, HFONT hDefFont, bool fNormalSize )
{
	if ( fNormalSize ){
		if ( useuniqfont & flag ){
			return font.CreateFontNS();
		} else {
			return hDefFont;
		}
	} else
	{
		return useuniqfont & flag ? font.CreateFont() : hDefFont;
	}
}
HFONT CreateOneFont( int type, bool fNormalSize )
{
	HFONT hFont;
	switch ( type ){
		case SN_WORD: hFont = CreateAFont( wfont, SNF_WORD, NULL, fNormalSize ); break;
		case SN_PRON: hFont = CreateAFont( pfont, SNF_PRON, NULL, fNormalSize ); break;
		case SN_JAPA: hFont = CreateAFont( jfont, SNF_JAPA, NULL, fNormalSize ); break;
		case SN_EXP1: hFont = CreateAFont( efont, SNF_EXP1, NULL, fNormalSize ); break;
		case SN_EPWING: hFont = CreateAFont( gfont, SNF_EPWING, NULL, fNormalSize ); break;
		default: hFont = NULL; break;
	}
	if ( !hFont ){
		hFont = nfont.CreateFont(fNormalSize);
	}
	return hFont;
}
#endif

#if 0
// 発音記号入力用メニューのためのもの
// これで作成したらDeleteObjectすること
HFONT CreatePFont( )
{
	return CreateFontIndirect( useuniqfont & SNF_PRON ? pfont : nfont );
}
#endif

// 注意：USE_DEFFONTSIZEが0であると、fNormalSizeは無視される(必ずfalse)
void FontHandles::Create( int fNormalSize )	// SQM_フラグの組み合わせ
{
#if SQUFONT
	if ( hnFont )
		return;

#if USE_DEFFONTSIZE
	hnFont = nfont.CreateFont(fNormalSize);
#else
	hnFont = nfont.CreateFont();
#endif

	hwFont = CreateAFont( wfont, SNF_WORD, hnFont, IsNormalSize( SQM_WORD ) );
	hpFont = CreateAFont( pfont, SNF_PRON, hnFont, IsNormalSize( SQM_PRON ) );
	hjFont = CreateAFont( jfont, SNF_JAPA, hnFont, IsNormalSize( SQM_JAPA ) );
	heFont = CreateAFont( efont, SNF_EXP1, hnFont, IsNormalSize( SQM_EXP ) );
	hgFont = CreateAFont( gfont, SNF_EXP1, hnFont, false );
#endif
}

void FontHandles::Delete()
{
	if ( !hnFont )
		return;
	
#if SQUFONT
	DeleteObject( hnFont );
	if ( useuniqfont & SNF_WORD ){
		DeleteObject( hwFont );
		hwFont = NULL;
	}
	if ( useuniqfont & SN_PRON ){
		DeleteObject( hpFont );
		hpFont = NULL;
	}
	if ( useuniqfont & SN_JAPA ){
		DeleteObject( hjFont );
		hjFont = NULL;
	}
	if ( useuniqfont & SN_EXP ){
		DeleteObject( heFont );
		heFont = NULL;
	}
	if ( useuniqfont & SN_EPWING ){
		DeleteObject( hgFont );
		hgFont = NULL;
	}
#endif
}

///// プロファイル処理

// type が -1 のときは id をGetString()したものを返す
// それ以外では末尾に type + '0' を付加したものを返す
// note: bufの領域確保にはやや難あり
static tchar *buf_GetString = NULL;
static const tchar *GetString(const tchar *str, int type)
{
	if ( type == -1 )
		return str;
	tchar *buf = buf_GetString;
	if ( buf_GetString )
		delete[] buf_GetString;
	if ( str ){
		int l = _tcslen( str );
		buf = new tchar[ l + 2 ];
		_tcscpy( buf, str );
		buf[ l ] = (tchar)(type + '0');
		buf[ l+1 ] = '\0';
	} else {
		buf = new tchar[1];
		buf[0] = '\0';
	}
	buf_GetString = buf;
	return buf;
}
static const tchar *GetString( int id, int type )
{
	const tchar *key = GetString(id);
	return GetString(key, type);
}
static void FreeGetString()
{
	if (buf_GetString){
		delete[] buf_GetString;
		buf_GetString = NULL;
	}
}

// フォントに関するプロファイルを保存
void SaveProfileFont( TRegKey *key, int type, TFontAttr &fa, COLORREF color, COLORREF linkcolor, TFontAttr *faASC )
{
	key->WriteInteger(GetString(PFS_FONTUSEUNIQ, type), (useuniqfont & (1<<type)) ? 1 : 0);
#if SQUFONT
	key->WriteString( GetString( PFS_FONTNAME, type ), fa.Name );
	key->WriteInteger( GetString( PFS_FONTHEIGHT, type ), fa.Size );
	key->WriteInteger( GetString( PFS_FONTCHARSET, type ), fa.CharSet );
	key->WriteInteger( GetString( PFS_FONTBOLD, type ), fa.IsBold() );
#ifndef NOSQUCOLOR
	key->WriteInteger( GetString( PFS_COLOR, type ), color );
#endif
	key->WriteInteger( GetString( PFS_LINKCOLOR, type ), linkcolor );
	if ( faASC ){
		key->WriteString( GetString( PFS_FONTNAMEASC, type ), faASC ? faASC->Name.c_str() : _t("") );
		key->WriteInteger( GetString( PFS_FONTCHARSETASC, type ), faASC ? faASC->CharSet : 0 );
	} else {
		key->DeleteValue( GetString( PFS_FONTNAMEASC, type ) );
		key->DeleteValue( GetString( PFS_FONTCHARSETASC, type ) );
	}
#if USE_SPECIALCHAR
	int _type = -1;
	if ( type >= 0 )
		_type = sn2type( type );
	if ( _type >= 0 && splangname[_type][0] )
		key->WriteString( GetString( PFS_SPLANGNAME, type ), splangname[_type] );
	else
		key->DeleteValue( GetString( PFS_SPLANGNAME, type ) );
#endif
	FreeGetString();
#endif
}

void SaveProfileFont( int type, TFont *font, COLORREF linkcolor )
{
	prof.WriteString( NULL, GetString( PFS_FONTNAME, type ), TString(font->Name.c_str()) );
	prof.WriteInteger( GetString( PFS_FONTHEIGHT, type ), font->Height );
	prof.WriteInteger( GetString( PFS_FONTCHARSET, type ), font->Charset );
	prof.WriteInteger( GetString( PFS_FONTBOLD, type ), ToInteger(font->Style) );
#ifndef NOSQUCOLOR
	prof.WriteInteger( GetString( PFS_COLOR, type ), font->Color );
#endif
	prof.WriteInteger( GetString( PFS_LINKCOLOR, type ), linkcolor );
#if 0
	if ( faASC ){
		prof.Put( GetString( PFS_FONTNAMEASC, type ), faASC ? faASC->Name : NULL );
		prof.PutLong( GetString( PFS_FONTCHARSETASC, type ), faASC ? faASC->CharSet : NULL );
	} else {
		prof.EraseEntry( GetString( PFS_FONTNAMEASC, type ) );
		prof.EraseEntry( GetString( PFS_FONTCHARSETASC, type ) );
	}
#endif
#if USE_SPECIALCHAR
	int _type = -1;
	if ( type >= 0 )
		_type = sn2type( type );
	if ( _type >= 0 && splangname[_type][0] )
		prof.WriteString( NULL, GetString( PFS_SPLANGNAME, type ), splangname[_type] );
	else
		prof.EraseEntry( GetString( PFS_SPLANGNAME, type ) );
#endif
}

// フォントに関するエントリを全て削除
void DeleteProfileFont( TRegKey *key, int type )
{
#if SQUFONT
	key->DeleteValue( GetString( PFS_FONTNAME, type ) );
	key->DeleteValue( GetString( PFS_FONTHEIGHT, type ) );
	key->DeleteValue( GetString( PFS_FONTCHARSET, type ) );
	key->DeleteValue( GetString( PFS_FONTBOLD, type ) );
#ifndef NOSQUCOLOR
	key->DeleteValue( GetString( PFS_COLOR, type ) );
#endif
	key->DeleteValue( GetString( PFS_LINKCOLOR, type ) );
	key->DeleteValue( GetString( PFS_FONTNAMEASC, type ) );
	key->DeleteValue( GetString( PFS_FONTCHARSET, type ) );
#if USE_SPECIALCHAR
	key->DeleteValue( GetString( PFS_SPLANGNAME, type ) );
#endif
	FreeGetString();
#endif
}

//Note:
//	DicAutoConf.cppからも参照しているので注意
void LoadProfileFont( TRegKey *key, int type, TFontAttr &fa, COLORREF &color, COLORREF &linkcolor, TFontAttr *faASC )
{
	bool useuniq = false;
	tnstr buf = key->ReadString( GetString( PFS_FONTNAME, type ), fa.Name);
	if ( buf[0] ){
		// 存在した場合
		fa.Name = buf;
		fa.Size = key->ReadInteger( GetString( PFS_FONTHEIGHT, type ), fa.Size );
		fa.CharSet = key->ReadInteger( GetString( PFS_FONTCHARSET, type ), fa.CharSet );
		fa.SetBold( key->ReadInteger( GetString( PFS_FONTBOLD, type ), fa.IsBold() ) );
#ifndef NOSQUCOLOR
		color = key->ReadInteger( GetString( PFS_COLOR, type ), color );
		linkcolor = key->ReadInteger( GetString( PFS_LINKCOLOR, type ), linkcolor );
#endif
		if ( faASC ){
			*faASC = fa;
			faASC->Name = key->ReadString( GetString( PFS_FONTNAMEASC, type ), faASC->Name);
			faASC->CharSet = key->ReadInteger( GetString( PFS_FONTCHARSETASC, type ), faASC->CharSet);
		}
		if (type!=-1){
			useuniq = key->ReadInteger( GetString(PFS_FONTUSEUNIQ, type), (useuniqfont & (1<<type))?1:0) ? true : false;
		}
	}
	if ( type != -1 ){
		if (useuniq)
			useuniqfont |= (1<<type);
		else
			useuniqfont &= ~(1<<type);
	}


#if USE_SPECIALCHAR
	if ( type >= 0 ){
		int _type = sn2type( type );
		if ( _type >= 0 ){
			splangname[_type] = key->ReadString( GetString( PFS_SPLANGNAME, type ), splangname[_type] );
			if (splangname[_type].IsEmpty()){
				if ( type == SN_PRON ){
					splangname[_type].set( GetString( IDS_ENGLISH ) );
				}
			}
		}
	}
#endif
	FreeGetString();
}

void LoadProfileFont( int type, TFont *font, COLORREF &linkcolor )
{
	static const tchar *Str = _T("@@@@");
	tnstr buf = prof.ReadString( NULL, GetString( PFS_FONTNAME, type ), Str);
	if ( _tcscmp( buf, Str ) ){
		// 成功した場合
		font->Name = buf.c_str();
		font->Height = prof.ReadInteger( GetString( PFS_FONTHEIGHT, type ), font->Height );
		font->Charset = prof.ReadInteger( GetString( PFS_FONTCHARSET, type ), font->Charset );
		font->Style = ToFontStyles(prof.ReadInteger( GetString( PFS_FONTBOLD, type ), ToInteger(font->Style) ));
#ifndef NOSQUCOLOR
		font->Color = (TColor)prof.ReadInteger( GetString( PFS_COLOR, type ), font->Color );
		linkcolor = prof.ReadInteger( GetString( PFS_LINKCOLOR, type ), linkcolor );
#endif
#if 0
		if ( faASC ){
			*faASC = fa;
			faASC->Name = prof.ReadString( GetString( PFS_FONTNAMEASC, type ), faASC->Name );
			faASC->CharSet = prof.ReadInteger( GetString( PFS_FONTCHARSETASC, type ), faASC->CharSet );
		}
#endif
		if ( type != -1 )
			useuniqfont |= (1<<type);
	} else {
		if ( type != -1 )
			useuniqfont &= ~(1<<type);
	}


#if USE_SPECIALCHAR
	if ( type >= 0 ){
		int _type = sn2type( type );
		if ( _type >= 0 ){
			prof.ReadString( NULL, GetString( PFS_SPLANGNAME, type ), splangname[_type] );
			if (splangname[_type].IsEmpty()){
				if ( type == SN_PRON ){
					splangname[_type].set( GetString( IDS_ENGLISH ) );
				}
			}
		}
	}
#endif
}

int GetCY( int no )
{
#if SQUFONT
	switch ( no ){
		case SN_NUMBER:	return hnFont->cy;
		case SN_WORD:	return hwFont->cy;
		case SN_PRON:	return hpFont->cy;
		case SN_JAPA:	return hjFont->cy;
		case SN_EXP1:	return heFont->cy;
		case SN_EPWING:	return hgFont->cy;
	}
	return 0;
#else
	return hnFont->cy;
#endif
}

// sqm_flag can be combination of flags.
void AssignFont(int sqm_flag, TFont *font, TWinControl *control, int maxheight)
{
	// sqm_flagで一番最初に立っているフラグのフォントを使用する
	// 複数のフラグが立っていてもそれらは同じフォントを使用していると考えられるため
	TFontAttr *fa = &nfont;
	if (sqm_flag&SQM_WORD){
		sqm_flag = SQM_WORD;
		if (useuniqfont & SNF_WORD)
			fa = &wfont;
	} else
	if (sqm_flag&SQM_PRON){
		sqm_flag = SQM_PRON;
		if (useuniqfont & SNF_PRON)
			fa = &pfont;
	} else
	if (sqm_flag&SQM_JAPA){
		sqm_flag = SQM_JAPA;
		if (useuniqfont & SNF_JAPA)
			fa = &jfont;
	} else
	if (sqm_flag&SQM_EXP){
		sqm_flag = SQM_EXP;
		if (useuniqfont & SNF_EXP1)
			fa = &efont;
	} else
	if (sqm_flag&SQM_EPWING){
		sqm_flag = SQM_EPWING;
		if (useuniqfont & SNF_EPWING)
			fa = &gfont;
	} else {
		sqm_flag = SQM_COMMON;
	}
	return AssignFont(*fa, font, control, fNormalSize&sqm_flag?true:false, maxheight);
}

void AssignFont(TFontAttr &fa, TFont *font, TWinControl *control, bool useNormalSize, int maxheight)
{
	LOGFONT lf;
	fa.Get(lf);
	if (control){
		HDC hdc = GetDC(control->Handle);
		int height = ::SetFont(font, lf, useNormalSize, hdc, false);
		control->Height = min((int)(maxheight>=0?maxheight:INT_MAX), height+2);	//TODO: +2は微調整（何に依存しているか不明）
		if (hdc)
			DeleteDC(hdc);
	} else {
		::SetFont(font, lf, useNormalSize);
	}
}

// 特殊文字入力関連 //////////////////////////////////////////////////////////////////////////
#if USE_SPECIALCHAR
tnstr splangname[4];
#endif

#if USE_DT2
#include "hyplink.h"
#include "draw4.h"
// 強調表示 for HyperLink
//#define	MAXENPHTEXT	1024
void MakeEnph( EnphTextVec &et, THyperLinks &hls, int item )
{
	for ( int i=0;i<hls.get_num();i++ ){
		if ( hls[i].item != item ) continue;
		EnphText item;
		item.start = hls[i].loc;
		item.end = hls[i].loc + hls[i].length;
		item.area = &hls[i].area;
		item.type = HLT_WORD;
//		item.color = linkcolor;
		item.wordcount = hls[i].wordcount;
		et.push_back(item);
		//if ( et.size() == MAXENPHTEXT ) break;
	}
}
#endif
