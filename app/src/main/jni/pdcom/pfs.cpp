#include "tnlib.h"
#pragma	hdrstop
#include "pfs.h"

//BUG:
// C++Builder5にはバグがある。
// const tchar name[];
// という定義をする場合、宣言を
// extern const tchar name[];
// で行ったあとに定義を行わないとリンクエラーが発生する。（未定義エラー）
// よって、宣言のためのinclude "pfs.h"と定義のためのinclude "pfs.h"
// の二度のincludeを行う。

#undef __pfs_h
#undef DEF_PFS
#define	DEFINE_PFS
#include "pfs.h"

