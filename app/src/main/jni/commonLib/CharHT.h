#ifndef __CharHT_h
#define	__CharHT_h

#define	CHT_NONE	0
#define	CHT_ABOVE	1
#define	CHT_UNDER	2
#define	CHT_LEFT	3
#define	CHT_RIGHT	4

// 文字単位ヒットテスト構造体
struct CharHT {
	int curitem;	// (I)現在ヒットテスト中のitem(ヒットした項目をitem,item1,item2にセットする)
    				// DrawText2()関数で使用する.DrawText2()関数を呼ぶ前にセットしておく必要があるs
	int item;		// (O/I)ヒットテスト対象項目(-1にセットしてから呼んだ場合はヒットした項目)(SN_...)
				// DF_DISPのときは反転表示したい項目
	// ヒットテスト項目(DF_NODISPのときのみ有効）
	POINT pt;	// (I)マウスの位置(クライアント座標系）
	int pos;	// (O)ヒットした文字位置（ヒットした項目の何文字目であるかを示す）

	POINT cpos;		// (O) client座標系での左上座標を返す
					// 現在のところ、item >= SN_OBJECTである場合のみ返す(item=-1ではセットされない)

	// for HitTest
//	bool fOverRect;	// 領域をオーバしたものもヒットとするか？
	int fOver;		// 領域オーバのヒット(CHT_...)
	int pos1;		// fOverがCHT_ABOVE,CHT_LEFTのときのヒット位置
	int pos2;		// fOverがCHT_RIGHT,CHT_UNDERのときのヒット位置
	int item1;
	int item2;

	// 表示関係
	int start;	// (I)反転表示にする開始文字位置(dispf=DF_DISPのときのみ）
	int end;	// (I)反転表示にする終了文字位置(dispf=DF_DISPのときのみ）
	int start0;	// (I)前回の反転表示(DF_UNREDRAW時のみ)
	int end0;

	CharHT( int _item, POINT *_pt=NULL );
	bool HitTest( CharHT &htPivot );
	bool SetStartEnd( CharHT &htPivot, int rvStart, int rvEnd );
};

// hit area 情報
struct THitArea {
	RECT rect;
	int cy;
	int start_x;
	int end_x;
    bool HitTest( POINT pt );
};

#endif	/* __CharHT _h */


