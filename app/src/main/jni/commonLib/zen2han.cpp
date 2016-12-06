#include	"zen2han.h"

static unsigned short kigou_tbl[] = {											//半角ｺｰﾄﾞ (連続値)
	0x8140, 0x8149, 0x8168, 0x8194, 0x8190, 0x8193, 0x8195, 0x8166,		//0x20-0x27
	0x8169, 0x816A, 0x8196, 0x817B, 0x8143, 0x817C, 0x8144, 0x815E,		//0x28-0x2f
    0x8146, 0x8147, 0x8183, 0x8181, 0x8184, 0x8148, 0x8197,				//0x3a-0x40(0x30-0x36)
	0x816D, 0x818F, 0x816E, 0x814F, 0x8151, 0x8165,						//0x5b-0x60(0x37-0x3c)
	0x816F, 0x8162, 0x8170, 0x8150										//0x7b-0x7e(0x3d-0x40)
};

#if 0
#define ZTOH_SYMBOLS 9
static unsigned short mbc_to_mbb_symbol_table_1[ZTOH_SYMBOLS] = {
    0x8142, 0x8175, 0x8176, 0x8141, 0x8145, 0x815B, 0x814A, 0x814B
};
static unsigned char mbc_to_mbb_symbol_table_2[ZTOH_SYMBOLS] = {
	0xA1,   0xA2,   0xA3,   0xA4,   0xA5,   0xB0,   0xDE,   0xDF
};

static unsigned char mbc_to_mbb_katakana_table[0x8396-0x8340+1] = {
	0xA7, 0xB1, 0xA8, 0xB2, 0xA9, 0xB3, 0xAA, 0xB4,
	0xAB, 0xB5, 0xB6, 0xB6, 0xB7, 0xB7, 0xB8, 0xB8,
	0xB9, 0xB9, 0xBA, 0xBA, 0xBB, 0xBB, 0xBC, 0xBC,
	0xBD, 0xBD, 0xBE, 0xBE, 0xBF, 0xBF, 0xC0, 0xC0,
	0xC1, 0xC1, 0xAF, 0xC2, 0xC2, 0xC3, 0xC3, 0xC4,
	0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCA,
	0xCA, 0xCB, 0xCB, 0xCB, 0xCC, 0xCC, 0xCC, 0xCD,
	0xCD, 0xCD, 0xCE, 0xCE, 0xCE, 0xCF, 0xD0, 0,
	0xD1, 0xD2, 0xD3, 0xAC, 0xD4, 0xAD, 0xD5, 0xAE,
	0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDC,
	0xB2, 0xB4, 0xA6, 0xDD, 0xB3, 0xB6, 0xB9
};
#endif	// 0

#if defined(__WIN32__) || defined(UNIX)
// char ZHTable[] = "、・。「」";
unsigned short zen2han( unsigned short c )
{
	/*-------- ０から９ --------*/
	if (c >= 0x824f && c <= 0x8258){
		return (unsigned short)(c - 0x824f + '0');
	/*-------- ＡからＺ --------*/
	} else if (c >= 0x8260 && c <= 0x8279){
		return (unsigned short)(c - 0x8260 + 'A');
	/*-------- ａからｚ --------*/
	} else if (c >= 0x8281 && c <= 0x829a){
		return (unsigned short)(c - 0x8281 + 'a');
	} else if (c == CODE_COMMA1 ){	// 、
		return ',';
#if 1
	} else if (c == CODE_COMMA2 ){	// ・
		return (unsigned char)'･';
	} else if (c == CODE_COMMA3 ){	// 。
		return (unsigned char)'｡';
	} else if (c == CODE_BRACKET1 ){	// 「
		return (unsigned char)'｢';
	} else if (c == CODE_BRACKET2 ){	// 」
		return (unsigned char)'｣';
#endif
	} else if (c >= 0x8140 && c <= 0x8197) {
		unsigned short *p;

		/*-- 0x20から0x7eのコードに変換される文字 --*/
		int i;
		for (i = 0x20, p = kigou_tbl; i <= 0x40; i++, p++) {
			if (*p == c) {
				if (i <= 0x2f) return (unsigned short)i;
				else if (i <= 0x36) return (unsigned short)(i+0x0a);
				else if (i <= 0x3c) return (unsigned short)(i+0x24);
				return (unsigned short)(i+0x3e);
			}
		}
		// 以上の変換で引っかからなかったもの //
		switch ( c ){
			case 0x815d:	// ‐
				return '-';
		}
#if 0
		/*-- 。「」、・ー゛゜ --*/
		for (i = 0; i < ZTOH_SYMBOLS; i++) {
			if (mbc_to_mbb_symbol_table_1[i] == c) {
				return mbc_to_mbb_symbol_table_2[i];
			}
		}
#endif
	}
	return c;
}
#else
unsigned short zen2han(unsigned short c)
{
	int i;

#if 0	//濁点処理が面倒くさい、'潟'検索が出来なくなるので止める
	/*-------- カタカナ --------*/
	if ( c >= 0x8340 && c <= 0x8396 ) {
		if (c == 0x837F) return c;
		return mbc_to_mbb_katakana_table[c - 0x8340];
#endif
//	/*-------- ひらがな --------*/
//	} else if ( c >= 0x829F && c <= 0x82F1 ) {
//		return c;
	/*-------- ０から９ --------*/
	if (c >= 0x824f && c <= 0x8258){
		return c - 0x824f + '0';
	/*-------- ＡからＺ --------*/
	} else if (c >= 0x8260 && c <= 0x8279){
		return c - 0x8260 + 'A';
	/*-------- ａからｚ --------*/
	} else if (c >= 0x8281 && c <= 0x829a){
		return c - 0x8281 + 'a';
	} else if (c == L'、' ){
		return ',';
	} else if (c == L'・'){
		return (unsigned char)'･';
	} else if (c == L'。'){
		return (unsigned char)'｡';
	} else if (c == L'「'){
		return (unsigned char)'｢';
	} else if (c == L'」'){
		return (unsigned char)'｣';
	} else if (c >= 0x8140 && c <= 0x8197) {
		unsigned short *p;

		/*-- 0x20から0x7eのコードに変換される文字 --*/
		for (i = 0x20, p = kigou_tbl; i <= 0x40; i++, p++) {
			if (*p == c) {
				if (i <= 0x2f) return i;
				else if (i <= 0x36) return i+0x0a;
				else if (i <= 0x3c) return i+0x24;
				return i+0x3e;
			}
		}
		// 以上の変換で引っかからなかったもの //
		switch ( c ){
			case 0x815d:	// ‐
				return '-';
		}
#if 0
		/*-- 。「」、・ー゛゜ --*/
		for (i = 0; i < ZTOH_SYMBOLS; i++) {
			if (mbc_to_mbb_symbol_table_1[i] == c) {
				return mbc_to_mbb_symbol_table_2[i];
			}
		}
#endif
	}
	return c;
}

#endif
