#include "tnlib.h"
#pragma	hdrstop
#include	"jtype.h"

//日本語の単語マージ用のデリミタ判断
bool isdelim2(unsigned short c)
{
	if (isdelim(c))
		return true;
	if (c == _T(';') || /* c == '.' || */ c == CODE_SEMICOLON || c=='\r' || c=='\n' || c=='\t')
		return true;
	return false;
}

#ifndef _UNICODE
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
#endif

#ifndef _UNICODE
//全角・半角片仮名同一視の文字列比較
int jkstricmp( const char *str1, const char *str2 )
{
	while ( 1 ){
		unsigned short us1, us2;
		LD_CHAR( us1, str1 );
		LD_CHAR( us2, str2 );
		if ( !us1 ){
			if ( !us2 ){
				return 0;
			} else {
				return -1;
			}
		} else if ( !us2 ){
			return 1;
		}
		/*-------- カタカナ --------*/
		if ( us1 >= 0x8340 && us1 <= 0x8396 && us1 != 0x837f ) {
			us1 = mbc_to_mbb_katakana_table[us1 - 0x8340];
		}
		if ( us2 >= 0x8340 && us2 <= 0x8396 && us2 != 0x837f ) {
			us2 = mbc_to_mbb_katakana_table[us2 - 0x8340];
		}
    
		if ( us1 != us2 ){
			return (int) ( us1 - us2 );
		}
	}
}
#endif

// strが語尾(s,ing,ed)であるかどうか判断する
// 語尾の次がスペース以下のコードでなければならない
// 返り値は語尾の次のポインタ
char *issuffix( const char *str )
{
	if ( *str == 's' ){
		str++;
		if ( (unsigned char)*str <= ' ' ){
			return (char*)str;
		}
	}
	if ( !strncmp( str, "ing", 3 ) ){
		str += 3;
		if ( (unsigned char)*str <= ' ' ){
			return (char*)str;
		}
	}
	if ( !strncmp( str, "ed", 2 ) ){
		str += 2;
		if ( (unsigned char)*str <= ' ' ){
			return (char*)str;
		}
	}
	return NULL;
}


int jcut(tnstr &str, int len)
{
	int l = _tcslen(str);
	jstrcut( str.c_str(), len );
	return ( l > len )?1:0;
}

bool IsRegularChar(tchar c)
{
	static const tchar *regstr = _t("\\[]-+*?.^$()|");
	const tchar *p = regstr;
	for(;*p;){
		if (*p==c){
			return true;
		}
		p++;
	}
	return false;
}

//TODO: to langproc
const tchar *NextDelim(const tchar *s)
{
	int type = -1;
	for (;;){
		tchar c = *s;
		if (!IsWordChar(c))
			return s;
		int new_type;
		if (c>=0x300){
			if (iskatakana(c) || c==CODE_JPROLONG){
				new_type = 3;
			} else
			if (ishiragana(c)){
				new_type = 2;
			} else
			if (c>=0x1E00 && c<0x2000){
				new_type = 0;
			} else {
				new_type = 1;
			}
		} else {
			new_type = 0;
		}
		if (type==-1)
			type = new_type;
		else
			if (type!=new_type)
				return s;	// 文字種が変わった
		s++;
	}
}

