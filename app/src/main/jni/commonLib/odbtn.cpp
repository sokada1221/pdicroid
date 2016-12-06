// Owner draw button routine
#include "tnlib.h"
#pragma	hdrstop

enum {
	bsDisabled,
	bsDown,
	bsUp
};

void DrawItemButton( DRAWITEMSTRUCT &ds, COLORREF color )
{
	RECT R;
	GetClientRect( ds.hwndItem, &R );

	bool IsDown = ((ds.itemState & ODS_SELECTED) != 0);
	bool IsDefault = ((ds.itemState & ODS_FOCUS) != 0);
#if 0
	int State;
	if ( !IsWindowEnabled( ds.hwndItem ) ){
		State = bsDisabled;
	} else
	if ( IsDown )
		State = bsDown;
	else
		State = bsUp;
#endif

	int Flags = DFCS_BUTTONPUSH
#ifndef WINCE
		| DFCS_ADJUSTRECT
#endif
		;

	if ( IsDown ) Flags |= DFCS_PUSHED;
	if ( ds.itemState & ODS_DISABLED){
		Flags |= DFCS_INACTIVE;
	}

	HPEN hOldPen;
	HBRUSH hOldBrush;

	bool IsFocused = (GetFocus() == ds.hwndItem);


	// DrawFrameControl doesn't allow for drawing a button as the
	// default button, so it must be done here.
	if ( IsFocused || IsDefault )
	{
		hOldPen = (HPEN)SelectObject( ds.hDC, CreatePen( PS_SOLID, 1, GetSysColor( COLOR_WINDOWFRAME ) ) );
		hOldBrush = (HBRUSH)SelectObject( ds.hDC, GetStockObject( NULL_BRUSH ) );
		Rectangle( ds.hDC, R.left, R.top, R.right, R.bottom );
		// DrawFrameControl must draw within this border
		InflateRect( &R, -1, -1 );
		DeleteObject( SelectObject( ds.hDC, hOldPen ) );
		SelectObject( ds.hDC, hOldBrush );
	}


	// DrawFrameControl does not draw a pressed button correctly
	if ( IsDown ){
		hOldPen = (HPEN)SelectObject( ds.hDC, CreatePen( PS_SOLID, 1, GetSysColor( COLOR_BTNSHADOW ) ) );
		hOldBrush = (HBRUSH)SelectObject( ds.hDC, CreateSolidBrush( GetSysColor( COLOR_BTNFACE ) ) );
		Rectangle( ds.hDC, R.left, R.top, R.right, R.bottom );
		DeleteObject( SelectObject( ds.hDC, hOldPen ) );
		DeleteObject( SelectObject( ds.hDC, hOldBrush ) );

		InflateRect( &R, -1, -1 );
	} else {
		DrawFrameControl( ds.hDC, &R, DFC_BUTTON, Flags );
	}

	if ( IsFocused )
	{
		GetClientRect( ds.hwndItem, &R );
		InflateRect( &R, -1, -1 );
	}

//	FCanvas.Font := Self.Font;
	if ( IsDown )
		OffsetRect( &R, 1, 1 );

	// ŒÅ—L‚Ì•`‰æ
	GetClientRect( ds.hwndItem, &R );
	InflateRect( &R, -4, -4 );
	hOldPen = (HPEN)SelectObject( ds.hDC, GetStockObject( BLACK_PEN ) );
	hOldBrush = (HBRUSH)SelectObject( ds.hDC, CreateSolidBrush( color ) );
	Rectangle( ds.hDC, R.left, R.top, R.right, R.bottom );
	DeleteObject( SelectObject( ds.hDC, hOldPen ) );
	DeleteObject( SelectObject( ds.hDC, hOldBrush ) );

	if ( IsFocused )
	{
		GetClientRect( ds.hwndItem, &R );
		InflateRect( &R, -2, -2 );
//		FCanvas.Pen.Color := clWindowFrame;
//		FCanvas.Brush.Color := clBtnFace;
		DrawFocusRect( ds.hDC, &R );
	}
}


