#include "stdafx.h"
#include "tnlib.h"
#pragma hdrstop
#include "file.h"
#include "tfilebuf.h"
#include "utf.h"

int TTextFileBuffer::read(void *buf, int len)
{
	return ::_tread(fd, buf, len);
}

int TTextFileBufferUTF8::read()
{
	int d;
	// UTF-8
#if !defined(__UTF8) && wchar_size==2
	if (surrogate){
		wchar_t rc = surrogate;
		surrogate = 0;
		return rc;
	}
#endif
	if (curp+4 >= Size){	// 4 is the maximum length of the UTF-8.
		int r = Size-curp;	// unread bytes.
		if (r>0){
			memmove(Buffer, Buffer+curp, r);
		}
		if ((d = super::read(Buffer+r, Size-r)) <= 0){
			if (curp>=Size){
				return -1;
			} else {
				goto jmp1;	// ３バイト以下のUTF8に違いない
			}
		}
		d += r;
		if (d < Size){
			curp = Size - d;
			memmove(Buffer+curp, Buffer, d);
		} else {
			curp = 0;
		}
	}
jmp1:;
#if defined(__UTF8)
	return Buffer[curp++];
#else
#if wchar_size==2
	// UTF8 -> UTF16
	const char *next;
	unsigned long uc = UTF8toUTF16((const char*)Buffer+curp, &next);
	// uc includes surrogate
	if (uc==0){
		if (curp>=BUFFSIZE)
			return -1;	// end of file
		curp++;
	} else {
		curp = (int)((char*)next-(char*)Buffer);
	}
	if (uc>=0x10000){
		surrogate = uc&0xFFFF;
		return (wchar_t)(uc>>16);
	}
	return uc;
#else
	// UTF8 -> UTF32
	const char *next;
	wchar_t uc = UTF8toUTF32((const char*)Buffer+curp, &next);
	if (uc==0){
		if (curp>=BUFFSIZE)
			return -1;	// end of file
		curp++;
	} else{
		curp = (int)((char*)next-(char*)Buffer);
	}
	return uc;
#endif
#endif
}

void TTextFileBufferUTF16BE::changeEndian()
{
	// endian change
	utf16_t *p = Buffer+curp;
	utf16_t *ep = Buffer+Size;
	for (;p<ep;){
		*p = (p[0]<<8) | (p[0]>>8);
		p++;
	}
}

int TTextFileBufferCC::read()
{
#if 1
	//TODO: surrogateは動くかどうか不明
	if (curp >= Size){
		int r;
		if (readbuf_len<readbuf_size){
			if (readbuf_len>0){
				memmove(readbuf, readbuf+readbuf_cur, readbuf_len);
			}
			int len = (readbuf_size-readbuf_len)/4;	// 4はUTF8の最大長
			if (len<1) len = readbuf_size-readbuf_len;
			if ((r = (super::read(readbuf+readbuf_len, len))) < 0)
				return -1;	// error
			readbuf_len += r;
			readbuf_cur = 0;
		}
		if (readbuf_len==0){
			return -1;	// end of file
		}

		int srclen = readbuf_len;
		r = cc->Convert(cp_src, CP_UTF16LE, (const char*)readbuf+readbuf_cur, &srclen, Buffer, Size);

		readbuf_len -= srclen;
		readbuf_cur += srclen;

		if (r < Size){
			curp = Size - r;
			memmove(Buffer+curp, Buffer, r*sizeof(wchar_t));
		} else {
			curp = 0;
		}
	}
	return Buffer[curp++];
#elif 0
	int d;
	if (surrogate){
		wchar_t rc = surrogate;
		surrogate = 0;
		return rc;
	}
	const int maxlen = 4;	// 4 is the maximum length of the UTF-8. (if no larger char code exists)
	if (curp+maxlen >= Size){
		int r = Size-curp;	// unread bytes.
		if (r>0){
			memmove(Buffer, Buffer+curp, r);
		}
		if ((d = super::read(Buffer+r, Size-r)) <= 0){
			if (curp>=Size){
				return -1;
			} else {
				goto jmp1;	// ３バイト以下のUTF8に違いない
			}
		}
		d += r;
		if (d < Size){
			curp = Size - d;
			memmove(Buffer+curp, Buffer, d);
		} else {
			curp = 0;
		}
	}
jmp1:;
	//TODO: 一気にConvert()でBuffer分変換したほうが速いかな。。。
	// any code -> UTF16
	const char *next;
	unsigned long uc = cc->Convert(cp_src, CP_UTF16LE, (const char*)Buffer+curp, Size-curp, &next);
	// uc includes surrogate
	curp = (int)((char*)next-(char*)Buffer);
	if (uc==0 && curp>=BUFFSIZE){
		return -1;	// end of file
	}
	if (uc>=0x10000){
		surrogate = uc&0xFFFF;
		return (wchar_t)(uc>>16);
	}
	return uc;
#else
	int d;
	if ( curp >= Size ){
//		cc->Convert();
		if ((d = (super::read(Buffer, Size*sizeof(wchar_t)))) <= 0)
			return -1;
		d = d/sizeof(wchar_t);
		if (d < Size){
			curp = Size - d;
			memmove(Buffer+curp, Buffer, d*sizeof(wchar_t));
		} else {
			curp = 0;
		}
//		changeEndian();
	}
	return Buffer[curp++];
#endif
}
