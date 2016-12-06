//
//	Win-CE版,SMALL版で使用するjtypeのみ
//
#include "tndefs.h"
#pragma	hdrstop
#include "jtype.h"

//区切り文字統一用のデリミタ判断
bool isdelim(unsigned short c)
{
	if ( c == _T(',') || c == (tuchar)_T('､') || c == (tuchar)_T('｡') || c == _T('\0') || c == CODE_COMMA1 || c == CODE_COMMA || c == CODE_SPACE || c == CODE_PERIOD || c == CODE_COMMA3 )
		return true;
	//半角スペースは区切りであると認識しない
	return false;
}

//英数字の判別
bool isalphanum( int c )
{
	if ( (tuchar)c >= _T('A') && (tuchar)c <= _T('Z') ){
		return true;
	}
	if ( (tuchar)c >= _T('a') && (tuchar)c <= _T('z') ){
		return true;
	}
	if ( (tuchar)c >= _T('0') && (tuchar)c <= _T('9') ){
		return true;
	}
	return false;
}

#if !defined(__ANDROID__) || defined(__BORLANDC__)
#define	LBR_ZENTOP	6
static tuschar lbr_tbl[] = {
	'(', '[', '{', '｢', '<', '{',
//	L'（',L'［',L'｛',L'「',L'＜',L'《',L'〔',L'〈',L'『',L'【'
	CODE_LBR1, CODE_LBR2, CODE_LBR3, CODE_LBR4, CODE_LBR5,
	CODE_LBR6, CODE_LBR7, CODE_LBR8, CODE_LBR9, CODE_LBR10,
	0
};

static tuschar rbr_tbl[] = {
	')', ']', '}', '｣', '>', '}',
//	L'）',L'］',L'｝',L'」',L'＞',L'〕',L'〉',L'》',L'』',L'】'
	CODE_RBR1, CODE_RBR2, CODE_RBR3, CODE_RBR4, CODE_RBR5,
	CODE_RBR6, CODE_RBR7, CODE_RBR8, CODE_RBR9, CODE_RBR10,
	0
};

bool islbr(tushort c)
{
	tushort *table = lbr_tbl;

	for (;*table;){
		if (*table == c)
			return true;
		table++;
	}
	return false;
}

// 全角の左括弧のみ
bool islbr2( tushort c )
{
	tushort *table = &lbr_tbl[LBR_ZENTOP];

	for (;*table;){
		if ( *table == c )
			return true;
		table++;
	}
	return false;
}

bool isrbr(tushort c)
{
	tushort *table = rbr_tbl;
	for (;*table;){
		if (*table == c)
			return true;
		table++;
	}
	return false;
}
#endif

#ifndef _UNICODE
bool mbIsWordChar( unsigned short c )
{
	if ( c < 0x100 ){
		// 半角文字
		if ( isalphanum( c ) || c == '-' || c == '\'' ||
			 (c >= 0x86 && c <= 0xBF ) )
			return true;
		else
			return false;
	}
	// 全角文字
	if (
		// 英数字、カタカナ、ひらがな、ギリシャ、ロシア
		(c >= 0x824f && c <= 0x8491) ||
		// 漢字
		(c >= 0x889F) ||
		// アポストロフィ
		(c == CODE_APOSTROPHE)
		)
		return true;
	else
		return false;
}
#endif

#ifdef WINCE
#pragma	message( "UNICODE charactor not completed")
#endif
