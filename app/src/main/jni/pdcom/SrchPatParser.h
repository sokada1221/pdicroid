#ifndef __SrchPatParser_h
#define	__SrchPatParser_h

#include "jtype.h"

bool SearchPatternParser(const tchar *str, tnstr_vec &pats, const tchar **next);
void split_words(const tchar *str, tnstr_vec &words);

#endif	/* __SrchPatParser_h */
