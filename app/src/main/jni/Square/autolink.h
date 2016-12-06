//---------------------------------------------------------------------------
#ifndef autolinkH
#define autolinkH
//---------------------------------------------------------------------------
extern "C" {
int WINAPI SearchAutoLinkText( const TCHAR *str, const char *prevstr, class THyperLinks *hls, bool SingleByte );
}
int SearchAutoLink( const TCHAR *str, THyperLinks &hls, int item, bool SingleByte, int maxnum=1024 );
bool FindFileLinkPath(class Pdic *dic, tnstr &fullpath, const tchar *filename, bool extra);
bool FindFileLinkPath(class MPdic *dic, tnstr &fullpath, const tchar *filename, int &i);
bool GetRelFileLinkPath(Pdic *dic, tnstr &relpath, const tchar *filename);

extern bool AutoLinkCtrlKey;

#endif

