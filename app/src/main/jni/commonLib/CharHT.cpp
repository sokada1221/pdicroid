#include <windows.h>
#pragma hdrstop
#include "CharHT.h"

bool THitArea::HitTest( POINT pt )
{
#if 0
	if ( cy == 0 ) start_x = end_x = 0;
	DBW("%d %d %d %d - %d %d - %d - %d %d", rect.left, rect.top, rect.right, rect.bottom, pt.x, pt.y, cy, start_x, end_x );
#endif
	if ( !PtInRect( &rect, pt ) ) return false;
	if ( cy ){
//		DBW("start=%d %d", start_x, end_x);
		if ( pt.y < rect.top + cy ){
			// 最初の行である
			if ( pt.x < rect.left+start_x ) return false;
		} else
		if ( pt.y >= rect.bottom - cy ){
			// 最終行である
			if ( pt.x >= rect.left+end_x ) return false;
		}
	}
	return true;
}

CharHT::CharHT( int _item, POINT *_pt )
{
	item = _item;
	if ( _pt )
		pt = *_pt;
	pos = -1;
	pos1 = pos2 = item1 = item2 = -1;
	fOver = CHT_NONE;
}

// htPivotを開始点としたヒットテストを行なう
bool CharHT::HitTest( CharHT &htPivot )
{
	if ( htPivot.item == -1 ){
		if ( htPivot.item2 != -1 && (item1 == htPivot.item2 || item == htPivot.item2) ){
			if ( item != htPivot.item2 ){
				item = item1;
				pos = pos1;
			}
			start = htPivot.pos2;
			end = pos;
			start0 = htPivot.pos2;
			end0 = htPivot.pos2;
			return true;
		}
		if ( htPivot.item1 != -1 && (item2 == htPivot.item1 || item == htPivot.item1) ){
			if ( item != htPivot.item1 ){
				item = item2;
				pos = pos2;
			}
			start = htPivot.pos1;
			end = pos;
			start0 = htPivot.pos1;
			end0 = htPivot.pos1;
			return true;
		}
		if ( item1 != -1 && item1 == htPivot.item1 ){
			if ( pos1 != htPivot.pos1 ){
				item = item1;
				pos = pos1;
				start = htPivot.pos1;
				end = pos1;
				start0 = htPivot.pos1;
				end0 = htPivot.pos1;
				return true;
			}
		}
		if ( item2 != -1 && item2 == htPivot.item2 ){
			if ( pos2 != htPivot.pos2 ){
				item = item2;
				pos = pos2;
				start = htPivot.pos2;
				end = pos2;
				start0 = htPivot.pos2;
				end0 = htPivot.pos2;
				return true;
			}
		}
	}
	return false;
}

// rvStart,rvEnd:前回の選択状況
bool CharHT::SetStartEnd( CharHT &htPivot, int rvStart, int rvEnd )
{
	if ( pos == -1 ){
		item = -1;
		if ( htPivot.item != -1 ){
			if ( htPivot.item == item1 ){
				item = htPivot.item;
				pos = pos1;
			} else if ( htPivot.item == item2 ){
				item = htPivot.item;
				pos = pos2;
			} else {
				// 変化なし
				pos = rvEnd;
			}
		} else {
			// 両方無効
			if ( !HitTest( htPivot ) ){
				start = start0 = rvStart;
				end0 = rvEnd;
				end = rvStart;
				return false;
			}
		}
	}

	start = start0 = rvStart;
	end0 = rvEnd;
	end = pos;
	return true;
}


