#include "tnlib.h"
#pragma hdrstop
#include "defs.h"
#include "draw4com.h"
#include "CharHT.h"

#define	foreach(obj, it, type) \
	for (type::iterator it=(obj).begin();it!=(obj).end();it++)

TDrawSetting::TDrawSetting()
{
	TabCol = 4;
	RevHeight = 14;	// Height ratio of underline.
	InitLeft = 0;	// １行目のオフセット値(rc->leftからのオフセット・ドット値)
	DefReverseSize = 0;
	WordBreak = true;
	HtmlParse = true;
	LastRight = 0;		// 最後に表示したテキストの右端座標
	LastTop = 0;	// 最終行のY座業
	MaxRight = 0;	// 最大の表示幅(ただし、DF_CALCRECTの場合のみ)
	CYMax = 0;		// 最終行の最大高さ
}

void TDrawSetting::SetRevHeight( int ratio )
{
	RevHeight = 16 - ratio;
}

TDispLineInfo &TDispLineInfo::operator = (const TDispLineInfo&o)
{
	memcpy(this, &o, sizeof(*this));
	return *this;
}

void EnphTextVec::Sort()
{
	// Bubble sort
	for (int i=0;i<(int)size()-1;i++){
		for (int j=(int)size()-1;j>i;j--){
			if ((*this)[j].start<(*this)[j-1].start){
				EnphText t = (*this)[j];
				(*this)[j] = (*this)[j-1];
				(*this)[j-1] = t;
			}
		}
	}
}

#if 1	// linkの途中で改行された場合にも対処するため
struct EnphTextOuter {
	EnphText *et;
	int index;
};
int EnphTextWork::sortfunc( const void *a, const void *b )
{
	int r = (*((EnphTextOuter*)a)).et->start - (*((EnphTextOuter*)b)).et->start;
	if ( r==0 ){
		return (*((EnphTextOuter*)a)).et->end - (*((EnphTextOuter*)b)).et->end;
	} else {
		return r;
	}
}
#else
int EnphTextWork::sortfunc( const void *a, const void *b )
{
	if ( ((LocList*)a)->loc == ((LocList*)b)->loc ){
		return ((LocList*)a)->index - ((LocList*)b)->index;
	} else {
		return ((LocList*)a)->loc - ((LocList*)b)->loc;
	}
}
#endif
// enphのLocリストからソートして、loclistへ昇順に並べたリストを作る
// Note:
//	重複分はマージされる。
//	しかし、loclist[].indexの値はマージ前のindexであるため要注意。
//	indexを使用する配列はマージ前のsizeでallocateすること。
/* static */
int EnphTextWork::sEnphSort( const EnphTextVec &enph, LocList *loclist )
{
	if (!enph.size())
		return 0;
#if 1
	const int auto_num = 16;
	EnphTextOuter enph_pts_a[auto_num];
	EnphTextOuter *enph_pts_d = NULL;

	EnphTextOuter *enph_pts;
	if (enph.size()<=auto_num){
		enph_pts = enph_pts_a;	// use stack area
	} else {
		enph_pts = enph_pts_d = new EnphTextOuter[enph.size()];	// use dynamic memory
	}

	// copy the pointers of enph.
	for (int i=0;i<enph.size();i++){
		enph_pts[i].et = (EnphText*)&enph[i];
		enph_pts[i].index = i;
	}
	qsort(enph_pts, enph.size(), sizeof(EnphTextOuter), sortfunc);

	int _etnum = 0;
	int last_loc[2] = {0,0};
	for ( int i=0;i<enph.size();i++ ){
		EnphText &et = *enph_pts[i].et;
		__assert(et.type<sizeof(last_loc)/sizeof(int));

		if (last_loc[et.type]>et.start){
			if (last_loc[et.type]>et.end){
				continue;	// overwrite by previous enph
			}
			loclist[_etnum].loc = last_loc[et.type];
		//dbw("%d:loc1=%d", _etnum, loclist[_etnum].loc);
		} else {
			loclist[_etnum].loc = et.start;
		//dbw("%d:loc2=%d", _etnum, loclist[_etnum].loc);
		}
		//loclist[_etnum].index = _etnum;
		loclist[_etnum].index = enph_pts[i].index*2;
		_etnum++;
		last_loc[et.type] = loclist[_etnum].loc = et.end;
		//loclist[_etnum].index = _etnum;
		loclist[_etnum].index = enph_pts[i].index*2+1;
		//dbw("%d:loc=%d", _etnum, loclist[_etnum].loc);
		_etnum++;
	}
	if (enph_pts_d)
		delete[] enph_pts_d;
	return _etnum;
#else
	int i;
	int _etnum = 0;
	for ( i=0;i<enph.size();i++ ){
//		if ( enph[i].start == enph[i].end ) continue;
		loclist[_etnum].loc = enph[i].start;
		loclist[_etnum].index = _etnum;
		_etnum++;
		loclist[_etnum].loc = enph[i].end;
		loclist[_etnum].index = _etnum;
		_etnum++;
	}
	qsort( loclist, _etnum, sizeof(LocList), sortfunc );
#if 0	// 意味なし？期待通りに動かない
	_etnum = 0;
	int last_loc = 0;
	for ( i=0;i<enph.size();i++ ){
		if (last_loc>loclist[i*2].loc){	// start
			if (last_loc>loclist[i*2+1].loc){	// end
				continue;	// overwrite by previous enph
			}
			loclist[_etnum].loc = last_loc;
		} else {
			loclist[_etnum].loc = loclist[i*2].loc;
		}
		//loclist[_etnum].loc = loclist[i*2].loc;	// start
		//loclist[_etnum].index = _etnum;
		_etnum++;
		loclist[_etnum].loc = loclist[i*2+1].loc;	// end
		//loclist[_etnum].index = _etnum;
		_etnum++;
	}
#endif
	return _etnum;
#endif
}

void EnphTextWork::AdjustEnph(CharHT *cht, int flags, RECT *rc)
{
	if ( cht ){
		fCheckHit = true;
		//cht->pt;	// ヒットテスト座標
		if ( flags & DF_CALCRECT ){
//			if ( cht->fOverRect ){
				// 上方チェック
				if ( cht->pt.y < rc->top ){
					if ( cht->pos1 == -1 ){
						// 先に現れた方を優先
						cht->pos1 = 0;
						cht->item1 = cht->curitem;
						cht->fOver = CHT_ABOVE;
					}
					fHit = true;
				} else
				// 左方チェック
				//TODO: originalでは nInitLeft を加算してチェックしていたが
				// 正しくないと思う。あとでチェックすること！！！ 2007.10.2
				if ( cht->pt.x < rc->left /*+ nInitLeft*/ ){
					cht->fOver = CHT_LEFT;
					fOverLeft = true;
				}
//			}
		} else {
			ccPoint = new int[ 2 ];
			// start,endの入換え
			int cht_start;
			int cht_end;
			if ( cht->start > cht->end ){
				cht_start = cht->end;
				cht_end = cht->start;
			} else {
				cht_start = cht->start;
				cht_end = cht->end;
			}
			if ( flags & DF_UNREDRAW ){
				int cht_start0 = cht->start0;
				int cht_end0 = cht->end0;
				if ( cht_start0 > cht_end0 ){	// 順番入換え
					cht_start0 = cht->end0;
					cht_end0 = cht->start0;
				}
				// 差分を求める！
				if ( cht_start == cht_end ){
					cht_start = cht_start0;
					cht_end = cht_end0;
				} else if ( cht_end0 == cht_start ){
					cht_start = cht_start0;
				} else if ( cht_start0 == cht_end ){
					cht_end = cht_end0;
				} else if ( cht_end != cht_end0 ){
					if ( cht_end > cht_end0 )
						cht_start = cht_end0;
					else {
						cht_start = cht_end;
						cht_end = cht_end0;
					}
				} else if ( cht_start != cht_start0 ){
					if ( cht_start < cht_start0 ){
						cht_end = cht_start0;
					} else {
						cht_end = cht_start;
						cht_start = cht_start0;
					}
				}
			}
			if ( cht_start == cht_end )
				cht_start = cht_end = -1;
			if ( cht_start != -1 ){
				ccLocSize = 2;
//				etct = 2;
				ccLoc = new LocList[ 2 ];
				ccLoc[0].loc = cht_start;
				ccLoc[0].index = 0;
				ccLoc[1].loc = cht_end;
				ccLoc[1].index = 1;
			}
		}
	} else
	if ( et && et->size()>0 && !(flags & (DF_CALCRECT|DF_UNREDRAW)) ){	// 計算 or 再描画無しでは無い場合
		ccLocSize = et->size()<<1;	// 再計算された強調文字列数
		ccLoc = new LocList[ccLocSize];		// chtによる反転とマージしない場合はこのままでよい
		if ( ccPoint )
			delete[] ccPoint;
		ccPoint = new int[ccLocSize];		// chtによる反転とマージしない場合はこのままでよい
		//Note:
		// et->size()<<1の値とEnphSort()後のccLocSizeはマージにより一致しない場合がある
		ccLocSize = EnphSort();
		if ( ccLocSize ) fCheckHit = true;
	}
	if ( ccPoint )
		memset( ccPoint, 0xFF, ccLocSize * sizeof(int) );
}

int EnphTextWork::GetNextLoc()
{
	if (ccLocIndex < ccLocSize){
		return ccLoc[ccLocIndex].loc;
	}
	return -1;	// no available loc.
}

#if 0	// to be deleted.
// str_locと一致する位置情報をccLocから探し、現在のX座標(cx)を設定する
// 2002.2.14
//	ccLoc[etct].loc == STR_DIFF(p,str)を<=に変更
//  理由：\r\n改行に対応したため、\r\nの間にccLoc[etct].locが
//        あったとき、==という条件のときにここが実行されないため。
//        改行処理を工夫すればあるかもしれないが・・・
void EnphTextWork::SetPoints(int str_loc, int cx)
{
	while (etct < _etnum && ccLoc[etct].loc <= str_loc){
		ccPoint[ ccLoc[etct].index ] = cx;
		//DBW("ccPoint[%d]=%d",ccLoc[etct].index,cx);
		//DBW("ccLoc[%d].loc=%d %d", etct, ccLoc[etct].loc, str_loc);
		etct++;
		ccnum++;
	}
}
#endif

void EnphTextWork::DrawReverse(HDC hdc, int right, bool cr, RECT *rc, int top, int revh, int cy)
{
	RECT irc;

	if ( ccPointNum ){
		for ( int l=ccLocIndex-ccPointNum;l<ccLocIndex; ){
			int index = ccLoc[l].index;
			int start = ccPoint[index&~1];
			int end = ccPoint[index|1];
			//DBW("start=%d end=%d", start, end);
			if ( start >= right ){	// ワードブレークのところにあった
				if ( cr && start == right ){	// 改行であった場合
					ccPoint[index&~1] = 0;	// 終端がこの行に無かったため
				}
				ccLocIndex = l;	// 行末ではなく行頭として扱う
				break;
			} else {
				int _end = ((end == -1 || end > right) ? right : end);
				SetRect( &irc, rc->left+start, top+revh, rc->left + _end, top + cy );
				InvertRect( hdc, &irc );
				ccPoint[index|1] = -2;	// 処理済マーク
				if ( end <= right && end != -1 ){
					// 反転表示はこの行で完結
					unredraw_start = -1;
					l = (l|1)+1;
				} else {
					// ワードブレークであった場合は終端が再び見つかるように
					ccPoint[index&~1] = 0;	// 終端がこの行に無かったため
					ccLocIndex = (l|1);
					unredraw_start = start;
					break;
				}
			}
		}
	} else {
		// 複数行対応
		if ( unredraw_start != -1 ){
			SetRect( &irc, rc->left, top+revh, rc->left + right, top + cy );
			InvertRect( hdc, &irc );
		}
	}
}

// 下線表示
//TODO: 色づけ処理は不要になったため、色の実際の処理は削除可能
//		ただし、linkのhit testがあるためロジックは必要
void EnphTextWork::DrawEnph(TNFONT &tnfont, TDispLineInfo &linfo, int right, RECT *rc, int top, int revh, int cy)
{
	if (!et || !et->size()){
		// AdjustEnph()でchtのみの場合はDrawEnph()は無関係
		// つまり、強調表示の場合のみ限定
		__assert__;
		return;
	}

	HDC hdc = tnfont;

	RECT irc;
	bool under_exist = false;
	bool color_exist = false;
	for ( int l=ccLocIndex-ccPointNum;l<ccLocIndex;l++ ){
		int index = ccLoc[l].index;
		int pos = ccPoint[index];
		//DBW("pos=%d", pos);
		const EnphText *eet = &(*et)[index>>1];
		if ( pos > right ){	// ワードブレークのところにあった
			ccLocIndex = l;
			if ( linfo.cr() && pos == right ){	// 改行であった場合
				ccPoint[index&~1] = 0;	// 終端がこの行に無かったため
			}
			break;
		} else {
			if ( eet->type == 0 )
				under_exist = true;	// under line
			else
				color_exist = true;	// color
			if ( ccLoc[l].index & 1 ){
				// underline/color end
				if ( eet->type == 0 ){
					// underline
					//DBW("left=%d under_start=%d pos=%d", rc->left, under_start, pos);
					SetRect( &irc, rc->left+under_start, top+revh, rc->left + pos, top + cy );
					InvertRect( hdc, &irc );
					under_start = -1;
				} else {
					// color
#if 0	// 実際の色処理はparser&drawが行う
					SetTextColor( hdc, color_old );
#endif
					color_start = -1;
				}
				if ( eet && eet->area ){
					// １行で終わっていた場合(under_rect.right==0)、cxを与える
					eet->area->rect.bottom = top+cy;
					if ( eet->area->rect.right == 0 ){
						// １行だけ
//						eet->area->rect.left += rc->left;
						eet->area->rect.right = rc->left+pos;
						eet->area->cy = 0;
					} else {
						// 複数行
						eet->area->start_x = eet->area->rect.left - rc->left;	// 4.13 1999.6.4 - rc->left追加
						eet->area->end_x = pos;
						eet->area->rect.left = rc->left;
						eet->area->rect.right += rc->left;
						eet->area->cy = cy;
					}
				}
			} else {
				// underline/color start
				if ( eet && eet->area ){
					// X座標は相対、Y座標は絶対
					eet->area->rect.left = rc->left+pos;
					eet->area->rect.top = top;
					eet->area->rect.right = 0;
					if ( eet->type == 0 )
						under_rect = &eet->area->rect;
					else
						color_rect = &eet->area->rect;
				} else {
					if ( eet->type == 0 )
						under_rect = &dummyrect;
					else
						color_rect = &dummyrect;
				}
				if ( eet->type == 0 ){
					under_start = pos;
				} else {
#if 0	// 実際の色処理はparser&drawが行う
					color_old = tnfont.SelectLinkColor( );
#endif
					color_start = pos;
				}
			}
		}
	}
	// １行で終わらなかった
	if ( under_start != -1 ){
		if (under_exist){
			SetRect( &irc, rc->left+under_start, top+revh, rc->left + right, top + cy );
			InvertRect( hdc, &irc );
			under_rect->right = right;
		} else {
			// 複数行対応
			SetRect( &irc, rc->left, top+revh, rc->left + right, top + cy );
			InvertRect( hdc, &irc );
			if ( under_rect->right < right )
				under_rect->right = right;
		}
		under_start = 0;
	}
	if ( color_start != -1 ){
		if (color_exist){
			color_rect->right = right;
		} else {
			// 複数行対応
			if ( color_rect->right < right )
				color_rect->right = right;
		}
		color_start = 0;
	}
}

void EnphTextWork::Next()
{
	ccPointNum = 0;
	if ( ccLocIndex >= ccLocSize ) fCheckHit = false;
}

TFontAttrMap::~TFontAttrMap()
{
	foreach (*this, it, TFontAttrMap_t){
		delete it->second;
	}
}


