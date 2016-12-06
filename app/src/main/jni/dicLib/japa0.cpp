// OLDDIC	
// NEWDIC	O
// NEWDIC2	O
// DOS		O
// WIN16	
// WIN32	O
#include	"tnlib.h"
#pragma	hdrstop
#include	"dic.h"
#ifdef NEWDIC2
#include	"arif.h"
#endif

#include	"jmerge.h"
#include	"japa.h"

#ifdef NEWDIC2
#include "id.h"
#endif

Japa::Japa()
{
	attr = WA_NORMAL;
#ifdef USE_REF
	refnum = 0;
	for ( int i=0;i<NMAXREF;i++ ){
		refdata[i] = -1;
	}
#endif
#if CHKJOBJ
	HasUndefObj = false;
#endif
}

#ifdef NEWDIC2
HWND Japa::HWindow = NULL;
#endif

void Japa::operator = ( const Japa &j )
{
	attr = j.attr;
	japa.set( ((Japa&)j).japa );
	exp.set( ((Japa&)j).exp );
	pron.set( ((Japa&)j).pron );
#ifdef USE_REF
	refnum = j.refnum;
	if ( refnum ){
		for ( int i=0;i<NMAXREF;i++ ){
			refdata[i] = j.refdata[i];
		}
	}
#endif
#if defined(NEWDIC2)
	jlinks.clear( );
	for ( int i=0;i<((Japa&)j).jlinks.get_num();i++ ){
		jlinks.add(  ((Japa&)j).jlinks[i].Clone( NULL ) );
	}
#endif
}

Japa::~Japa()
{
}

void Japa::clear( )
{
	attr = WA_NORMAL;
	japa.clear();
	exp.clear();
	pron.clear();
#ifdef USE_REF
	refnum = 0;
	for ( int i=0;i<NMAXREF;i++ ){
		refdata[i] = -1;
	}
#endif
#if defined(NEWDIC2)
	jlinks.clear();
#endif
}


BOOL Japa::IsEmpty( )
{
	if ( !japa[0] && IsEmptyEx() ){
		return TRUE;
	}
	return FALSE;
}

BOOL Japa::IsEmptyEx( )
{
	if ( !exp[0] && !pron[0]
#ifdef USE_REF
&& !refnum
#endif
#if defined(NEWDIC2)
&& !jlinks.get_num()
#endif
		){
		return TRUE;
	}
	return FALSE;
}

#if defined(NEWDIC2)
ulong Japa::GetJLinksLength( )
{
	ulong l = 0;
	for ( int i=0;i<jlinks.get_num();i++ ){
		l += jlinks[i].GetLength( );
	}
	return l;
}
#endif

int Japa::GetAllLen( )
{
	if ( IsEmptyEx() ){
		return 1 + strlen( japa );
	} else {
		// 拡張部の長さ取得
		int l1 = strlen( exp );
		if ( l1 )
			l1 += 2;
			// JT_EXP + 本体 + '\0'
#ifdef NEWDIC2
		int l2 = strlen( pron );
		if ( l2 )
			l2 += 2;
			// JT_PRON + 本体 + '\0'
		l1 += l2 + GetJLinksLength( ) + jlinks.get_num() * ( 1 + sizeof(t_jtb) + 1 );
														// JT_LINK + t_jtb + 本体 + '\0'
#endif
		;
		// 属性 + 訳 + 区切り('\0') + 拡張部の長さ + JT_END
		return 1 + strlen( japa ) + 1 + l1 + 1;
#ifdef USE_REF
( refnum ? refnum * 2 + 2 : 0 )
#endif
		;
	}
}

#ifdef NEWDIC2
// src はt_jtbを指していること(＝JT_...の次)
// decodelenは戻り値であり、非圧縮部、解凍部の合計値である（t_noc部は含まない）
// 戻り値はデコードしたバッファ
//		デコードしたバッファの先頭はt_jtb分のダミーがある（値は不定）
//		従って、デコードデータには、t_jtb + 非圧縮部 + 解凍部
//		よって、デコードデータとdecodelenは一致しない( length(デコードバッファ) = sizeof(t_jtb) + decodelen )
// 必ず、デコードバッファは呼出側でdeleteすること！！
// エラー発生は、メモリ不足か、解凍失敗
char *Japa::Decode( const char *src, ulong &decodelen )
{
	uint nocomplen = *((t_noc*)(src + sizeof(t_jtb)));	// 非圧縮部の長さ
	const char *header = src + sizeof(t_jtb) + sizeof(t_noc) + nocomplen;	// 圧縮部の先頭（ヘッダー部を含む）
	ulong complen = *((t_jtb*)src) - sizeof(t_noc) - nocomplen;	// 圧縮部の長さ（ヘッダーを含む）

	ulong orglen = ARGetOrgSize( header );
	char *decode = new char[ sizeof(t_jtb) + orglen + nocomplen ];	// 先頭のt_jtbはダミー
	if ( !decode )
		return NULL;

	memcpy( decode + sizeof(t_jtb), src + sizeof(t_jtb) + sizeof(t_noc), nocomplen );	// 非圧縮部のコピー
	long destlen;
	if ( !ARDecode( header, complen, decode + sizeof(t_jtb) + nocomplen, destlen ) ){
		delete decode;
		return NULL;
	}

	decodelen = orglen + nocomplen;
	return decode;
}
#endif

void Japa::SetAll( const char *src, int l
#ifdef NEWDIC2
, IndexData *dic 
#endif
)
{
#ifdef NEWDIC2
	int linkattr = ((Pdic*)dic)->GetLinkAttr( );
#endif
	clear();
	attr = *src++;
	if ( attr & WA_EX ){
		src = japa.set( src ) + 1;
		while ( 1 ){
			int i;
			uchar jt = *src++;
			const char *_src = src;	// _srcからt_jtbの取得はしないこと！！（圧縮していた場合、t_jtbがオーバフローすることがあるため！）
#ifdef NEWDIC2
			char *decode = NULL;
			ulong jtb;
			ulong _srclen = jtb = *((t_jtb*)src);	// _srclenは解凍すると解凍後の長さになるので注意！
			if ( jt & JT_COMP ){
				if ( jt & JT_LINK ){	// 非圧縮部にID番号が必ずあると仮定
					JLinkStructC &jlc = *(JLinkStructC*)(src-1);
					if ( ( jlc.jlink == JL_OLE && linkattr & OLA_NOTREADOLE )
						|| ( (jlc.jlink == JL_FILE || jlc.jlink == JL_ICONFILE) && linkattr & OLA_NOTREADFILE )
						|| ( jlc.jlink == JL_VOICE && linkattr & OLA_NOTREADVOICE )
#ifdef USE_JBMP
						|| ( ( jlc.jlink == JL_DIB || jlc.jlink == JL_DDB ) && linkattr & OLA_NOTREADBMP )
#endif
						)
						goto nextbyte;
					{
						JLink *jlink = NULL;
						_src = (const char*)&jlc.jlink;
						switch ( (uchar)*_src ){
#if 0		// ファイルリンクオブジェクトにはPreSetはない
							case JL_FILE:
								jlink = new JLFile( (Pdic*)dic,StrNull, 0 );
								if ( linkattr & OLA_NOTREADFILE )
									goto nextbyte;
								break;
#endif
#if 0		// BitmapオブジェクトにはPreSetはない?
							case JL_DIB:
								jlink = new JLDib( (Pdic*)dic, NULL, 0 );
								if ( linkattr & OLA_NOTREADBMP )
									goto nextbyte;
								break;
							case JL_DDB:
								jlink = new JLDdb( (Pdic*)dic, NULL, 0 );
								if ( linkattr & OLA_NOTREADBMP )
									goto nextbyte;
								break;
#endif
#if defined(USE_OLE)
							case JL_OLE:
								jlink = new JLOle( (Pdic*)dic, NULL );
								if ( linkattr & OLA_NOTREADOLE )
									goto nextbyte;
								break;
#endif
							case JL_VOICE:
								jlink = new JLVoice( (Pdic*)dic, 0, NULL, NULL, 0 );
								if ( linkattr & OLA_NOTREADVOICE )
									goto nextbyte;
								break;
						}
						if ( jlink ){
							if ( jlink->PreSet( (char*)&jlc.id ) ){
								// プリセット成功！
								src += *(t_jtb*)src + sizeof(t_jtb);
								jlinks.add( jlink );
								continue;
							}
							delete jlink;
						}
					}

				}
				// 解凍
				decode = Decode( src, _srclen );
				if ( !decode ){
					// エラー !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					return;
				}
				_src = decode;
				jt &= ~JT_COMP;
			}
			if ( jt & JT_BYTE ){
				_src += sizeof(t_jtb);
				src += jtb + sizeof(t_jtb);
			}
#endif	// NEWDIC2
			switch ( jt ){
				case JT_PRON:
					src = pron.set( _src ) + 1;
					break;
#ifdef NEWDIC2
				case JT_PRON|JT_BYTE:	// w/comp
					pron.set( _src );
					break;
#endif
				case JT_EXP:
					src = exp.set( _src ) + 1;
					break;
#ifdef NEWDIC2
				case JT_EXP|JT_BYTE:	// w/comp
					exp.set( _src );
					break;
#endif
				case JT_REF:
					{
#ifdef USE_REF
						refnum = ( (uchar)*_src++ ) / sizeof( short );
						for ( i=0;i<refnum;i++ ){
							refdata[i] = ((short*)_src)++;
						}
						for ( ;i<MAXREFNUM;i++ ){
							refdata[i] = -1;
						}
#else
						int refnum = ( (uchar)*src++ ) / sizeof( short );
						for ( i=0;i<refnum;i++ ){
							((short*)src)++;
						}
#endif
					}
					break;
				case JT_END:
					goto jmp1;
#if defined(NEWDIC2) && !defined(DICTEST)
				case JT_LINK|JT_BYTE:
					{
						JLinkFormat &jlf = *(JLinkFormat*)(_src);	// = &JLinkStruct.jlink
						JLink *jlink = NULL;
						switch ( jlf.type ){
							case JL_FILE:
								jlink = new JLFile( (Pdic*)dic,StrNull, 0 );
								if ( linkattr & OLA_NOTREADFILE )
									goto nextbyte1;
								break;
#ifdef USE_ICONFILE
							case JL_ICONFILE:
								jlink = new JLIconFile( (Pdic*)dic,StrNull,StrNull, 0 );
								if ( linkattr & OLA_NOTREADFILE )
									goto nextbyte1;
								break;
#endif
#ifdef USE_JBMP
							case JL_DIB:
								jlink = new JLDib( (Pdic*)dic, NULL, 0 );
								if ( linkattr & OLA_NOTREADBMP )
									goto nextbyte1;
								break;
							case JL_DDB:
								jlink = new JLDdb( (Pdic*)dic, NULL, NULL, 0 );
								if ( linkattr & OLA_NOTREADBMP )
									goto nextbyte1;
								break;
#endif
#if defined(USE_OLE)
							case JL_OLE:
								jlink = new JLOle( (Pdic*)dic, NULL );
								if ( linkattr & OLA_NOTREADOLE )
									goto nextbyte1;
								break;
#endif
							case JL_VOICE:
								jlink = new JLVoice( (Pdic*)dic, 0, NULL, NULL, 0 );
								if ( linkattr & OLA_NOTREADVOICE )
									goto nextbyte1;
								break;
							default:
								jlink = new JLUFO( (Pdic*)dic, 0 );
								break;
						}
						if ( !jlink->Set( _src + 1, _srclen-1 ) ){
//							error = jlink->GetErrorCode( );
//							r = FALSE;
						}
						jlinks.add( jlink );
					}
					// fall through!!
#endif
				default:
#if defined(NEWDIC2)
	nextbyte:
					if ( jt & JT_BYTE ){
		nextbyte1:
						src += jtb + sizeof(t_jtb);
						break;
					}
#endif
					goto jmp1;
			}
#ifdef NEWDIC2
			if ( decode )
				delete decode;
#endif
			continue;
jmp1:
			break;
		}
	} else {
		if ( l ){
			japa.set( src, l-1 );
		}
	}
//	return r;
}

#ifdef NEWDIC2

#define	AR_HEADER_SIZE	sizeof(AR_HEADER)

// テキスト専用圧縮
// 返り値は次へのポインター
// NULLだとエラー
// pはJT_...の次のポインターであること！！
static char *TextComp( char *p, char *top, const char *src, int compflag, uint limitlen )
{
	BOOL fOver = FALSE;
	uint len = (uint)strlen( src ) + 1;
	if ( FP_DIFF( p, top ) + len + 1 > limitlen ){	// len + JT_END
		fOver = TRUE;
		if ( !(compflag & CP_TEXT) )
			return 0;	// limitlenオーバー
		goto jmp1;
	}
	if ( !(compflag & CP_TEXT) ){
		goto notcomp;
	}
	if ( ( compflag & CP_COMP2 ) == CP_COMP2 && len >= CP_MINLEN ){
jmp1:
		char *codbuf = new char[ len + sizeof(t_jtb) + sizeof(t_noc) + AR_HEADER_SIZE ];
		if ( !codbuf ){
			if ( fOver )
				return NULL;	// メモリ不足
			goto notcomp;
		}
		long destlen;
        ARSetWindow( NULL );
		if ( AREncode( src, len, codbuf, destlen ) ){	// 圧縮成功したら元のサイズをオーバーしないと仮定している
			// 圧縮できた
			if ( fOver ){
				if ( FP_DIFF( p, top ) + sizeof(t_jtb) + sizeof(t_noc) + destlen > limitlen ){
					// 圧縮しても足りない
					delete codbuf;
					return NULL;	// limitlenオーバー
				}
			}
			*(p-1) |= JT_COMP | JT_BYTE;
			*((t_jtb*)p)++ = (t_jtb)(destlen + sizeof(t_noc));
			*((t_noc*)p)++ = 0;	// 非圧縮部長
			memcpy( p, codbuf, destlen );
			delete codbuf;
			return p+destlen;
		} else {
			// 圧縮できない
			delete codbuf;
			if ( fOver ){
				return NULL;	// limitlenオーバー
			}
			goto notcomp;
		}

	}
notcomp:
	memcpy( p, src, len );
	return p+len;
}
#endif

#ifdef NEWDIC2
// 返り値をチェックすること！
// 0 のときはメモリ不足またはlimitlenをオーバ
int Japa::Get( char *buf, int compflag, uint limitlen )
#else
int Japa::Get( char *buf )
#endif
{
//	BOOL r = TRUE;
	char *p = buf;
	if ( IsEmptyEx() ){
		attr &= ~WA_EX;
	} else {
		attr |= WA_EX;
	}
	*p++ = attr;
	if ( attr & WA_EX ){
		int ljapa = strlen( japa );
		strcpy( p, japa );
		p += ljapa + 1;

#ifdef NEWDIC2
		// トータルの長さで圧縮ポリシーを変える
		DWORD totallen = 0;
		DWORD *objlen = new DWORD[ jlinks.get_num() ];
		for ( int i=0;i<jlinks.get_num();i++ ){
			totallen += ( objlen[i] = jlinks[i].GetLength() ) + 1 + sizeof(t_jtb) + 5;	// ちょっと余計に5に
		}

		if ( (compflag & CP_COMP2) == CP_COMP ){	// 圧縮するがなるべく圧縮するでは無い場合
			totallen += strlen( exp ) + 2 + strlen( pron ) + 2 + ljapa + 5;	// ちょっと余計に5に
			if ( totallen >= limitlen )
            	compflag |= CP_COMP2;
		}
#endif
		if ( exp[0] ){
			*p++ = JT_EXP;
#ifdef NEWDIC2
			if ( (p=TextComp( p, buf, exp, compflag, limitlen )) == NULL ){
				delete objlen;
				return 0;
			}
#else
			strcpy( p, exp );
			p += strlen( exp ) + 1;
#endif
		}
#ifdef USE_REF
		if ( refnum ){
			*p++ = JT_REF;
			*p++ = (char)(refnum * sizeof( short ));
			for ( int i =0;i<refnum;i++ ){
				*((short*)p)++ = refdata[i];
			}
		}
#endif
		if ( pron[0] ){
			*p++ = JT_PRON;
#ifdef NEWDIC2
			if ( (p=TextComp( p, buf, pron, compflag, limitlen )) == NULL ){
            	delete objlen;
				return 0;
			}
#else
			strcpy( p, pron );
			p += strlen( pron ) + 1;
#endif
		}
#if defined(NEWDIC2)
		if ( FP_DIFF( p, buf ) + 1 > limitlen ){
        	delete objlen;
			return 0;
		}
		for ( i=0;i<jlinks.get_num();i++ ){
			JLink &jl = jlinks[i];
			DWORD l = objlen[i];
			if ( !l ){
				continue;	// データが無い
			}
			BOOL fOver = FALSE;
			if ( FP_DIFF( p, buf ) + 1 + sizeof(t_jtb) + 1 + l + 1 > limitlen ){	// + JT_LINK + t_jtb + ID + type + l + JT_END
				fOver = TRUE;
				if ( compflag & CP_COMP ){
					goto comp;
				} else {
					// limitlenオーバー
					delete objlen;
					return 0;
				}
			}
			if ( ( compflag & CP_COMP2 ) == CP_COMP2 && l >= CP_MINLEN ){
				// なるべく圧縮する
comp:
				char *srcbuf = new char[ l ];	// l には nocomplenが含まれている
				if ( !srcbuf ){
					if ( fOver ){
                    	delete objlen;
						return 0;	// メモリ不足
					}
					goto notcomp;
				}
				char *codbuf = new char[ l ];
				if ( !codbuf ){
					delete srcbuf;
					if ( fOver ){
                    	delete objlen;
						return 0;	// メモリ不足
					}
					goto notcomp;
				}
				if ( !jlinks[i].Get( srcbuf ) ){	// 生データ
					/// エラー処理
                    delete objlen;
					return 0;		// OLEエラー
				}
				ARSetWindow( HWindow );
				if ( HWindow ){
					SendMessage( HWindow, WM_COMP, MSGCOMP_START, 0 );
				}
				int nocomplen = 1 + sizeof(t_id) + strlen( jlinks[i].GetTitle( ) ) + 1;	// JL_... + ID + title + '\0'
				long destlen;
				int r = AREncode( srcbuf + ( nocomplen - 1 ), l - ( nocomplen - 1 ), codbuf, destlen );	// -1はリンクタイプ分(srcbufにはJL_が入っていない）
				delete srcbuf;
				if ( HWindow ){
					SendMessage( HWindow, WM_COMP, MSGCOMP_END, 0 );
				}
				ARSetWindow( NULL );
				if ( r ){
					// 圧縮できた
					if ( fOver ){
						if ( FP_DIFF( p, buf ) + sizeof(t_jtb) + sizeof(t_noc) + nocomplen + destlen > limitlen ){
							// 圧縮しても足りない
							delete codbuf;
                            delete objlen;
							return 0;	// limitlenオーバー
						}
					}
					JLinkStructC &jlsc = *(JLinkStructC*)p;
					jlsc.jtype = JT_LINK | JT_BYTE | JT_COMP;
					jlsc.size = (t_jtb)(sizeof(t_noc) + nocomplen + destlen);
					jlsc.nocomplen = (t_noc)nocomplen;	// 非圧縮長
					jlsc.jlink = (char)jl.GetType( );				// リンクタイプ
					jlsc.id = jl.GetID( );
					int titlelen = nocomplen - sizeof(t_id) - 1;	// -1はリンクタイプ分
					memcpy( jlsc.title, jl.GetTitle( ), titlelen );
					p = jlsc.title + titlelen;
					memcpy( p, codbuf, destlen );
					p += destlen;
					delete codbuf;
					goto next;
				} else {
					// 圧縮できない
					delete codbuf;
					if ( fOver ){
                    	delete objlen;
						return 0;	// limitlenオーバー
					}
					goto notcomp;
				}
			}
notcomp:
			*p++ = JT_LINK|JT_BYTE;
			*((t_jtb*)p)++ = (t_jtb)(l + 1);
			*p++ = (char)jlinks[i].GetType( );
			if ( !jlinks[i].Get( p ) ){
				delete objlen;
				return 0;		// OLEエラー
				// エラー処理
//				error = jlinks[i].GetErrorCode( );
//				r = FALSE;
			}
			p += l;
next:;
		}
        delete objlen;
#endif
		*p++ = JT_END;
	} else {
		int l = strlen( japa );
#ifdef NEWDIC2
		if ( FP_DIFF( p, buf ) + l > limitlen )
			return 0;
#endif
		strncpy( p, japa, l );
		p += l;
	}
	return FP_DIFF( p, buf );
}

#if !defined(MINI)
// mul : 標準の最大文字数の何倍まで許容するか
// -1 : エラー（メモリ不足）
//  1 : オーバー
//  0 : 正常
#ifdef NEWDIC2
int Japa::Merge( Japa &j, Pdic *dic, int SingleByte, int flags, int nmul )
#else
int Japa::Merge( Japa &j
#ifdef _Windows
	, int SingleByte
#endif
	, int flags, int nmul )
#endif
{
	int overf = 0;
	// 単語レベルはマージ元のレベルを優先？
	uchar a = j.attr;
	if ( attr & WA_LEVELMASK ){
		attr |= (uchar)( a & ~WA_LEVELMASK );
	} else {
		attr = (uchar)( attr & ~WA_LEVELMASK ) | a;
	}

#ifdef NEWDIC2
	const int l = LJALL * 2 + 100;	// 用例の最大長にあわせる
#else
	int l = strlen( japa ) + strlen( j.japa ) + 1 + strlen( exp ) + strlen( j.exp ) + 5;
#endif
	char *bf = new char[ l + 1 ];
	if ( !bf )
		return -1;

	strcpy( bf, pron );
//	overf |= jmerge( bf, j.pron, LPRON*nmul, 0, SingleByte & SRCH_PRON );
	overf |= expmerge( bf, j.pron, LPRON*nmul, 0, DLM_COMMA );
	pron.set( bf );

	strcpy( bf, japa );
#ifdef _Windows
	int r = jmerge( bf, j.japa, LJAPA*nmul, flags, SingleByte & SRCH_JAPA );
#else
	int r = jmerge( bf, j.japa, LJAPA*nmul, flags, FALSE );
#endif
	if ( r == -1 ){
		overf = -1;
		goto exit;
	}
	overf |= r;
#ifdef PDUTY
	extern int oflag;
	if ( oflag ){
		char *p = bf;
		bf = new char[ strlen( p ) + 1 ];
		bf[0] = '\0';
		r = jmerge( bf, p, LJAPA*nmul, flags, FALSE );
		delete p;
		if ( r == -1 ){
			overf = -1;
			goto exit;
		}
		overf |= r;
	}
#endif	// PDUTY
	japa.set( bf );

// 用例のマージはどうしよう？？
// 基本的に追加で、" / "を区切り文字とする
// また、重複チェックも行なう
#if 1
	if ( strlen( exp ) + strlen( j.exp ) > LEXP * nmul ){
		overf = 1;
	} else {
		if ( exp[0] ){
			if ( j.exp[0] ){
				// 用例マージ？
				strcpy( bf, exp );
#ifdef _Windows
				r = expmerge( bf, j.exp, l, SingleByte & SRCH_EXP ? 0 : flags );
#else
				r = expmerge( bf, j.exp, l, flags );
#endif
				if ( r == -1 ){
					overf = -1;
					goto exit;
				}
				exp.set( bf );
			} else {
				// 何もしない
			}
		} else {
			// 単なるコピー
			exp.set( j.exp );
		}
	}
#else
	if ( strlen( exp ) + strlen( j.exp ) > LEXP * nmul ){
		overf = 1;
	} else {
		exp.cat( j.exp );
	}
#endif

#ifdef USE_REF
	// 参照のマージ
	if ( refnum < j.refnum ){
		refnum = j.refnum;
	}
	for ( int i=0;i<refnum;i++ ){
		if ( refdata[i] == -1 )
			refdata[i] = j.refdata[i];	// 未参照の場合のみ変更（で良いかな？）
	}
#endif

#if defined(NEWDIC2)
	// リンクデータのマージ
	// 単純に追加？
	{
	for ( int i=0;i<j.jlinks.get_num();i++ ){
		jlinks.add( j.jlinks[i].Clone( dic ) );
	}
	}
#endif

exit:
	delete bf;
	return overf;
}
#endif	// !defined(MINI)

#ifdef NEWDIC2
// lengthは返り値がNULL出ないときのみ有効
char *Japa::Get( uint &length, int compflag, uint limitlen )
#else
char *Japa::Get( )
#endif
{
	uint len = GetAllLen( );
#ifdef NEWDIC2
	if ( len >= limitlen && !compflag )
		return NULL;	// 制限超過
#endif
	char *buf = new char[ len ];
	if ( !buf ){
		return NULL;	// メモリ不足
	}
#ifdef NEWDIC2
	if ( !(length = Get( buf, compflag, limitlen )) ){
		delete buf;
		return NULL;
	}
#else
	Get( buf );
#endif
	return buf;

}

#ifdef USE_REF
void Japa::GetRef( char *buf, int i )
{
	if ( refdata[i] == -1 ){
		buf[0] = '\0';
	} else {
#ifdef	_WINDOWS
		wsprintf( buf, "%04d", refdata[i] );
#else
		esprintf( buf, "%04d", refdata[i] );
#endif
	}
}

void Japa::SetRef( const char *buf, int i )
{
	if ( buf[0] ){
		refdata[i] = (short)atoi( buf );
		if ( refnum <= i ){
			refnum = i + 1;
		}
	} else {
		refdata[i] = -1;
		TruncRef( );
	}
}

void Japa::TruncRef( )
{
	while ( refnum && refdata[refnum-1] == -1 ) refnum--;
}
#endif



