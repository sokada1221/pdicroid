#ifndef	__PDWMEM_H
#define	__PDWMEM_H

class PdicMem {
public:
    PdicMem(){}
    int init(uint ){ return 0; }
    void *alloc(uint size)
//        { return (void *)(new tchar[ size ]); }
		{ return (void*)::malloc( size ); }
	void free(void *ptr)
//		{ delete (tchar*)ptr; }
		{ ::free( ptr ); }
	void *realloc( void *ptr, uint size )
        { return (void*)::realloc( ptr, size ); }
    void finish( ){}
    void lock( ){}
    void unlock(){}
};

#endif	// __PDWMEM_H
