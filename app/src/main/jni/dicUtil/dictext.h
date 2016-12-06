#ifndef __DICTEXT_H
#define	__DICTEXT_H

int PutMultiLine( TOFile &tof, const tchar *p );
void PutMultiLine( tchar *buf, const tchar *p );
int PutText( TOFile*file, class Japa &japa, BOOL fExtPdicText );
#ifdef PDICW
void GetMultiPdicText( tchar *line, Japa &japa, BOOL fSeparate, BOOL fReplaceCRLF=TRUE );	// fReplaceCRLF=改行( \ )を認識するか？
#else
void GetMultiPdicText( tchar *line, Japa &japa, BOOL fSeparate, BOOL fReplaceCRLF=FALSE );	// fReplaceCRLF=改行( \ )を認識するか？
#endif

extern const tchar *EXTCRCHAR;	// 拡張PDICテキスト形式の改行代替文字
#define	L_EXTCRCHAR	3
extern tnstr StrOneLineDelim;	// " /// "
extern tnstr StrDelimiter;		// ","
#define	MAX_ONELINEDELIM	10

extern const tchar *StrCR;
extern const tchar *StrLF;
extern const tchar *StrCRLF;		// \r\n
extern const tchar *StrExpSepa;	// " / "
#define	L_StrExpSepa	3

#define	CharLF	'\n'
#define	CharCR	'\r'
#define	CharTAB	'\t'

#endif	// __DICTEXT_H
