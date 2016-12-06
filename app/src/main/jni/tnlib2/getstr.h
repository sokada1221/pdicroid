#ifndef __getstr_H
#define	__getstr_H

tchar *GetString( UINT msgno );

#ifdef __tnstr_h
void LoadStringArray(tnstr_vec &array, int id, const tchar *delim);
#endif

#endif	// __getstr_H
