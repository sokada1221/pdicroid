//---------------------------------------------------------------------------

#ifndef MemObjH
#define MemObjH
//---------------------------------------------------------------------------

class TMemoryObject {
protected:
	char *data;
	int datalen;
	TMemoryObject *dataref;
	int refcnt;
	bool byref;
public:
	TMemoryObject();
	TMemoryObject(void *data, int datalen, bool byref=false);
	TMemoryObject(TMemoryObject *obj);
protected:
	virtual ~TMemoryObject();
public:
	void ref()
		{ refcnt++; }
	void deref();
	char *get() const
		{ return dataref ? dataref->get() : data; }
	int size() const
		{ return dataref ? dataref->getlen() : datalen; }
	int getlen() const
		{ return size(); }
	void set(void *data, int datalen);
	int getref() const
		{ return refcnt; }
	TMemoryObject &operator = (TMemoryObject &obj);
};

#endif
