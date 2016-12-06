#include	<windows.h>
#pragma	hdrstop


void MoveCenter( RECT &r )
{
	int hoff = ( ( ( GetSystemMetrics( SM_CXSCREEN ) - ( r.right - r.left ) ) / 2 + 4 ) & ~7 ) - r.left;
	int voff = ( ( GetSystemMetrics( SM_CYSCREEN ) - ( r.bottom - r.top ) ) / 2 ) - r.top;
	OffsetRect( &r, hoff, voff );
}
