#include "stdafx.h"
#include "tndefs.h"
#define	BOCU_UTF16
typedef wchar_t	bocu_t;
#define	DEF_NAME(name)	name
#define	bocu_len	wcslen
#include "bocu1-core.cpp"
