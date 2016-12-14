// draw4 engine

#ifndef __draw4_h
#define	__draw4_h

#include "draw4com.h"
#include "draw4parser.h"

//TODO:
// 必要な機能
// ・通常の描画
// ・選択用反転表示(DF_REVERSE)
// ・選択用反転表示の高速表示(DF_UNREDRAW)
// ・表示領域の計算（非表示）
// ・ヒットテスト
// ・Color,Bold,Italic,Reverse,Underline,(Sizeは必要か？)

// Functions for compatibilities //
class TDrawSetting &GetDefDrawSetting();
int GetTabCol();
void SetTabCol( int tabcol );
int GetDefReverseSize();
void SetDefReverseSize(int size);
void SetRevHeight( int ratio );
void SetInitLeft( int initleft );
//int GetInitLeft();	// GetLastRight() - rc->leftで代用すること
int GetLastRight( );
int GetLastTop();
int GetDrawMaxRight();
int GetDrawCYMax();
int DrawText2( TNFONT &tnfont, const TCHAR *str, RECT *rc, int swidth, UINT flags, struct CharHT *cht=NULL, const EnphTextVec *et =NULL, THyperLinks *hls=NULL );
int DrawText(TNFONT &tnfont, const tchar *str, RECT *rc, int swidth, UINT flags, CharHT *cht, const EnphTextVec *et=NULL, THyperLinks *hls=NULL);
int DrawTextUCP( TNFONT &tnfont, const tchar *str, RECT *rc, int swidth, UINT flags, CharHT *cht=NULL, const EnphTextVec *et=NULL );
int DrawText( TNFONT &tnfont, const tchar *str, RECT *rc, int swidth, UINT flags, CharHT *cht, const EnphTextVec *et, bool updatecp, THyperLinks *hls=NULL );

// New style functions //
void DrawText(TNFONT &tnfont, TDispLinesInfo &linesinfo, const tchar *str, RECT *rc, int swidth, UINT flags, CharHT *cht, const EnphTextVec *et, TDrawSetting &setting);
void HitTest( TNFONT &tnfont, TDispLinesInfo &linesinfo, const tchar *str, RECT *rc, UINT flags, CharHT *cht, const EnphTextVec *et, TDrawSetting &setting);
int HitTestX(TNFONT &tnfont, TDispLineInfo &linfo, const tchar *str, const RECT &rc, UINT flags, CharHT &cht, const TDrawSetting &setting);
void HitTestX(TNFONT &tnfont, TDispLineInfo &linfo, const tchar *str, const RECT &rc, UINT flags, EnphTextWork &etw, const TDrawSetting &setting);

//extern int DEFAULT_REVERSE_SIZE;

#endif	/* __draw4_h */

