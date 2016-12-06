#include "tnlib.h"
#pragma hdrstop
#include "defs_android.h"

COLORREF SetTextColor(
  HDC hdc,           // デバイスコンテキストのハンドル
  COLORREF crColor   // テキストの色
)
{
	return 0;
}
COLORREF SetBkColor(
  HDC hdc,           // デバイスコンテキストのハンドル
  COLORREF crColor   // テキストの色
)
{
	return 0;
}
DWORD GetSysColor(
  int nIndex   // 表示要素
)
{
	return 0;
}
HGDIOBJ GetStockObject(
  int fnObject   // ストックオブジェクトのタイプ
)
{
	return 0;
}
HGDIOBJ SelectObject(
  HDC hdc,          // デバイスコンテキストのハンドル
  HGDIOBJ hgdiobj   // オブジェクトのハンドル
)
{
	return 0;
}
int GetLastError()
{
	return errno;
}
