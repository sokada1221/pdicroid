//---------------------------------------------------------------------------

#ifndef Base64BufferH
#define Base64BufferH
//---------------------------------------------------------------------------

#include "databuf.h"

class TBase64DataBuffer : public TDataBuffer {
typedef TDataBuffer super;
public:
	TBase64DataBuffer(int size)
		:super(size){}
	TBase64DataBuffer(const char *data, int size)
		:super(data, size){}
};

class TBase64 {
protected:
	TBase64DataBuffer *InData;
	TBase64DataBuffer *OutData;
public:
	TBase64(const char *data, int size);
	virtual ~TBase64();
	virtual const char *data() const
	{
		return OutData->data();
	}
	virtual int size() const
	{
		return OutData->size();
	}
protected:
	virtual void Do() = 0;
};
class TBase64Encode : public TBase64 {
typedef TBase64 super;
public:
	TBase64Encode(const char *data, int size);
protected:
	virtual void Do();
};
class TBase64Decode : public TBase64 {
typedef TBase64 super;
public:
	TBase64Decode(const char *data, int size);
protected:
	virtual void Do();
};

#endif
