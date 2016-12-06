#ifndef __MSGQUE_H
#define	__MSGQUE_H

// Message Queue //
class TMessageQueue {
protected:
	void **Queue;
	int TopIndex;
	int LastIndex;
	int Num;			// The maximum number of queue messages
	int Count;			// The number of queued messages 
public:
	TMessageQueue( int num );
    virtual ~TMessageQueue();
    virtual bool Put( void *obj );
	bool PriorityPut( void *obj );
	virtual bool Get( void **obj );
	bool Peek( void **obj );
	int GetCount() const
		{ return Count; }
	int GetTopIndex() const
		{ return TopIndex; }
	int GetLastIndex() const
		{ return LastIndex; }
	void *operator [] (int index)
		{ return Queue[index]; }
protected:
	virtual bool IsFull(){return true;}	// var message queue‚Ì‚½‚ß‚Ìhook
};
// Message Queue with semaphore when preemptive OS //
class TSemMessageQueue : public TMessageQueue {
typedef TMessageQueue inherited;
protected:
	DEF_SEM( sem );
	DEF_BSEM( block );
public:
	TSemMessageQueue( int num );
	~TSemMessageQueue( );
	virtual bool Put( void *obj );
	bool Put( int obj )
		{ return Put( (void*)obj ); }
	bool WaitPut( void *obj, int msec );
	bool PriorityPut( void *obj );
	bool WaitPriorityPut( void *obj, int msec );
	virtual bool Get( void **obj );
	bool WaitGet( void **obj, int msec );
};

class TSemVarMessageQueue : public TSemMessageQueue {
typedef TSemMessageQueue inherited;
protected:
	int SlotSize;
	int SlotNum;
public:
	TSemVarMessageQueue( int slotsize );
protected:
	virtual bool IsFull();
};

#endif

