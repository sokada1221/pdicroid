#include <windows.h>
#pragma	hdrstop
#include "dib.h"

/*************************************************************************
 *
 * PaintBitmap()
 *
 * Parameters:
 *
 * HDC hDC          - DC to do output to
 *
 * LPRECT lpDCRect  - rectangle on DC to do output to
 *
 * HBITMAP hDDB     - handle to device-dependent bitmap (DDB)
 *
 * LPRECT lpDDBRect - rectangle of DDB to output into lpDCRect
 *
 * HPALETTE hPalette - handle to the palette to use with hDDB
 *
 * Return Value:
 *
 * BOOL             - TRUE if bitmap was drawn, FLASE otherwise
 *
 * Description:
 *
 * Painting routine for a DDB.  Calls BitBlt() or
 * StretchBlt() to paint the DDB.  The DDB is
 * output to the specified DC, at the coordinates given
 * in lpDCRect.  The area of the DDB to be output is
 * given by lpDDBRect.  The specified palette is used.
 *
 * NOTE: This function always selects the palette as background. Before
 * calling this function, be sure your palette is selected to desired
 * priority (foreground or background).
 *
 * History:
 *
 *   Date      Author               Reason
 *   6/1/91    Garrett McAuliffe    Created
 *   12/12/91  Patrick Schreiber    Added return value, realizepalette,
 *                                       header and some comments
 *   6/8/92    Patrick Schreiber    Select palette as background always, added
 *                                  NOTE above.
 *
 ************************************************************************/

BOOL FAR PaintBitmap(HDC      hDC,
					 LPRECT   lpDCRect,
					 HBITMAP  hDDB,
					 LPRECT   lpDDBRect,
					 HPALETTE hPal)
{
   HDC      hMemDC;            // Handle to memory DC
   HBITMAP  hOldBitmap;        // Handle to previous bitmap
   HPALETTE hOldPal1 = NULL;   // Handle to previous palette
   HPALETTE hOldPal2 = NULL;   // Handle to previous palette

   /* Create a memory DC */
   hMemDC = CreateCompatibleDC (hDC);

   /* If this failed, return FALSE */
   if (!hMemDC)
	  return FALSE;

   /* If we have a palette, select and realize it */
   if (hPal)
   {
	  hOldPal1 = SelectPalette(hMemDC, hPal, TRUE);
	  hOldPal2 = SelectPalette(hDC, hPal, TRUE);
	  RealizePalette(hDC);
   }

   /* Select bitmap into the memory DC */
   hOldBitmap = (HBITMAP)SelectObject (hMemDC, hDDB);

#ifndef WINCE
   /* Make sure to use the stretching mode best for color pictures */
   SetStretchBltMode (hDC, COLORONCOLOR);
#endif

   /* Determine whether to call StretchBlt() or BitBlt() */
   BOOL     bSuccess;  // Success/fail flag
   if ((RECTWIDTH(lpDCRect)  == RECTWIDTH(lpDDBRect)) &&
	   (RECTHEIGHT(lpDCRect) == RECTHEIGHT(lpDDBRect)))
	  bSuccess = BitBlt(hDC,
						lpDCRect->left,
						lpDCRect->top,
						lpDCRect->right - lpDCRect->left,
						lpDCRect->bottom - lpDCRect->top,
						hMemDC,
						lpDDBRect->left,
						lpDDBRect->top,
						SRCCOPY);
   else
	  bSuccess = StretchBlt(hDC,
							lpDCRect->left,
							lpDCRect->top,
							lpDCRect->right - lpDCRect->left,
							lpDCRect->bottom - lpDCRect->top,
							hMemDC,
							lpDDBRect->left,
							lpDDBRect->top,
							lpDDBRect->right - lpDDBRect->left,
							lpDDBRect->bottom - lpDDBRect->top,
							SRCCOPY);

   /* Clean up */
   SelectObject(hMemDC, hOldBitmap);

   if (hOldPal1)
	  SelectPalette (hMemDC, hOldPal1, FALSE);

   if (hOldPal2)
	  SelectPalette (hDC, hOldPal2, FALSE);

   DeleteDC (hMemDC);

   /* Return with success/fail flag */
   return bSuccess;
}

#ifdef WINCE
#pragma	message("BitBlt is not supported yet")
#else

// ‚‘¬”ÅII
void BitBltDIB( HDC hdc, RECT *dcRect, RECT *dibRect, HDIB hDib )
{
#ifdef WINCE
	BITMAPINFOHEADER *lpbi = (BITMAPINFOHEADER*)LocalLock( hDib );
#else
	BITMAPINFOHEADER *lpbi = (BITMAPINFOHEADER*)GlobalLock( hDib );
#endif
	RECT rc;
	if ( dibRect ){
		rc = *dibRect;
	} else {
		rc.left = rc.top = 0;
		rc.right = (int)lpbi->biWidth;
		rc.bottom = (int)lpbi->biHeight;
	}
	SetDIBitsToDevice( hdc,
		dcRect->left, dcRect->top,
		lpbi->biWidth, lpbi->biHeight,
		rc.left, rc.top,
		0, lpbi->biHeight,
		FindDIBBits( lpbi ), (BITMAPINFO*)lpbi, DIB_RGB_COLORS );
#ifdef WINCE
	LocalUnlock( hDib );
#else
	GlobalUnlock( hDib );
#endif
}

void StretchBltDIB( HDC hdc, RECT *dcRect, RECT *dibRect, HDIB hDib )
{
#ifdef WINCE
	BITMAPINFOHEADER *lpbi = (BITMAPINFOHEADER*)LocalLock( hDib );
#else
	BITMAPINFOHEADER *lpbi = (BITMAPINFOHEADER*)GlobalLock( hDib );
#endif
	StretchDIBits( hdc,
		dcRect->left, dcRect->top, RECTWIDTH( dcRect ), RECTHEIGHT( dcRect ),
		dibRect->left, lpbi->biHeight, RECTWIDTH( dibRect ), RECTHEIGHT( dibRect ),
		FindDIBBits( lpbi ), (BITMAPINFO*)lpbi, DIB_RGB_COLORS, SRCCOPY );
#ifdef WINCE
	LocalUnlock( hDib );
#else
	GlobalUnlock( hDib );
#endif
}

#endif
