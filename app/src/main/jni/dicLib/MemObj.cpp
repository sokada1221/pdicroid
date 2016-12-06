//---------------------------------------------------------------------------
#include <stdio.h>
#pragma hdrstop

#include "MemObj.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

TMemoryObject::TMemoryObject()
{
	data = NULL;
	datalen = 0;
	dataref = NULL;
	refcnt = 1;
	byref = false;
}
TMemoryObject::TMemoryObject(void *_data, int _datalen, bool _byref)
{
	data = (char*)_data;
	datalen = _datalen;
	dataref = NULL;
	refcnt = 1;
	byref = _byref;
}
TMemoryObject::TMemoryObject(TMemoryObject *obj)
{
	data = NULL;
	datalen = 0;
	dataref = obj;
	byref = false;
	obj->ref();
}
TMemoryObject::~TMemoryObject()
{
	if (dataref){
		dataref->deref();
	} else
	if (data && !byref)
		delete[] data;
}
void TMemoryObject::deref()
{
	refcnt--;
	if (refcnt==0)
		delete this;
}
void TMemoryObject::set(void *_data, int _datalen)
{
	if (data && !byref)
		delete[] data;
	data = (char*)_data;
	datalen = _datalen;
	byref = false;
	if (dataref){
		dataref->deref();
		dataref = NULL;
	}
}
TMemoryObject &TMemoryObject::operator = (TMemoryObject &obj)
{
	if (data){
		if (!byref)
			delete[] data;
		data = NULL;
	}
	dataref = &obj;
	obj.ref();
	return *this;
}
