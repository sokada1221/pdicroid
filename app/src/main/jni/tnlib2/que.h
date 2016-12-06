#ifndef	__QUE_H
#define	__QUE_H

//pop(),get()したオブジェクトが破壊されない
//残りのオブジェクトはこのクラスのオブジェクトが破壊されるときに全て破壊される
//また、キューバッファを越えた場合、古いオブジェクも破壊される
template <class T>
class ObjectQueT {
protected:
	T **que;
	int num;
	int putp;
	int getp;
	int size;
public:
	ObjectQueT( int _size );
	virtual ~ObjectQueT();
	bool is_full( ) const
		{ return num == size; }
	bool count() const { return num; }
	int get_num( ) const {return num;}
	void put( T *obj );	// queueとして使用する場合はget()を使用する
	T *get( );
	void clear( );
	//特殊な操作(スタックと同様に扱える)
	T *pop( );
};

template <class T>
ObjectQueT<T>::ObjectQueT( int _size )
{
	size = _size;
	que = new T*[ size ];
	num = 0;
	putp = 0;
	getp = 0;
	que[0] = 0;
}

template <class T>
ObjectQueT<T>::~ObjectQueT()
{
	clear();
	delete[] que;
}

template <class T>
void ObjectQueT<T>::clear( )
{
	while ( putp != getp ){
		delete get();
	}
}

template <class T>
void ObjectQueT<T>::put( T *obj )
{
	if ( num >= size ){
		delete get();
	}
	que[putp++] = obj;
	num++;
	if ( putp >= size ){
		putp = 0;
	}
}

template <class T>
T *ObjectQueT<T>::get( )
{
	if ( num == 0 ){
		return 0;
	}
	T *t = que[getp++];
	num--;
	if ( getp >= size ){
		getp = 0;
	}
	return t;
}

template <class T>
T *ObjectQueT<T>::pop( )
{
	if ( num == 0 ){
		return 0;
	}
	if ( putp == 0 ){
		putp = size;
	}
	num--;
	return que[--putp];
}


// いらない？ 2014.11.5
class ObjectQueBase {
	protected:
		void **que;
		int num;
		int putp;
		int getp;
		int size;
	protected:
		virtual void delete_object( void *obj ) = 0;
	public:
		ObjectQueBase( int _size );
		virtual ~ObjectQueBase();
		void put( void *obj );
		void *get( void );
		void clear( void );
		//特殊な操作(スタックと同様に扱える)
		void *pop( void );
		int get_num( void )	{return num;}
		int is_full( )
			{ return num == size; }
};


template <class T>
class ObjectQue : public ObjectQueBase {
	protected:
		virtual void delete_object( void *obj );
	public:
		ObjectQue( int size )
			:ObjectQueBase( size ){}
		~ObjectQue();
};

template <class T>
void ObjectQue<T>::delete_object( void *obj )
{
	delete ( T* )obj;
}

template <class T>
ObjectQue<T>::~ObjectQue( )
{
	clear( );
	if ( que ){
		delete que;
		que = 0;
	}
}

#endif	// __QUE_H
