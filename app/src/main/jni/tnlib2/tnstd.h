#ifndef __tnstd_h
#define	__tnstd_h

// Standard include without OS depending.

#define	__NO_WINDOWS_H		// map/vector stl headers include windows.h

#include "tndefs.h"
#include "tnstr.h"
#include "file.h"
#include "tnassert.h"

#include "strlib.h"
#include "bocu1.h"

// ˆÈ‰ºg—p‚µ‚Ä‚¢‚È‚¯‚ê‚Îíœ
#include "charray.h"
#include "harray.h"
#include "que.h"
#include "buffer.h"
#include "bufarray.h"
#include "vbuffer.h"
// End of delete.

#ifdef _WINDOWS_
#error	// Cannot include 'windows.h' in this header.
// If you avoid the error, please include 'tnlib.h'.
#endif

#endif	/* __tnstd_h */
