#include "Base64Buffer.h"
#include "../commonLib/base64.h"

TBase64::TBase64(const char *data, int size)
{
	InData = new TBase64DataBuffer(data, size);
	OutData = NULL;
}
TBase64::~TBase64()
{
	delete InData;
	delete OutData;
}

TBase64Encode::TBase64Encode(const char *data, int size)
	:super(data, size)
{
	Do();
}

TBase64Decode::TBase64Decode(const char *data, int size)
	:super(data, size)
{
	Do();
}

void TBase64Encode::Do()
{
	OutData = new TBase64DataBuffer( base64enc_size(InData->size()) );
	base64enc(InData->data(), InData->size(), OutData->data());
}

void TBase64Decode::Do()
{
	OutData = new TBase64DataBuffer( base64enc_size(InData->size()) );
	base64dec(InData->data(), InData->size(), OutData->data());
}

