//---------------------------------------------------------------------------
#include <string.h>
#pragma hdrstop

#include "databuf.h"

//---------------------------------------------------------------------------

inline int min(int x, int y){ return x<y?x:y; }

TDataBuffer::TDataBuffer()
{
	Size = 0;
	Buffer = NULL;
}
TDataBuffer::TDataBuffer(size_t size)
{
	Size = size;
	Buffer = new char[size];
	memset(Buffer, 0, size);
}
TDataBuffer::TDataBuffer(const char *data, size_t size)
{
	Size = size;
	Buffer = new char[size];
	memcpy(Buffer, data, size);
}
TDataBuffer::TDataBuffer(TDataBuffer &o)
{
	Size = o.size();
	if (Size>0){
		Buffer = new char[Size];
		memcpy(Buffer, o.data(), Size);
	} else {
		Buffer = NULL;
	}
}
TDataBuffer::~TDataBuffer()
{
	if (Buffer)
		delete[] Buffer;
}
void TDataBuffer::assign(const char *data, size_t len)
{
	if (len==-1){
		len = strlen(data)+1;
	}
	if (!Buffer || Size<len){
		if (Buffer)
			delete[] Buffer;
		Buffer = new char[len];
	}
	Size = len;
	memcpy(Buffer, data, len);
}
void TDataBuffer::clear()
{
	if (!Buffer)
		return;
	memset(Buffer, 0, Size);
}
char *TDataBuffer::data() const
{
	return Buffer;
}
int TDataBuffer::size() const
{
	return (int)Size;
}
void TDataBuffer::setSize(size_t newsize)
{
	if (Size<newsize){
    	// expand buffer
        char *newbuf = new char[newsize];
        if (Buffer){
	        memcpy(newbuf, Buffer, Size);
            delete[] Buffer;
        }
        Buffer = newbuf;
    }
    Size = newsize;
}

