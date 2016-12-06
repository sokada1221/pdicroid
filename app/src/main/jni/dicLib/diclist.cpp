#include "tnlib.h"
#pragma hdrstop
#include "diclist.h"

#if MAX_MULTIDIC <= 30
#else
_diclist_t dl_error_obj(0, true);
_diclist_t dl_all_obj;
_diclist_t dl_empty_obj(0);
#endif
