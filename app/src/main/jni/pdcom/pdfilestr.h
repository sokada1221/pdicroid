#ifndef __pdfilestr_h
#define	__pdfilestr_h

bool FindFileForLink( tnstr &fullpath, const tchar *path, const tchar *filename, const tchar *dicpath );
bool GetRelPathForLink( tnstr &relpath, const tchar *path, const tchar *filename, const tchar *dicpath, bool inc_drive );

#endif	/* __pdfilestr_h */
