#include "tnlib.h"
#pragma hdrstop
#include "pdstr.h"

const tchar StrAppName[] =
#ifdef LIGHT
	_T("Personal Dictionary Super Light");
#elif defined(WINCE)
#if MIXDIC
	_T("Personal Dictionary for Windows CE");
#else	// !MIXDIC
	_T("Personal Dictionary for CE/Unicode");
#endif	// !MIXDIC
#elif defined(UNICODE)
	_T("Personal Dictionary/Unicode");
#else
	_T("Personal Dictionary for Windows");
#endif

const tchar StrPDICShort[] = _T("PDIC");
const tchar StrPDICLong[] = _T("Personal Dictionary");

const tchar StrSpc1[] = _T(" ");
const tchar StrSpc2[] = _T("  ");

const tchar *StrDic = _T("*.dic");
const tchar *StrTxt = _T("*.txt");
const tchar *StrExe = _T("*.exe");
const tchar *StrAll = _T("*.*");
const tchar *StrCSV = _T("*.csv");
const tchar *StrDll = _T("*.dll");
const tchar *StrInf = _T("*.inf");
const tchar *StrWav = _T("*.wav");
const tchar *StrMp3 = _T("*.mp3");
const tchar *StrEPWing = _T("catalog*.*");
const tchar *StrIni = _T("*.ini");

const tchar *StrEXE = _T("EXE");
const tchar *StrCOM = _T("COM");
const tchar *StrBAT = _T("BAT");
const tchar *StrBMP = _T("BMP");
const tchar *StrDIB = _T("DIB");
const tchar *StrLZH = _T("LZH");

const tchar *StrNull = _T("");

// Default extension for Common Dialog //
const tchar *StrDefTxt = _t("txt");
const tchar *StrDefRtf = _t("rtf");

