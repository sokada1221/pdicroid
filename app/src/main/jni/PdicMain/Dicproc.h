#ifndef __DICPROC_H
#define	__DICPROC_H

#include "windic.h"

int OpenDic( MPdic &dic, struct DICNAME &dicname );
bool FindDic(const tchar *filename, tnstr &fullpath);

void NotifyModifiedDic( const tchar *filename );
bool ChangeDic( const tchar *newgrpname, int oldsel );

HWND FindPdic();
HWND FindPdicWin32();
bool OpenPdic(HWND hwndTarget, const tchar *grpname);
HWND FindDicGroup(const tchar *grpname);

#endif	// __DICPROC_H

