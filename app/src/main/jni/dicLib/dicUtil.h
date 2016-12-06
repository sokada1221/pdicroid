#ifndef __DicUtil_h
#define	__DicUtil_h

class TLangProc;

void _split_word(const tchar *word, tnstr &cword, tnstr &kword);
void split_word(const tchar *word, tnstr *cword, tnstr *kword);
tnstr extract_cword(const tchar *word);
const tchar *find_cword_pos(const tchar *word);
const _kchar *find_cword_pos(const _kchar *word);
int comp_word(const tchar *word, const tchar *composit_word);
int comp_word(const _kchar *word, const _kchar *composit_word);

// Move to LangProc.h //
//tnstr join_word(const tchar *cword, const tchar *kword);
//inline bool is_composit_word(const tchar *word)
//	{ return find_cword_pos(word)!=word; }
//tnstr create_composit_word(TLangProc *proc, const tchar *word);
//tnstr create_kword(TLangProc *proc, const tchar *word);

#endif	/* __DicUtil_h */

