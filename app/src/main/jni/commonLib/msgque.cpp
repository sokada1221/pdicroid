// TMesageQueue
// 標準的なmessage queueのclass
// Queueにstoreできるelementはvoid*(またはint)のみ。
//
// TSemMessageQueue
// Semaphore付きのTMessageQueue。
// Preemptive multitask OSで、queueをtask間で共有する場合に使用する。
#include <windows.h>
#pragma hdrstop
#include "osdep.h"
#include "msgque.h"

void DBW( const TCHAR *,...);
#define	nullchk(x)	if (!(x)){ DBW(TEXT("NULL error!!")); }
#define	errmsg	DBW

TMessageQueue::TMessageQueue( int num )
{
	Num = num;
	Queue = new void*[ Num ];
	TopIndex = LastIndex = 0;
	Count = 0;
}
TMessageQueue::~TMessageQueue()
{
	if ( Queue )
		delete[] Queue;
}
bool TMessageQueue::Put( void *obj )
{
	if ( Count == Num ){
		if ( IsFull() ){
			return false;	// queue full
		}
	}
	Queue[TopIndex] = obj;
	if ( ++TopIndex == Num )
		TopIndex = 0;
	Count++;
	return true;
}
// 最優先でputする
bool TMessageQueue::PriorityPut( void *obj )
{
	if ( Count == Num ){
		if ( IsFull() ){
			return false;
		}
	}
	if ( LastIndex == 0 )
		LastIndex = Num-1;
	else
		LastIndex--;
	Queue[LastIndex] = obj;
	Count++;
	return true;
}
bool TMessageQueue::Get( void **obj )
{
	if ( Count == 0 ) return false;
	*obj = Queue[LastIndex];
	if ( ++LastIndex == Num )
		LastIndex = 0;
    Count--;
    return true;
}
// obj : can be NULL if not need
bool TMessageQueue::Peek( void **obj )
{
	if ( Count == 0 ) return false;
	if ( obj )
		*obj = Queue[LastIndex];
	return true;
}
#ifdef _Windows
TSemMessageQueue::TSemMessageQueue( int num )
	:TMessageQueue( num )
{
	CREATE_EMPTY_SEM( sem );
	nullchk( sem );
	CREATE_SEM( block );
}
TSemMessageQueue::~TSemMessageQueue( )
{
	DELETE_SEM( sem );
	DELETE_SEM( block );
}
bool TSemMessageQueue::Put( void *obj )
{
	TAKE_SEM( block, WAIT_INFINITE );
	bool r = inherited::Put( obj );
	GIVE_SEM( block );

	GIVE_SEM( sem );
	return r;
}
bool TSemMessageQueue::WaitPut( void *obj, int msec )
{
	if ( msec != INFINITE )
		msec = (msec+9) / 10;
	for ( int i=0;msec == INFINITE || i<=msec;i++ ){
		if ( Put(obj) ){
			return true;
		}
		SLEEP(10);
	}
	errmsg(TEXT("TSemMessageQueue::WaitPut() - error : %d\n"),msec);

	return false;
}
bool TSemMessageQueue::PriorityPut( void *obj )
{
	TAKE_SEM( block, WAIT_INFINITE );
	bool r = inherited::PriorityPut( obj );
	GIVE_SEM( block );

	GIVE_SEM( sem );
	return r;
}
bool TSemMessageQueue::WaitPriorityPut( void *obj, int msec )
{
	if ( msec != INFINITE )
		msec = (msec+9) / 10;
	for ( int i=0;msec == INFINITE || i<=msec;i++ ){
		if ( PriorityPut(obj) ){
			return true;
		}
		SLEEP(10);
	}
	errmsg(TEXT("TSemMessageQueue::WaitPriorityPut() - timeout\n"));

	return false;
}
bool TSemMessageQueue::Get( void **obj )
{
	if ( Count == 0 )
		return false;

	TAKE_SEM( block, WAIT_INFINITE );
	inherited::Get( obj );
	GIVE_SEM( block );

	if ( Count == 0 )
		TAKE_SEM( sem, WAIT_INFINITE );

	return true;
}
bool TSemMessageQueue::WaitGet( void **obj, int msec )
{
	if ( !TAKE_SEM( sem, msec ) ){
		return false;
	}

	TAKE_SEM( block, WAIT_INFINITE );
	bool r = inherited::Get( obj );
	GIVE_SEM( block );

	if ( Count > 0 )
		GIVE_SEM( sem );
	return r;
}

#endif

TSemVarMessageQueue::TSemVarMessageQueue( int slotsize )
	:inherited( slotsize )
{
	SlotSize = slotsize;
	SlotNum = 1;
}
// queueを拡大する
bool TSemVarMessageQueue::IsFull()
{
	void **newque = new void*[SlotSize*(SlotNum+1)];
	if ( !newque ){
		return true;
	}
	// TopIndex == LastIndexであることが条件 //
	memcpy( newque, &Queue[LastIndex], sizeof(void*)*(Num-LastIndex) );	// 前半部分
	memcpy( &newque[Num-LastIndex], Queue, sizeof(void*)*(LastIndex) );	// 後半部分
	delete[] Queue;
	TopIndex = Num;
	LastIndex = 0;
	
	SlotNum++;
	Num = SlotSize * SlotNum;
	Queue = newque;
//	DBW("Num=%d TopIndex=%d LastIndex=%d", Num,TopIndex,LastIndex);
	return false;
}

