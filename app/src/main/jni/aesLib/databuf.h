//---------------------------------------------------------------------------

#ifndef databufH
#define databufH
//---------------------------------------------------------------------------

#include <string.h>

class TDataBuffer {
protected:
	char *Buffer;
	size_t Size;
public:
	TDataBuffer();
	TDataBuffer(size_t size);
	TDataBuffer(const char *data, size_t size);
	TDataBuffer(TDataBuffer &o);
	virtual ~TDataBuffer();
	virtual void assign(const char *data, size_t len);
	virtual void clear();
	virtual char *data() const;
	virtual int size() const;
    virtual void setSize(size_t newsize);
};

#endif

