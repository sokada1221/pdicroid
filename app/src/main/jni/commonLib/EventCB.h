//---------------------------------------------------------------------------

#ifndef EventCBH
#define EventCBH
//---------------------------------------------------------------------------

typedef int (*FNEventCallback)(void *user, int param);

struct TEventCallbackItem {
	FNEventCallback Callback;
	void *User;
	// Link
	TEventCallbackItem *Prev;
	TEventCallbackItem *Next;
};

class TEventCallback {
protected:
	TEventCallbackItem *Top;
public:
	TEventCallback()
	{
		Top = NULL;
	}
	virtual ~TEventCallback()
	{
		Clear();
	}
	void Add(FNEventCallback fn, void *user)
	{
		TEventCallbackItem *p = new TEventCallbackItem;
		p->Callback = fn;
		p->User = user;
		if (Top){
			p->Next = Top;
			p->Prev = Top->Prev;
			Top->Prev = p;
			Top->Prev->Next = p;
			Top = p;
		} else {
			Top = p->Next = p->Prev = p;
		}
	}
	void Remove(FNEventCallback fn)
	{
		if (!Top)
			return;
		TEventCallbackItem *p = Top;
		do {
			if (p->Callback==fn){
				if (Top==p){
					if (p->Next==p){
						// only one item
						delete p;
						Top = NULL;
						break;
					}
					Top = p->Next;
				}
				p->Prev->Next = p->Next;
				p->Next->Prev = p->Prev;
				delete p;
				break;
			}
			p = p->Next;
		} while (p!=Top);
	}
	void HandleEvent(int param)
	{
		if (!Top)
			return;
		TEventCallbackItem *p = Top;
		do {
			p->Callback(p->User, param);
			p = p->Next;
		} while(p!=Top);
	}
	void Clear()
	{
		if (!Top)
			return;
		TEventCallbackItem *p = Top;
		do {
			TEventCallbackItem *next = p->Next;
			delete p;
			p = next;
		} while (p!=Top);
		Top = NULL;
	}
};

#endif
