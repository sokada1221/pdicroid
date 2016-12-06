//---------------------------------------------------------------------------

#ifndef draw4parserH
#define draw4parserH
//---------------------------------------------------------------------------

#include "draw4com.h"

void ParseText(TNFONT &org_tnfont, TDispLinesInfo &linesinfo, const tchar *str, RECT *rc, TDrawSetting &setting, TFontAttrMap &FontAttrs, class THyperLinks *hls=NULL);
void ParseHtmlHitPosition( TNFONT &tnfont, const tchar *str, RECT *rc, THyperLinks &hls);

#endif
