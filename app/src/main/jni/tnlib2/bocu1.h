/*
******************************************************************************
*
*   Copyright (C) 2002, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  bocu1.h
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002jan24
*   created by: Markus W. Scherer
*
*   This is the definition file for the sample implementation of BOCU-1,
*   a MIME-compatible Binary Ordered Compression for Unicode.
*/
/* Copyright (C) 2003, TaN for PDIC modification */
#ifndef __BOCU1_H__
#define __BOCU1_H__

/*
 * Standard ICU header.
 * - Includes inttypes.h or defines its types.
 * - Defines UChar for UTF-16 as an unsigned 16-bit type (wchar_t or uint16_t).
 * - Defines UTF* macros to handle reading and writing
 *   of in-process UTF-8/16 strings.
 */
//#include "unicode/utypes.h"

/* BOCU-1 constants and macros ---------------------------------------------- */

/*
 * BOCU-1 encodes the code points of a Unicode string as
 * a sequence of byte-encoded differences (slope detection),
 * preserving lexical order.
 *
 * Optimize the difference-taking for runs of Unicode text within
 * small scripts:
 *
 * Most small scripts are allocated within aligned 128-blocks of Unicode
 * code points. Lexical order is preserved if the "previous code point" state
 * is always moved into the middle of such a block.
 *
 * Additionally, "prev" is moved from anywhere in the Unihan and Hangul
 * areas into the middle of those areas.
 *
 * C0 control codes and space are encoded with their US-ASCII bytes.
 * "prev" is reset for C0 controls but not for space.
 */

/* initial value for "prev": middle of the ASCII range */
#define BOCU1_ASCII_PREV        0x40

/* bounding byte values for differences */
#define BOCU1_MIN               0x21
#define BOCU1_MIDDLE            0x90
#define BOCU1_MAX_LEAD          0xfe
#define BOCU1_MAX_TRAIL         0xff
#define BOCU1_RESET             0xff

/* number of lead bytes */
#define BOCU1_COUNT             (BOCU1_MAX_LEAD-BOCU1_MIN+1)

/* adjust trail byte counts for the use of some C0 control byte values */
#define BOCU1_TRAIL_CONTROLS_COUNT  20
#define BOCU1_TRAIL_BYTE_OFFSET     (BOCU1_MIN-BOCU1_TRAIL_CONTROLS_COUNT)

/* number of trail bytes */
#define BOCU1_TRAIL_COUNT       ((BOCU1_MAX_TRAIL-BOCU1_MIN+1)+BOCU1_TRAIL_CONTROLS_COUNT)

/*
 * number of positive and negative single-byte codes
 * (counting 0==BOCU1_MIDDLE among the positive ones)
 */
#define BOCU1_SINGLE            64

/* number of lead bytes for positive and negative 2/3/4-byte sequences */
#define BOCU1_LEAD_2            43
#define BOCU1_LEAD_3            3
#define BOCU1_LEAD_4            1

/* The difference value range for single-byters. */
#define BOCU1_REACH_POS_1   (BOCU1_SINGLE-1)
#define BOCU1_REACH_NEG_1   (-BOCU1_SINGLE)

/* The difference value range for double-byters. */
#define BOCU1_REACH_POS_2   (BOCU1_REACH_POS_1+BOCU1_LEAD_2*BOCU1_TRAIL_COUNT)
#define BOCU1_REACH_NEG_2   (BOCU1_REACH_NEG_1-BOCU1_LEAD_2*BOCU1_TRAIL_COUNT)

/* The difference value range for 3-byters. */
#define BOCU1_REACH_POS_3   \
    (BOCU1_REACH_POS_2+BOCU1_LEAD_3*BOCU1_TRAIL_COUNT*BOCU1_TRAIL_COUNT)

#define BOCU1_REACH_NEG_3   (BOCU1_REACH_NEG_2-BOCU1_LEAD_3*BOCU1_TRAIL_COUNT*BOCU1_TRAIL_COUNT)

/* The lead byte start values. */
#define BOCU1_START_POS_2   (BOCU1_MIDDLE+BOCU1_REACH_POS_1+1)
#define BOCU1_START_POS_3   (BOCU1_START_POS_2+BOCU1_LEAD_2)
#define BOCU1_START_POS_4   (BOCU1_START_POS_3+BOCU1_LEAD_3)
     /* ==BOCU1_MAX_LEAD */

#define BOCU1_START_NEG_2   (BOCU1_MIDDLE+BOCU1_REACH_NEG_1)
#define BOCU1_START_NEG_3   (BOCU1_START_NEG_2-BOCU1_LEAD_2)
#define BOCU1_START_NEG_4   (BOCU1_START_NEG_3-BOCU1_LEAD_3)
     /* ==BOCU1_MIN+1 */

/* The length of a byte sequence, according to the lead byte (!=BOCU1_RESET). */
#define BOCU1_LENGTH_FROM_LEAD(lead) \
    ((BOCU1_START_NEG_2<=(lead) && (lead)<BOCU1_START_POS_2) ? 1 : \
     (BOCU1_START_NEG_3<=(lead) && (lead)<BOCU1_START_POS_3) ? 2 : \
     (BOCU1_START_NEG_4<=(lead) && (lead)<BOCU1_START_POS_4) ? 3 : 4)

/* The length of a byte sequence, according to its packed form. */
#define BOCU1_LENGTH_FROM_PACKED(packed) \
    ((uint32_t)(packed)<0x04000000 ? (packed)>>24 : 4)

/*
 * 12 commonly used C0 control codes (and space) are only used to encode
 * themselves directly,
 * which makes BOCU-1 MIME-usable and reasonably safe for
 * ASCII-oriented software.
 *
 * These controls are
 *  0   NUL
 *
 *  7   BEL
 *  8   BS
 *
 *  9   TAB
 *  a   LF
 *  b   VT
 *  c   FF
 *  d   CR
 *
 *  e   SO
 *  f   SI
 *
 * 1a   SUB
 * 1b   ESC
 *
 * The other 20 C0 controls are also encoded directly (to preserve order)
 * but are also used as trail bytes in difference encoding
 * (for better compression).
 */
#define BOCU1_TRAIL_TO_BYTE(t) ((t)>=BOCU1_TRAIL_CONTROLS_COUNT ? (t)+BOCU1_TRAIL_BYTE_OFFSET : bocu1TrailToByte[t])

/**
 * Integer division and modulo with negative numerators
 * yields negative modulo results and quotients that are one more than
 * what we need here.
 * This macro adjust the results so that the modulo-value m is always >=0.
 *
 * For positive n, the if() condition is always FALSE.
 *
 * @param n Number to be split into quotient and rest.
 *          Will be modified to contain the quotient.
 * @param d Divisor.
 * @param m Output variable for the rest (modulo result).
 */
#define NEGDIVMOD(n, d, m) { \
	(m)=(n)%(d); \
	(n)/=(d); \
	if((m)<0) { \
		--(n); \
		(m)+=(d); \
	} \
}

/* State for BOCU-1 decoder function. */
struct Bocu1Rx {
	long prev, count, diff;
};

/*------------------------------------------*/
/*		Prototypes							*/
/*------------------------------------------*/
typedef unsigned long (*FNPreCodeTranslate)(unsigned long);
unsigned char *bocu1Encode( const wchar_t *src, const wchar_t *end, unsigned char*dst, FNPreCodeTranslate fn=NULL);
int bocu1EncodeLength( const wchar_t *src, const wchar_t *end, unsigned char*dst );
wchar_t *bocu1Decode( const unsigned char **_src, const unsigned char *endp, int outmaxlength, wchar_t *outbuffer=NULL, int *outlen=NULL, FNPreCodeTranslate fn=NULL );
unsigned char *bocu1EncodeUTF8( const char *src, const char *end, unsigned char*dst, FNPreCodeTranslate=NULL );
char *bocu1DecodeUTF8( const unsigned char **_src, const unsigned char *endp, int outmaxlength, char *outbuffer=NULL, int *outlen=NULL, FNPreCodeTranslate=NULL );

#endif	// __BOCU1_H__

